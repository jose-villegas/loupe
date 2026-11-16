#include "window/editor.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <format>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "editor/placement.hpp"
#include "paths.hpp"
#include "tools.hpp"

namespace loupe::window {

namespace {

constexpr char SCENE_SUFFIX[] = ".scene.toml";
// Where the handles' depth range ends, in near planes: past any scene's far wall.
constexpr float FAR_IN_NEARS = 1e6F;
// A click picks the object whose origin lands nearest it, within this many screen pixels.
constexpr float PICK_RADIUS_PX = 24.0F;
// The view cube's side and its inset from the frame's top right, in screen pixels.
constexpr float CUBE_PX = 96.0F;
constexpr float CUBE_INSET_PX = 8.0F;
constexpr float CUBE_DISTANCE = 8.0F;
constexpr ImU32 CUBE_BACKGROUND = IM_COL32(0, 0, 0, 0);
// Inspector drag speeds: scene units, degrees and scale per pixel.
constexpr float POSITION_SPEED = 0.01F;
constexpr float ROTATION_SPEED = 0.5F;
constexpr float SCALE_SPEED = 0.005F;
constexpr ImVec4 BAKED_COLOUR = {0.95F, 0.75F, 0.3F, 1.0F};

bool
is_scene_file(const std::filesystem::path& scene) {
    return utf8(scene.filename()).ends_with(SCENE_SUFFIX);
}

// What the selected object shows now: the drag's placement while it lasts.
editor::Placement
shown_placement(const Editor& editor, const editor::Object& object) {
    return editor.drag && editor.drag->object == object.path ? editor.drag->now : object.local;
}

// Moves the drag on `object` to `now`, starting it from the object's placement.
void
drag_to(Editor& editor, const editor::Object& object, const editor::Placement& now) {
    if (!editor.drag || editor.drag->object != object.path) {
        editor.drag = Drag{.object = object.path, .before = object.local, .now = now};
    } else {
        editor.drag->now = now;
    }
}

// Ends the drag as one Place edit, or as nothing when it came back to where it began.
void
end_drag(Editor& editor) {
    if (editor.drag && editor.drag->now != editor.drag->before) {
        editor.edits.push(std::make_unique<editor::Place>(editor.drag->object, editor.drag->before, editor.drag->now),
                          editor.docs);
    }
    editor.drag.reset();
}

// `placement` with the handle's own part taken from `moved`, rounded; the other parts stay exact.
editor::Placement
handle_result(Handle handle, const editor::Placement& placement, const editor::Placement& moved) {
    editor::Placement out = placement;
    const auto round = [](const std::array<float, 3>& values, float step) {
        return std::array<float, 3>{editor::rounded(values[0], step), editor::rounded(values[1], step),
                                    editor::rounded(values[2], step)};
    };
    switch (handle) {
        case Handle::Move: out.position = round(moved.position, editor::UNIT_STEP); break;
        case Handle::Rotate: out.rotation_deg = round(moved.rotation_deg, editor::DEGREE_STEP); break;
        case Handle::Scale: out.scale = round(moved.scale, editor::UNIT_STEP); break;
    }
    return out;
}

ImGuizmo::OPERATION
operation(Handle handle) {
    switch (handle) {
        case Handle::Rotate: return ImGuizmo::ROTATE;
        case Handle::Scale: return ImGuizmo::SCALE;
        case Handle::Move: break;
    }
    return ImGuizmo::TRANSLATE;
}

// Where `world` (column-major, unmirrored) puts its origin on screen, through `view` and `projection` over the
// frame at `min`..`max`; none behind the eye.
std::optional<ImVec2>
screen_origin(const editor::Matrix& world, const editor::Matrix& view, const editor::Matrix& projection, ImVec2 min,
              ImVec2 max) {
    const editor::Matrix clip = editor::multiply(projection, editor::multiply(view, editor::mirror_z(world)));
    const float w = clip[15];
    if (w <= 0.0F) {
        return std::nullopt;
    }
    const float x = clip[12] / w, y = clip[13] / w;
    return ImVec2(min.x + (x + 1.0F) * 0.5F * (max.x - min.x), min.y + (1.0F - y) * 0.5F * (max.y - min.y));
}

// Selects the object whose origin lands nearest the click at `mouse`, within PICK_RADIUS_PX.
void
pick(Editor& editor, const Frames& frames, const editor::Matrix& view, const editor::Matrix& projection, ImVec2 min,
     ImVec2 max, ImVec2 mouse) {
    float nearest = PICK_RADIUS_PX * PICK_RADIUS_PX;
    for (const editor::Object& object : editor.docs.scene.objects) {
        const std::optional<Matrix4> parent = frames.parent_world(object.path);
        if (!parent) {
            continue;
        }
        const editor::Matrix world = editor::multiply(*parent, editor::to_matrix(shown_placement(editor, object)));
        if (const std::optional<ImVec2> at = screen_origin(world, view, projection, min, max)) {
            const float dx = at->x - mouse.x, dy = at->y - mouse.y;
            if (dx * dx + dy * dy < nearest) {
                nearest = dx * dx + dy * dy;
                editor.selected = object.path;
            }
        }
    }
}

// One inspector row: `label`'s three numbers, dragged or typed. A change moves the drag; letting go ends it.
void
placement_row(Editor& editor, const editor::Object& object, const char* label, size_t part, float speed) {
    editor::Placement placement = shown_placement(editor, object);
    std::array<float, 3>& values = part == 0   ? placement.position
                                   : part == 1 ? placement.rotation_deg
                                               : placement.scale;
    if (ImGui::DragFloat3(label, values.data(), speed, 0.0F, 0.0F, "%.4g")) {
        drag_to(editor, object, placement);
    }
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        end_drag(editor);
    }
}

void
outliner_node(Editor& editor, const std::vector<editor::Object>& objects, const editor::Object& object) {
    std::vector<const editor::Object*> children;
    for (const editor::Object& other : objects) {
        if (other.parent == object.path) {
            children.push_back(&other);
        }
    }
    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (children.empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }
    if (editor.selected == object.path) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    const bool open = ImGui::TreeNodeEx(object.path.c_str(), flags, "%s", object.path.c_str());
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        editor.selected = object.path;
    }
    ImGui::SameLine();
    ImGui::TextDisabled("%s%s", object.kind.c_str(), object.baked ? ", baked" : "");
    if (open) {
        for (const editor::Object* child : children) {
            outliner_node(editor, objects, *child);
        }
        ImGui::TreePop();
    }
}

