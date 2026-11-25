#include "window/path_editor.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <format>
#include <memory>
#include <utility>

#include "editor/doc.hpp"
#include "editor/placement.hpp"
#include "editor/scene_tool.hpp"
#include "engine/engine_frame.hpp"
#include "window/editor.hpp"

namespace loupe::window {

namespace {

using editor::CameraKey;
using editor::KeyPoint;

// Points along the drawn curve: enough that a 45 s tour reads as a curve.
constexpr int CURVE_SAMPLES = 256;
constexpr float EYE_RADIUS_PX = 5.0F, LOOK_RADIUS_PX = 3.0F, MARKER_RADIUS_PX = 6.0F, LINE_PX = 2.0F;
constexpr ImU32 CURVE_COLOUR = IM_COL32(255, 200, 60, 220);
constexpr ImU32 KEY_COLOUR = IM_COL32(255, 255, 255, 200);
constexpr ImU32 PICKED_COLOUR = IM_COL32(80, 220, 255, 255);
constexpr ImU32 MARKER_COLOUR = IM_COL32(255, 90, 90, 255);
constexpr ImVec4 WARNING_COLOUR = {0.95F, 0.75F, 0.3F, 1.0F};
// Inspector drag speeds per pixel: seconds, scene units.
constexpr float TIME_SPEED = 0.01F, POINT_SPEED = 0.05F;

// Whether two key lists are the same path: a key's id is not part of it.
bool
same_path(const std::vector<CameraKey>& a, const std::vector<CameraKey>& b) {
    return std::ranges::equal(a, b, [](const CameraKey& x, const CameraKey& y) {
        return x.t == y.t && x.eye == y.eye && x.look_at == y.look_at;
    });
}

// The picked key on `camera`'s path, when it is one of its `count` keys.
std::optional<KeyPick>
pick_on(const Editor& editor, const std::string& camera, size_t count) {
    const std::optional<KeyPick>& key = editor.path.key;
    return key && key->camera == camera && key->index < count ? key : std::nullopt;
}

// Pushes `after` as `camera`'s keys in one SetKeys.
void
set_keys(Editor& editor, const std::string& camera, std::vector<CameraKey> after) {
    editor.edits.push(std::make_unique<editor::SetKeys>(camera, editor.docs.paths.at(camera).keys, std::move(after)),
                      editor.docs);
}

editor::Point&
point_of(CameraKey& key, KeyPoint point) {
    return point == KeyPoint::Eye ? key.eye : key.look_at;
}

const editor::Point&
point_of(const CameraKey& key, KeyPoint point) {
    return point == KeyPoint::Eye ? key.eye : key.look_at;
}

// The world matrix standing at `point`, in the keys' frame under `parent`; unmirrored, as the object handles take it.
mat4f_t
at_point(const mat4f_t& parent, const editor::Point& point) {
    const vec3f_t engine = engine_frame(vec3f_t{point[0], point[1], point[2]});
    mat4f_t local = mat4f_identity();
    local.m[0][3] = engine.x;
    local.m[1][3] = engine.y;
    local.m[2][3] = engine.z;
    return mat4f_mul(parent, local);
}

// Where path points land over the frame.
struct PathView {
    mat4f_t parent{}, clip_from_world{};
    FrameRect frame;

