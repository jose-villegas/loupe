#include "window/editor.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <memory>
#include <utility>
#include <vector>

#include "editor/placement.hpp"
#include "engine/engine_frame.hpp"
#include "paths.hpp"

namespace loupe::window {

namespace {

constexpr char SCENE_SUFFIX[] = ".scene.toml";
// Where the handles' depth range ends, in near planes: past any scene's far wall.
constexpr float FAR_IN_NEARS = 1e6F;
// The view cube's side and its inset from the frame's top right, in screen pixels.
constexpr float CUBE_PX = 96.0F;
constexpr float CUBE_INSET_PX = 8.0F;
constexpr float CUBE_DISTANCE = 8.0F;
constexpr ImU32 CUBE_BACKGROUND = IM_COL32(0, 0, 0, 0);
// Inspector drag speeds per pixel for editor::PLACEMENT_FIELDS in order: scene units, degrees, scale.
constexpr std::array<float, 3> FIELD_SPEEDS = {0.01F, 0.5F, 0.005F};
constexpr ImVec4 BAKED_COLOUR = {0.95F, 0.75F, 0.3F, 1.0F};

using editor::from_gizmo;
using editor::gizmo;
using editor::GizmoMatrix;

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
        editor.edits.push(
            std::make_unique<editor::Place>(editor.drag->object,
                                            editor::Move{.before = editor.drag->before, .after = editor.drag->now}),
            editor.docs);
    }
    editor.drag.reset();
}

// `drag.before` with the handle's own part taken from `drag.after`, rounded; the other parts stay exact.
editor::Placement
handle_result(Handle handle, const editor::Move& drag) {
    editor::Placement out = drag.before;
    const editor::Placement& moved = drag.after;
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

// Selects the object whose origin lands nearest the click at `mouse`, within PICK_RADIUS_PX.
void
pick(Editor& editor, const Frames& frames, const mat4f_t& clip_from_world, const FrameRect& frame, ImVec2 mouse) {
    float nearest = PICK_RADIUS_PX * PICK_RADIUS_PX;
    for (const editor::Object& object : editor.docs.scene.objects) {
        const std::optional<mat4f_t> parent = frames.parent_world(object.path);
        if (!parent) {
            continue;
        }
        const mat4f_t world = mat4f_mul(*parent, editor::to_matrix(shown_placement(editor, object)));
        if (const std::optional<ImVec2> at = screen_origin(world, clip_from_world, frame)) {
            const float dx = at->x - mouse.x, dy = at->y - mouse.y;
            if (dx * dx + dy * dy < nearest) {
                nearest = dx * dx + dy * dy;
                editor.selected = object.path;
            }
        }
    }
}

// One inspector row per placement field, dragged or typed. A change moves the drag; letting go ends it.
void
placement_rows(Editor& editor, const editor::Object& object) {
    for (size_t i = 0; i < editor::PLACEMENT_FIELDS.size(); i++) {
        const editor::PlacementField& field = editor::PLACEMENT_FIELDS[i];
        editor::Placement placement = shown_placement(editor, object);
        if (ImGui::DragFloat3(field.name, (placement.*field.values).data(), FIELD_SPEEDS[i], 0.0F, 0.0F, "%.4g")) {
            drag_to(editor, object, placement);
        }
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            end_drag(editor);
        }
    }
}

void
outliner_node(Editor& editor, size_t index) {
    const editor::Object& object = editor.docs.scene.objects[index];
    const std::vector<size_t>& children = editor.children[index];
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
        for (const size_t child : children) {
            outliner_node(editor, child);
        }
        ImGui::TreePop();
    }
}

// The bake check's result, once it is in.
void
collect_check(Editor& editor) {
    std::future<editor::Rebuild>& result = editor.checking.result;
    if (result.valid() && result.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        editor.checked = result.get();
    }
}

