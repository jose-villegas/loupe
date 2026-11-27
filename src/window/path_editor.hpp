// The editor's camera paths: the selected camera's path drawn over the frame
// with a handle on each key's eye and look_at, the key rows in the inspector,
// a scrub bar over the path, and the free view that flies the active camera
// by hand. Every change is a SetKeys on the editor's one EditStack, and the
// engine flies each camera on its edited path (Frames::fly) until it is the
// path the pack was built with again.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "editor/camera_path.hpp"
#include "engine/frames.hpp"
#include "window/observer.hpp"
#include "window/panels.hpp"

namespace loupe::window {

struct Editor;

// The picked key, which of its points the handle moves, and the camera whose
// path it is on: a pick on another camera's path is no pick.
struct KeyPick {
    size_t index = 0;
    editor::KeyPoint point = editor::KeyPoint::Eye;
    std::string camera;
};

// While a key is dragged, by its handle or an inspector field: the camera, its
// keys when the drag began, and now. The drag ends in one SetKeys.
struct KeyDrag {
    std::string camera;
    std::vector<editor::CameraKey> before, now;
};

// The paths' editing state: the pick, the drag, the cameras whose bake
// warning was accepted, each path as the pack was built with it, what the
// engine flies each camera on (absent: its own clip), the free view (its
// camera, and whether it moved since the engine was told), and the shown
// frame's time of play, where a key is added.
struct PathEdit {
    std::optional<KeyPick> key;
    std::optional<KeyDrag> drag;
    bool handles_held = false;
    std::set<std::string> warned;
    std::map<std::string, std::vector<editor::CameraKey>> packed, flown;
    Observer free;
    std::string free_camera;
    bool free_moved = false;
    uint32_t shown_ms = 0;
};

// Opens `path` as `camera`'s, the path the pack was built with.
void open_path(Editor& editor, const std::string& camera, editor::CameraPath path);
// open_path on the keys file of every camera on one; false with `why` set when one would not read.
bool load_paths(Editor& editor, const std::filesystem::path& folder, std::string* why);
// The selected camera's path, when it has one the editor can write.
const editor::CameraPath* selected_path(const Editor& editor);
// `camera`'s keys as shown: the drag's while it lasts.
const std::vector<editor::CameraKey>& shown_keys(const Editor& editor, const std::string& camera);
// Whether `camera`'s path may be edited now: no bake reads it, or its warning was accepted.
bool may_edit(const Editor& editor, const std::string& camera);
// Moves the drag on `camera` to `now`, starting it from the camera's keys; nothing while !may_edit.
void drag_keys(Editor& editor, const std::string& camera, std::vector<editor::CameraKey> now);
// Ends the key drag as one SetKeys, or as nothing when it came back to where it began.
void end_key_drag(Editor& editor);
// Adds a key to the selected path at `seconds` on its curve and picks it; false when one sits there or !may_edit.
bool add_key(Editor& editor, float seconds);
// Deletes the picked key; false with `why` set when it may not go (delete_refusal, or !may_edit).
bool delete_key(Editor& editor, std::string* why);
// Whether any path holds keys other than its file's.
bool paths_unsaved(const Editor& editor);
// Writes every changed path into its file (save_path); false with `why` set at the first that fails.
bool save_paths(Editor& editor, const std::filesystem::path& folder, std::string* why);

// Flies a camera on a pose, or lands it back on its clip: Frames::fly and land, or a test's stand-ins.
using FlyFn = std::function<bool(const std::string& camera, Frames::PoseAt pose)>;
using LandFn = std::function<bool(const std::string& camera)>;
// Flies every camera whose shown path is not the one the pack was built with
// on that path, once per change, and lands each that is again; a path with a
// fault stays as it last flew. The free view's camera flies the free view's
// pose instead while it is on, and lands when it goes off. False when a fly
// or land failed; it is tried again on the next sync.
bool sync_paths(Editor& editor, const FlyFn& fly, const LandFn& land);

// Where play stands: the shown frame's time of play, and the dt between frames.
struct Play {
    uint32_t shown_ms = 0;
    uint32_t dt_ms = 0;
};

// The frame `seconds` along `keys`' path on the lap the shown frame is in; never before Frames::FIRST_SEEKABLE.
int frame_at(const std::vector<editor::CameraKey>& keys, float seconds, Play play);

// What draw_path's handles hold this frame: `stands_in` when a key is picked,
// so its handle stands in for the camera's own; `took_click` when the mouse is
// on that handle or a click picked a point, so the object pick yields.
struct PathHandles {
    bool stands_in = false;
    bool took_click = false;
};

// The selected camera's path over the frame: the curve, each key's eye and
// look_at joined, the picked key bright, a marker at the shown time, and the
// handle on the picked point; a click near a point picks it. `view` and
// `projection` are the handles' (editor::view_matrix, projection_matrix).
PathHandles draw_path(Editor& editor, const Frames& frames, const FrameRect& frame, const mat4f_t& view,
                      const mat4f_t& projection);
// The picked key's field rows' labels: its time and its two points.
constexpr std::array<const char*, 3> KEY_FIELD_LABELS = {"t (s)", "eye", "look_at"};
// The selected camera's path rows: the keys, the picked key's time and points
// as field rows starting `column` along (field_column), Add key and Delete
// key; a .glb path says it is read-only, and a path a bake reads asks before
// the first edit.
void draw_path_rows(Editor& editor, float column);
// A scrub bar over the selected camera's path that sets `seek_to` to the frame at the time picked.
void draw_path_scrub(Editor& editor, const Frames& frames, std::optional<int>* seek_to);
// The free view's checkbox: on, the active camera flies by hand (the
// observer's steering) instead of on its path, so the path can be seen from
// outside. True when it was turned on or off.
bool draw_free_view_toggle(Editor& editor, const Frames& frames);
// Flies the free view on this tick's steering; true when it moved.
bool drive_free_view(Editor& editor, float dt_seconds);

} // namespace loupe::window