    std::optional<ImVec2>
    screen(const editor::Point& point) const {
        return screen_origin(at_point(parent, point), clip_from_world, frame);
    }
};

editor::Point
point(const vec3f_t& value) {
    return {value.x, value.y, value.z};
}

void
draw_curve(ImDrawList& draw, const PathView& view, const std::vector<CameraKey>& keys) {
    const PathCurve curve = editor::path_curve(keys);
    const float end = editor::path_end(keys);
    std::optional<ImVec2> last;
    for (int i = 0; i <= CURVE_SAMPLES; i++) {
        const std::optional<ImVec2> at =
            view.screen(point(pose_on(curve, end * static_cast<float>(i) / CURVE_SAMPLES).position));
        if (last && at) {
            draw.AddLine(*last, *at, CURVE_COLOUR, LINE_PX);
        }
        last = at;
    }
}

void
draw_keys(ImDrawList& draw, const PathView& view, const std::vector<CameraKey>& keys, std::optional<size_t> picked) {
    for (size_t i = 0; i < keys.size(); i++) {
        const ImU32 colour = picked == i ? PICKED_COLOUR : KEY_COLOUR;
        const std::optional<ImVec2> eye = view.screen(keys[i].eye), look = view.screen(keys[i].look_at);
        if (eye && look) {
            draw.AddLine(*eye, *look, colour);
        }
        if (eye) {
            draw.AddCircleFilled(*eye, EYE_RADIUS_PX, colour);
        }
        if (look) {
            draw.AddCircle(*look, LOOK_RADIUS_PX, colour);
        }
    }
}

// The key point nearest `mouse` on screen, within PICK_RADIUS_PX.
std::optional<KeyPick>
nearest_point(const PathView& view, const std::vector<CameraKey>& keys, const std::string& camera, ImVec2 mouse) {
    float nearest = PICK_RADIUS_PX * PICK_RADIUS_PX;
    std::optional<KeyPick> found;
    for (size_t i = 0; i < keys.size(); i++) {
        for (const KeyPoint which : {KeyPoint::Eye, KeyPoint::LookAt}) {
            if (const std::optional<ImVec2> at = view.screen(point_of(keys[i], which))) {
                const float dx = at->x - mouse.x, dy = at->y - mouse.y;
                if (dx * dx + dy * dy < nearest) {
                    nearest = dx * dx + dy * dy;
                    found = KeyPick{.index = i, .point = which, .camera = camera};
                }
            }
        }
    }
    return found;
}

// The move handle on the picked point; a drag moves it, letting go ends the drag in one SetKeys.
void
point_handle(Editor& editor, const KeyPick& pick, const std::vector<CameraKey>& keys, const mat4f_t& parent,
             const editor::GizmoMatrix& view, const editor::GizmoMatrix& projection) {
    editor::GizmoMatrix moved = editor::gizmo(engine_frame(at_point(parent, point_of(keys[pick.index], pick.point))));
    if (ImGuizmo::Manipulate(view.data(), projection.data(), ImGuizmo::TRANSLATE, ImGuizmo::WORLD, moved.data())) {
        const mat4f_t local = mat4f_mul(mat4f_invert_affine(parent), engine_frame(editor::from_gizmo(moved)));
        // The mirror is its own inverse: the engine's point back in the keys' frame.
        const vec3f_t keys_frame = engine_frame(vec3f_t{local.m[0][3], local.m[1][3], local.m[2][3]});
        CameraKey key = keys[pick.index];
        point_of(key, pick.point) = {editor::rounded(keys_frame.x, editor::UNIT_STEP),
                                     editor::rounded(keys_frame.y, editor::UNIT_STEP),
                                     editor::rounded(keys_frame.z, editor::UNIT_STEP)};
        drag_keys(editor, pick.camera, editor::with_key(keys, pick.index, key));
    }
    const bool held = ImGuizmo::IsUsing();
    if (editor.path.handles_held && !held) {
        end_key_drag(editor);
    }
    editor.path.handles_held = held;
}

// The picked key's time and points as fields, and which point the handle moves. A change drags; letting go ends it.
void
key_fields(Editor& editor, const KeyPick& pick, const std::vector<CameraKey>& keys) {
    CameraKey key = keys[pick.index];
    bool changed = false;
    ImGui::BeginDisabled(pick.index == 0);
    if (ImGui::DragFloat("t (s)", &key.t, TIME_SPEED, 0.0F, 0.0F, "%.3f")) {
        key.t = editor::clamp_time(keys, pick.index, key.t);
        changed = true;
    }
    bool ended = ImGui::IsItemDeactivatedAfterEdit();
    ImGui::EndDisabled();
    changed |= ImGui::DragFloat3("eye", key.eye.data(), POINT_SPEED, 0.0F, 0.0F, "%.4g");
    ended |= ImGui::IsItemDeactivatedAfterEdit();
    changed |= ImGui::DragFloat3("look_at", key.look_at.data(), POINT_SPEED, 0.0F, 0.0F, "%.4g");
    ended |= ImGui::IsItemDeactivatedAfterEdit();
    if (changed) {
        drag_keys(editor, pick.camera, editor::with_key(keys, pick.index, key));
    }
    if (ended) {
        end_key_drag(editor);
    }
    int handle_on = static_cast<int>(pick.point);
    ImGui::TextUnformatted("Handle on");
    ImGui::SameLine();
    ImGui::RadioButton("eye##handle", &handle_on, static_cast<int>(KeyPoint::Eye));
    ImGui::SameLine();
    ImGui::RadioButton("look_at##handle", &handle_on, static_cast<int>(KeyPoint::LookAt));
    if (editor.path.key) {
        editor.path.key->point = static_cast<KeyPoint>(handle_on);
    }
}

// Lands the free view's camera once the view is off; true when nothing is left to do.
bool
land_free_view(PathEdit& edit, const LandFn& land) {
    if (edit.free.on || edit.free_camera.empty()) {
        return true;
    }
    if (!land(edit.free_camera)) {
        return false;
    }
    edit.flown.erase(edit.free_camera);
    edit.free_camera.clear();
    return true;
}

// Flies the free view's camera at its pose when it moved; true when nothing is left to do.
bool
fly_free_view(PathEdit& edit, const FlyFn& fly) {
    if (!edit.free.on || !edit.free_moved) {
        return true;
    }
    const transformf_t pose = edit.free.fly.pose;
    if (!fly(edit.free_camera, [pose](uint32_t /*t_ms*/) { return pose; })) {
        return false;
    }
    edit.free_moved = false;
    return true;
}

// Flies or lands `camera` to match its shown keys; true when nothing is left to do.
bool
sync_path(Editor& editor, const std::string& camera, const FlyFn& fly, const LandFn& land) {
    PathEdit& edit = editor.path;
    const std::vector<CameraKey>& keys = shown_keys(editor, camera);
    const auto flown = edit.flown.find(camera);
    if (same_path(keys, edit.packed[camera])) {
        if (flown == edit.flown.end()) {
            return true;
        }
        if (!land(camera)) {
            return false;
        }
        edit.flown.erase(flown);
        return true;
    }
    if (editor::path_fault(keys) || (flown != edit.flown.end() && same_path(flown->second, keys))) {
        return true;
    }
    const PathCurve curve = editor::path_curve(keys);
    // A failed fly may still have taken the camera: no keys stands for that, so the next sync flies or lands it.
    edit.flown[camera] = {};
    if (!fly(camera,
             [curve, keys](uint32_t t_ms) { return engine_frame(pose_on(curve, editor::clip_seconds(keys, t_ms))); })) {
        return false;
    }
    edit.flown[camera] = keys;
    return true;
}

} // namespace

void
open_path(Editor& editor, const std::string& camera, editor::CameraPath path) {
    editor.path.packed[camera] = path.keys;
    editor.docs.paths[camera] = std::move(path);
}

bool
load_paths(Editor& editor, const std::filesystem::path& folder, std::string* why) {
    for (const editor::Object& object : editor.docs.scene.objects) {
        if (!object.path_source || object.path_source->keys.empty()) {
            continue;
        }
        std::optional<editor::CameraPath> path = editor::read_path(object.path_source->keys, folder, why);
        if (!path) {
            *why = object.path + "'s path: " + *why;
            return false;
        }
        open_path(editor, object.path, std::move(*path));
    }
    return true;
}

const editor::CameraPath*
selected_path(const Editor& editor) {
    const auto found = editor.docs.paths.find(editor.selected);
    return found == editor.docs.paths.end() ? nullptr : &found->second;
}

const std::vector<CameraKey>&
shown_keys(const Editor& editor, const std::string& camera) {
    const std::optional<KeyDrag>& drag = editor.path.drag;
    return drag && drag->camera == camera ? drag->now : editor.docs.paths.at(camera).keys;
}

bool
may_edit(const Editor& editor, const std::string& camera) {
    const editor::Object* object = editor.docs.scene.find(camera);
    const bool baked = object != nullptr && object->path_source && object->path_source->feeds_bakes;
    return editor.docs.paths.contains(camera) && (!baked || editor.path.warned.contains(camera));
}

void
drag_keys(Editor& editor, const std::string& camera, std::vector<CameraKey> now) {
    if (!may_edit(editor, camera)) {
        return;
    }
    std::optional<KeyDrag>& drag = editor.path.drag;
    if (drag && drag->camera != camera) {
        end_key_drag(editor);
    }
    if (!drag) {
        drag = KeyDrag{.camera = camera, .before = editor.docs.paths.at(camera).keys, .now = std::move(now)};
    } else {
        drag->now = std::move(now);
    }
}

void
end_key_drag(Editor& editor) {
    std::optional<KeyDrag>& drag = editor.path.drag;
    if (drag && drag->now != drag->before) {
        editor.edits.push(std::make_unique<editor::SetKeys>(drag->camera, drag->before, drag->now), editor.docs);
    }
    drag.reset();
}

bool
add_key(Editor& editor, float seconds) {
    const editor::CameraPath* path = selected_path(editor);
    if (path == nullptr || !may_edit(editor, editor.selected)) {
        return false;
    }
    size_t added = 0;
    std::optional<std::vector<CameraKey>> keys = editor::with_key_added(path->keys, seconds, &added);
    if (!keys) {
        return false;
    }
    set_keys(editor, editor.selected, std::move(*keys));
    editor.path.key = KeyPick{.index = added, .point = KeyPoint::Eye, .camera = editor.selected};
    return true;
}

bool
delete_key(Editor& editor, std::string* why) {
    const editor::CameraPath* path = selected_path(editor);
    const std::optional<KeyPick> pick =
        path != nullptr ? pick_on(editor, editor.selected, path->keys.size()) : std::nullopt;
    if (!pick) {
        *why = "no key is picked";
        return false;
    }
    if (!may_edit(editor, editor.selected)) {
        *why = "a bake reads this path: accept its warning first";
        return false;
    }
    if (const std::optional<std::string> refusal = editor::delete_refusal(path->keys, pick->index)) {
        *why = *refusal;
        return false;
    }
    std::vector<CameraKey> keys = path->keys;
    keys.erase(keys.begin() + static_cast<std::ptrdiff_t>(pick->index));
    set_keys(editor, editor.selected, std::move(keys));
    editor.path.key.reset();
    return true;
}

bool
paths_unsaved(const Editor& editor) {
    return std::ranges::any_of(editor.docs.paths, [](const auto& entry) { return entry.second.unsaved(); });
}

bool
save_paths(Editor& editor, const std::filesystem::path& folder, std::string* why) {
    for (auto& [camera, path] : editor.docs.paths) {
        if (!editor::save_path(path, folder, why)) {
            *why = camera + "'s path: " + *why;
            return false;
        }
    }
    return true;
}

bool
sync_paths(Editor& editor, const FlyFn& fly, const LandFn& land) {
    bool ok = fly_free_view(editor.path, fly) && land_free_view(editor.path, land);
    for (const auto& entry : editor.docs.paths) {
        if (entry.first != editor.path.free_camera) {
            ok &= sync_path(editor, entry.first, fly, land);
        }
    }
    return ok;
}

int
frame_at(const std::vector<CameraKey>& keys, float seconds, Play play) {
    if (play.dt_ms == 0 || editor::path_end(keys) <= 0.0F) {
        return Frames::FIRST_SEEKABLE;
    }
    const auto lap_ms = static_cast<double>(editor::lap_start_ms(keys, play.shown_ms));
    const double end_ms = std::round(static_cast<double>(editor::path_end(keys)) * editor::MS_PER_S);
    const double target_ms = lap_ms + std::round(static_cast<double>(seconds) * editor::MS_PER_S);
    const auto dt = static_cast<double>(play.dt_ms);
    // Frame i is drawn at (i + 1) dts of play (Frames::time_ms). The lap's end is the next lap's start, so the
    // nearest frame stops at the last one drawn before it.
    const int last = static_cast<int>(std::ceil((lap_ms + end_ms) / dt)) - 2;
    const int frame = std::min(static_cast<int>(std::lround(target_ms / dt)) - 1, last);
    return std::max(frame, Frames::FIRST_SEEKABLE);
}

namespace {

// The marker at where the camera stands on `keys`' curve at `shown_ms` of play.
void
draw_marker(ImDrawList& draw, const PathView& on_frame, const std::vector<CameraKey>& keys, uint32_t shown_ms) {
    const transformf_t at = pose_on(editor::path_curve(keys), editor::clip_seconds(keys, shown_ms));
    if (const std::optional<ImVec2> marker = on_frame.screen(point(at.position))) {
        draw.AddCircle(*marker, MARKER_RADIUS_PX, MARKER_COLOUR, 0, LINE_PX);
    }
}

// A click on the frame, off the handle, near one of `keys`' points picks it; true when one did.
bool
click_picks(Editor& editor, const PathView& on_frame, const std::vector<CameraKey>& keys) {
    const ImVec2 mouse = ImGui::GetIO().MousePos;
    const FrameRect& frame = on_frame.frame;
    const bool inside =
        mouse.x >= frame.min.x && mouse.x < frame.max.x && mouse.y >= frame.min.y && mouse.y < frame.max.y;
    if (!inside || !ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGuizmo::IsOver()) {
        return false;
    }
    const std::optional<KeyPick> near = nearest_point(on_frame, keys, editor.selected, mouse);
    if (near) {
        editor.path.key = near;
    }
    return near.has_value();
}

} // namespace

PathHandles
draw_path(Editor& editor, const Frames& frames, const FrameRect& frame, const mat4f_t& view,
          const mat4f_t& projection) {
    editor.path.shown_ms = frames.index() > 0 ? frames.time_ms(frames.index() - 1) : 0;
    const editor::CameraPath* path = selected_path(editor);
    const std::optional<mat4f_t> parent = path != nullptr ? frames.parent_world(editor.selected) : std::nullopt;
    if (!parent) {
        return {};
    }
    const std::vector<CameraKey>& keys = shown_keys(editor, editor.selected);
    const PathView on_frame{.parent = *parent, .clip_from_world = mat4f_mul(projection, view), .frame = frame};
    ImDrawList& draw = *ImGui::GetWindowDrawList();
    // Past the frame's edges the projection shows nothing the frame does.
    draw.PushClipRect(frame.min, frame.max, true);
    const bool builds = !editor::path_fault(keys);
    if (builds) {
        draw_curve(draw, on_frame, keys);
    }
    const std::optional<KeyPick> pick = pick_on(editor, editor.selected, keys.size());
    draw_keys(draw, on_frame, keys, pick ? std::optional(pick->index) : std::nullopt);
    if (builds) {
        draw_marker(draw, on_frame, keys, editor.path.shown_ms);
    }
    draw.PopClipRect();
    bool over_handle = false;
    if (pick && may_edit(editor, editor.selected)) {
        point_handle(editor, *pick, keys, *parent, editor::gizmo(view), editor::gizmo(projection));
        over_handle = ImGuizmo::IsOver() || editor.path.handles_held;
    } else if (editor.path.handles_held) {
        end_key_drag(editor);
        editor.path.handles_held = false;
    }
    if (!editor.path.handles_held && click_picks(editor, on_frame, keys)) {
        return {.stands_in = true, .took_click = true};
    }
    return {.stands_in = pick.has_value(), .took_click = over_handle};
}

namespace {

// The warning a path a bake reads shows, and its Edit anyway button.
void
bake_warning(Editor& editor) {
    ImGui::PushStyleColor(ImGuiCol_Text, WARNING_COLOUR);
    ImGui::TextWrapped("A bake reads this path: saving an edit re-keys those bakes, and the board shows it only "
                       "once they are rebuilt.");
    ImGui::PopStyleColor();
    if (ImGui::Button("Edit anyway")) {
        editor.path.warned.insert(editor.selected);
    }
}

// A row per key, its time; a click picks it, the handle on the same point as before.
// The row's id is its index alone, so it stays one row while its time changes.
void
key_rows(Editor& editor, const std::vector<CameraKey>& keys, const std::optional<KeyPick>& pick) {
    for (size_t i = 0; i < keys.size(); i++) {
        const std::string label = std::format("key {}: t = {:.3f} s###key{}", i, keys[i].t, i);
        if (ImGui::Selectable(label.c_str(), pick && pick->index == i)) {
            editor.path.key =
                KeyPick{.index = i, .point = pick ? pick->point : KeyPoint::Eye, .camera = editor.selected};
        }
    }
}

} // namespace

void
draw_path_rows(Editor& editor) {
    const editor::Object* object = editor.docs.scene.find(editor.selected);
    if (object == nullptr || !object->path_source) {
        return;
    }
    ImGui::SeparatorText("Path");
    if (object->path_source->keys.empty()) {
        hint("This camera's path is a clip from a .glb: shown, not edited here.");
        return;
    }
    if (selected_path(editor) == nullptr) {
        hint("This camera's keys file did not read; the status below says why.");
        return;
    }
    const bool editable = may_edit(editor, editor.selected);
    if (!editable) {
        bake_warning(editor);
    }
    ImGui::BeginDisabled(!editable);
    const std::vector<CameraKey>& keys = shown_keys(editor, editor.selected);
    const std::optional<KeyPick> pick = pick_on(editor, editor.selected, keys.size());
    key_rows(editor, keys, pick);
    if (pick) {
        key_fields(editor, *pick, keys);
    }
    const float at = editor::clip_seconds(keys, editor.path.shown_ms);
    if (ImGui::Button(std::format("Add key at {:.2f} s", at).c_str())) {
        (void)add_key(editor, at);
    }
    ImGui::SameLine();
    const std::optional<std::string> refusal =
        pick ? editor::delete_refusal(keys, pick->index) : std::optional<std::string>("pick a key first");
    ImGui::BeginDisabled(refusal.has_value());
    if (ImGui::Button("Delete key")) {
        std::string why;
        (void)delete_key(editor, &why);
    }
    ImGui::EndDisabled();
    if (refusal) {
        ImGui::SetItemTooltip("%s", refusal->c_str());
    }
    ImGui::EndDisabled();
}

void
draw_path_scrub(Editor& editor, const Frames& frames, std::optional<int>* seek_to) {
    if (selected_path(editor) == nullptr) {
        return;
    }
    const std::vector<CameraKey>& keys = shown_keys(editor, editor.selected);
    float seconds = editor::clip_seconds(keys, editor.path.shown_ms);
    ImGui::SetNextItemWidth(-1.0F);
    if (ImGui::SliderFloat("##path", &seconds, 0.0F, editor::path_end(keys), "path %.2f s")) {
        *seek_to = frame_at(keys, seconds, {.shown_ms = editor.path.shown_ms, .dt_ms = frames.dt_ms()});
    }
}

bool
draw_free_view_toggle(Editor& editor, const Frames& frames) {
    PathEdit& edit = editor.path;
    bool on = edit.free.on;
    if (!ImGui::Checkbox("Free view", &on)) {
        return false;
    }
    ImGui::SetItemTooltip("Fly the camera by hand (WASD, right drag) to see its path from outside");
    if (on) {
        const auto& objects = editor.docs.scene.objects;
        const auto active = std::ranges::find_if(
            objects, [&frames](const editor::Object& object) { return frames.is_active(object.path); });
        const std::optional<transformf_t> start =
            active != objects.end() ? frames.local_pose(active->path) : std::nullopt;
        if (!start) {
            return false;
        }
        edit.free_camera = active->path;
        edit.free.fly = fly_motion_start(*start, observer_start_speed(frames.scene_box()), OBSERVER_TUNING);
        edit.free_moved = true;
    }
    edit.free.on = on;
    return true;
}

bool
drive_free_view(Editor& editor, float dt_seconds) {
    if (!editor.path.free.on || !drive_observer(editor.path.free, dt_seconds)) {
        return false;
    }
    editor.path.free_moved = true;
    return true;
}

} // namespace loupe::window