// What the last save's bake check found, or that it is still asking.
void
draw_bake_check(const Editor& editor) {
    if (editor.checking.result.valid()) {
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

// The selected `object`'s handles under `parent`: a drag moves it, and letting
// go ends the drag in one edit. True while the mouse is over them.
bool
object_handles(Editor& editor, const editor::Object& object, const mat4f_t& parent, const GizmoMatrix& view,
               const GizmoMatrix& projection) {
    const editor::Placement now = shown_placement(editor, object);
    GizmoMatrix moved = gizmo(engine_frame(mat4f_mul(parent, editor::to_matrix(now))));
    const ImGuizmo::MODE mode = editor.local_axes ? ImGuizmo::LOCAL : ImGuizmo::WORLD;
    if (ImGuizmo::Manipulate(view.data(), projection.data(), operation(editor.handle), mode, moved.data())) {
        const mat4f_t local = mat4f_mul(mat4f_invert_affine(parent), engine_frame(from_gizmo(moved)));
        // A turn about the world's axes under an unevenly scaled parent can shear; a placement cannot hold that.
        if (const std::optional<editor::Placement> placed = editor::to_placement(local)) {
            drag_to(editor, object, handle_result(editor.handle, {.before = now, .after = *placed}));
        }
    }
    const bool held = ImGuizmo::IsUsing();
    if (editor.handles_held && !held) {
        end_drag(editor);
    }
    editor.handles_held = held;
    return ImGuizmo::IsOver();
}

} // namespace

BakeCheck&
BakeCheck::operator=(BakeCheck&& other) noexcept {
    stop.request_stop();
    stop = std::move(other.stop);
    result = std::move(other.result);
    return *this;
}

BakeCheck::~BakeCheck() { stop.request_stop(); }

void
BakeCheck::start(const std::filesystem::path& toml, const std::filesystem::path& folder) {
    *this = BakeCheck{};
    result = std::async(std::launch::async, editor::rebuild, toml, folder, stop.get_token());
}

Editor
editor_over(editor::SceneDoc doc) {
    Editor editor;
    editor.docs.scene = std::move(doc);
    const std::vector<editor::Object>& objects = editor.docs.scene.objects;
    editor.children.resize(objects.size());
    for (size_t i = 0; i < objects.size(); i++) {
        const auto parent = std::ranges::find(objects, objects[i].parent, &editor::Object::path);
        if (parent == objects.end()) {
            editor.roots.push_back(i);
        } else {
            editor.children[static_cast<size_t>(parent - objects.begin())].push_back(i);
        }
        editor.sent[objects[i].path] = objects[i].local;
    }
    return editor;
}

std::optional<Editor>
open_editor(const std::filesystem::path& scene, std::string* why) {
    if (!is_scene_file(scene)) {
        *why = "a pack has no scene file to edit; open its .scene.toml to place objects";
        return std::nullopt;
    }
    auto scratch = std::make_unique<Scratch>("loupe-editor-");
    if (scratch->path().empty()) {
        *why = "cannot make a scratch folder for the scene tool's logs";
        return std::nullopt;
    }
    std::optional<editor::SceneDoc> doc = editor::read_scene(scene, scratch->path(), why);
    if (!doc) {
        return std::nullopt;
    }
    Editor editor = editor_over(std::move(*doc));
    std::string unread;
    if (!load_paths(editor, scratch->path(), &unread)) {
        editor.status = "Paths not read: " + unread;
        std::fprintf(stderr, "loupe: %s\n", editor.status.c_str());
    }
    editor.scratch = std::move(scratch);
    return editor;
}

EditAsk
edit_keys(Editor& editor, bool letters) {
    EditAsk ask;
    ask.undo = ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Z);
    ask.redo = ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Y)
               || ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Z);
    ask.save = ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_S);
    if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) && !ImGui::GetIO().WantTextInput && !dragging(editor)) {
        editor.selected.clear();
        editor.path.key.reset();
    }
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
    if (dragging(editor)) {
        return; // a drag in progress ends first
    }
    if (ask.undo) {
        (void)editor.edits.undo(editor.docs);
    }
    if (ask.redo) {
        (void)editor.edits.redo(editor.docs);
    }
    if (!ask.save || editor.scratch == nullptr) {
        return;
    }
    // A running check reads the file the save writes, and writes the log the next check opens.
    editor.checking = {};
    std::string why;
    const std::filesystem::path folder = editor.scratch->path();
    if (!editor::save_scene(editor.docs.scene, folder, &why)) {
        editor.status = "Not saved: " + why;
        std::fprintf(stderr, "loupe: %s\n", editor.status.c_str());
        return;
    }
    editor.status = "Saved " + utf8(editor.docs.scene.toml.filename());
    if (!save_paths(editor, folder, &why)) {
        // The scene is written: its bake check runs, and the paths left unsaved say why.
        editor.status += "; a camera path not saved: " + why;
        std::fprintf(stderr, "loupe: %s\n", editor.status.c_str());
    }
    editor.checked.reset();
    editor.checking.start(editor.docs.scene.toml, folder);
}