// The bake check's result, once it is in.
void
collect_check(Editor& editor) {
    if (editor.checking.valid() && editor.checking.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        editor.checked = editor.checking.get();
    }
}

// `text` in the disabled colour, wrapped to the panel.
void
hint(const char* text) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
    ImGui::TextWrapped("%s", text);
    ImGui::PopStyleColor();
}

} // namespace

std::optional<Editor>
open_editor(const std::filesystem::path& scene, std::string* why) {
    if (!is_scene_file(scene)) {
        *why = "a pack has no scene file to edit; open its .scene.toml to place objects";
        return std::nullopt;
    }
    std::optional<editor::SceneDoc> doc = editor::read_scene(scene, why);
    if (!doc) {
        return std::nullopt;
    }
    Editor editor;
    editor.docs.scene = std::move(*doc);
    for (const editor::Object& object : editor.docs.scene.objects) {
        editor.sent[object.path] = object.local;
    }
    return editor;
}

EditAsk
edit_keys(Editor& editor, bool letters) {
    EditAsk ask;
    ask.undo = ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Z);
    ask.redo = ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Y)
               || ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Z);
    ask.save = ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_S);
    if (letters && !ImGui::GetIO().WantTextInput) {
        if (ImGui::IsKeyPressed(ImGuiKey_W, false)) {
            editor.handle = Handle::Move;
        } else if (ImGui::IsKeyPressed(ImGuiKey_E, false)) {
            editor.handle = Handle::Rotate;
        } else if (ImGui::IsKeyPressed(ImGuiKey_R, false)) {
            editor.handle = Handle::Scale;
        }
    }
    return ask;
}

void
apply(Editor& editor, const EditAsk& ask) {
    if (editor.drag) {
        return; // a drag in progress ends first
    }
    if (ask.undo) {
        (void)editor.edits.undo(editor.docs);
    }
    if (ask.redo) {
        (void)editor.edits.redo(editor.docs);
    }
    if (!ask.save) {
        return;
    }
    std::string why;
    if (!editor::save_scene(editor.docs.scene, &why)) {
        editor.status = "Not saved: " + why;
        std::fprintf(stderr, "loupe: %s\n", editor.status.c_str());
        return;
    }
    editor.status = "Saved " + utf8(editor.docs.scene.toml.filename());
    editor.checked.reset();
    editor.checking = std::async(std::launch::async, editor::rebuild, editor.docs.scene.toml);
}

void
draw_outliner(Editor& editor) {
    const std::vector<editor::Object>& objects = editor.docs.scene.objects;
    for (const editor::Object& object : objects) {
        if (object.parent.empty() || editor.docs.scene.find(object.parent) == nullptr) {
            outliner_node(editor, objects, object);
        }
    }
}

void
draw_inspector(Editor& editor) {
    collect_check(editor);
    const editor::Object* object = editor.docs.scene.find(editor.selected);
    if (object == nullptr) {
        hint("Select an object in the outliner or the viewport");
    } else {
        ImGui::TextUnformatted(object->path.c_str());
        ImGui::SameLine();
        ImGui::TextDisabled("%s", object->kind.c_str());
        if (object->baked) {
            ImGui::TextColored(BAKED_COLOUR, "Baked: locked");
            ImGui::SetItemTooltip(
                "A baked renderer (bake = true) is traced where it sits, at identity: it does not move");
        } else if (!editor::placeable(*object)) {
            hint("Lights and cameras keep their placement here: they change what every bake traces");
        }
        ImGui::BeginDisabled(!editor::placeable(*object));
        placement_row(editor, *object, "position", 0, POSITION_SPEED);
        placement_row(editor, *object, "rotation", 1, ROTATION_SPEED);
        placement_row(editor, *object, "scale", 2, SCALE_SPEED);
        ImGui::EndDisabled();
    }
    ImGui::Separator();
    const bool unsaved = std::ranges::any_of(editor.docs.scene.objects,
                                             [](const editor::Object& item) { return item.local != item.saved; });
    hint(unsaved ? "Unsaved changes (Ctrl+S saves)" : "No unsaved changes");
    if (!editor.status.empty()) {
        ImGui::TextWrapped("%s", editor.status.c_str());
    }
    if (editor.checking.valid()) {
        ImGui::TextDisabled("Asking the bakes what this re-keys...");
    } else if (editor.checked) {
        if (!editor.checked->ok) {
            ImGui::TextWrapped("Bake check failed: %s", editor.checked->why.c_str());
        } else if (editor.checked->rebake_rows.empty()) {
            ImGui::TextUnformatted("Re-keys no bakes");
        } else {
            ImGui::Text("Re-keys %zu bakes:", editor.checked->rebake_rows.size());
            for (const std::string& row : editor.checked->rebake_rows) {
                ImGui::BulletText("%s", row.c_str());
            }
        }
    }
}

void
draw_handle_bar(Editor& editor) {
    int handle = static_cast<int>(editor.handle);
    ImGui::RadioButton("Move (W)", &handle, static_cast<int>(Handle::Move));
    ImGui::SameLine();
    ImGui::RadioButton("Rotate (E)", &handle, static_cast<int>(Handle::Rotate));
    ImGui::SameLine();
    ImGui::RadioButton("Scale (R)", &handle, static_cast<int>(Handle::Scale));
    editor.handle = static_cast<Handle>(handle);
    ImGui::SameLine();
    ImGui::Checkbox("Local axes", &editor.local_axes);
}

void
draw_handles(Editor& editor, const Frames& frames, ImVec2 min, ImVec2 max) {
    const std::optional<ViewCamera> camera = frames.view_camera();
    if (!camera) {
        return;
    }
    editor::Matrix view = editor::view_matrix(*camera);
    const editor::Matrix projection = editor::projection_matrix(*camera, camera->near_z * FAR_IN_NEARS);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetRect(min.x, min.y, max.x - min.x, max.y - min.y);
    const editor::Object* object = editor.docs.scene.find(editor.selected);
    const std::optional<Matrix4> parent = object != nullptr ? frames.parent_world(object->path) : std::nullopt;
    bool over_handles = false;
    if (object != nullptr && parent && editor::placeable(*object)) {
        const editor::Placement now = shown_placement(editor, *object);
        editor::Matrix moved = editor::mirror_z(editor::multiply(*parent, editor::to_matrix(now)));
        const ImGuizmo::MODE mode = editor.local_axes ? ImGuizmo::LOCAL : ImGuizmo::WORLD;
        if (ImGuizmo::Manipulate(view.data(), projection.data(), operation(editor.handle), mode, moved.data())) {
            const editor::Matrix local = editor::multiply(editor::inverse_affine(*parent), editor::mirror_z(moved));
            drag_to(editor, *object, handle_result(editor.handle, now, editor::to_placement(local)));
        }
        over_handles = ImGuizmo::IsOver();
        const bool held = ImGuizmo::IsUsing();
        if (editor.handles_held && !held) {
            end_drag(editor);
        }
        editor.handles_held = held;
    }
    // The view cube shows which way the camera looks; it is the camera's own, so turning it moves nothing.
    ImGuizmo::ViewManipulate(view.data(), CUBE_DISTANCE, ImVec2(max.x - CUBE_PX - CUBE_INSET_PX, min.y + CUBE_INSET_PX),
                             ImVec2(CUBE_PX, CUBE_PX), CUBE_BACKGROUND);
    const ImVec2 mouse = ImGui::GetIO().MousePos;
    const bool inside = mouse.x >= min.x && mouse.x < max.x && mouse.y >= min.y && mouse.y < max.y;
    if (inside && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !over_handles && !editor.handles_held
        && !ImGuizmo::IsViewManipulateHovered()) {
        pick(editor, frames, editor::view_matrix(*camera), projection, min, max, mouse);
    }
}

bool
sync(Editor& editor, Frames& frames) {
    bool ok = true;
    for (const editor::Object& object : editor.docs.scene.objects) {
        const editor::Placement target = shown_placement(editor, object);
        editor::Placement& sent = editor.sent[object.path];
        if (sent != target) {
            ok &= frames.place(object.path, editor::to_transform(target));
            sent = target;
        }
    }
    return ok;
}

} // namespace loupe::window