void
draw_outliner(Editor& editor) {
    for (const size_t root : editor.roots) {
        outliner_node(editor, root);
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
            ImGui::SetItemTooltip("%s", object->locked.c_str());
        } else if (!editor::placeable(*object)) {
            hint(object->locked.c_str());
        }
        ImGui::BeginDisabled(!editor::placeable(*object));
        placement_rows(editor, *object);
        ImGui::EndDisabled();
        draw_path_rows(editor);
    }
    ImGui::Separator();
    hint(unsaved(editor) ? "Unsaved changes (Ctrl+S saves)" : "No unsaved changes");
    if (!editor.status.empty()) {
        ImGui::TextWrapped("%s", editor.status.c_str());
    }
    draw_bake_check(editor);
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
draw_handles(Editor& editor, const Frames& frames, const FrameRect& frame) {
    const std::optional<ViewCamera> camera = frames.view_camera();
    if (!camera) {
        return;
    }
    const mat4f_t view = editor::view_matrix(*camera);
    const mat4f_t projection = editor::projection_matrix(*camera, camera->near_z * FAR_IN_NEARS);
    const GizmoMatrix gizmo_view = gizmo(view), gizmo_projection = gizmo(projection);
    const ImVec2 min = frame.min, max = frame.max;
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetRect(min.x, min.y, max.x - min.x, max.y - min.y);
    // A picked key's handle stands in for its camera's own.
    const PathHandles on_path = draw_path(editor, frames, frame, view, projection);
    const editor::Object* object = editor.docs.scene.find(editor.selected);
    const std::optional<mat4f_t> parent = object != nullptr ? frames.parent_world(object->path) : std::nullopt;
    bool over_handles = on_path.took_click;
    if (!on_path.stands_in && object != nullptr && parent && editor::placeable(*object)) {
        over_handles = object_handles(editor, *object, *parent, gizmo_view, gizmo_projection);
    } else if (editor.handles_held) {
        // The object went away or stopped placing mid-drag: the drag ends here.
        end_drag(editor);
        editor.handles_held = false;
    }
    // The view cube shows which way the camera looks. It turns its own copy of the view, so it moves nothing.
    GizmoMatrix cube_view = gizmo_view;
    ImGuizmo::ViewManipulate(cube_view.data(), CUBE_DISTANCE,
                             ImVec2(max.x - CUBE_PX - CUBE_INSET_PX, min.y + CUBE_INSET_PX), ImVec2(CUBE_PX, CUBE_PX),
                             CUBE_BACKGROUND);
    const ImVec2 mouse = ImGui::GetIO().MousePos;
    const bool inside = mouse.x >= min.x && mouse.x < max.x && mouse.y >= min.y && mouse.y < max.y;
    if (inside && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !over_handles && !editor.handles_held
        && !ImGuizmo::IsViewManipulateHovered()) {
        pick(editor, frames, mat4f_mul(projection, view), frame, mouse);
    }
}

std::optional<ImVec2>
screen_origin(const mat4f_t& world, const mat4f_t& clip_from_world, const FrameRect& frame) {
    const ImVec2 min = frame.min, max = frame.max;
    const mat4f_t clip = mat4f_mul(clip_from_world, engine_frame(world));
    const float w = clip.m[3][3];
    if (w <= 0.0F) {
        return std::nullopt;
    }
    const float x = clip.m[0][3] / w, y = clip.m[1][3] / w;
    return ImVec2(min.x + (x + 1.0F) * 0.5F * (max.x - min.x), min.y + (1.0F - y) * 0.5F * (max.y - min.y));
}

bool
unsaved(const Editor& editor) {
    return editor.docs.scene.unsaved() || paths_unsaved(editor);
}

bool
dragging(const Editor& editor) {
    return editor.drag.has_value() || editor.path.drag.has_value();
}

bool
sync(Editor& editor, const PlaceFn& place) {
    bool ok = true;
    for (const editor::Object& object : editor.docs.scene.objects) {
        const editor::Placement target = shown_placement(editor, object);
        editor::Placement& sent = editor.sent[object.path];
        if (sent == target) {
            continue;
        }
        if (place(object.path, editor::to_transform(target))) {
            sent = target;
        } else {
            ok = false;
        }
    }
    return ok;
}

} // namespace loupe::window
