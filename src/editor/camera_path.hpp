// A camera path as its .keys.toml writes it (autana's tools/anim/camera_keys.py
// reads it): keys with a time, the eye and the point it looks at. The curve the
// camera flies is built from them as camera_keys.py builds its clip, and
// sampled by autana's anim_track (engine/path_curve.hpp).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "engine/path_curve.hpp"
#include "engine/pose.hpp"

namespace loupe::editor {

using Point = std::array<float, 3>;

// One key. `from` is its index in the file as last read or saved, -1 for a key
// added since: a save keeps each kept key's table and comments by it.
struct CameraKey {
    float t = 0.0F; // seconds
    Point eye{}, look_at{};
    int from = -1;
    bool operator==(const CameraKey&) const = default;
};

// A key's two points, which the handles move one at a time.
enum class KeyPoint : uint8_t { Eye, LookAt };

// The path a camera flies, read from `keys_toml`. `saved` is what the file
// holds, so a save writes only a changed path. `feeds_bakes`: a bake reads the
// path, so saving a change re-keys those bakes.
struct CameraPath {
    std::filesystem::path keys_toml;
    std::vector<CameraKey> keys, saved;
    bool feeds_bakes = false;

    bool unsaved() const;
};

// The key times step: a time typed or clamped is rounded to whole milliseconds.
constexpr float TIME_STEP_S = 1e-3F;

// Whether the first and last keys are the same eye and look_at: camera_keys.py
// then curves the path through the join as a loop.
bool loops(const std::vector<CameraKey>& keys);
// Why `keys` would not build as camera_keys.py builds a path; none when they do.
std::optional<std::string> path_fault(const std::vector<CameraKey>& keys);
// The curve camera_keys.py builds: the eye's tangents are Catmull-Rom's,
// scaled by time and closed through the join when loops(); the rotation looks
// from the eye at look_at with world +Y up, glTF's camera facing local -Z.
// `keys` must have no path_fault.
PathCurve path_curve(const std::vector<CameraKey>& keys);
// The last key's time.
float path_end(const std::vector<CameraKey>& keys);
// Where on the path the camera's clip is `t_ms` into play, in seconds: the
// engine plays every clip round, from the first key again at the last's time.
float clip_seconds(const std::vector<CameraKey>& keys, uint32_t t_ms);
// A key on the curve at `seconds`: its eye the curve's, its look_at along the
// curve's forward at the look distance of the keys either side. `from` -1.
CameraKey key_on(const PathCurve& curve, const std::vector<CameraKey>& keys, float seconds);

// `keys` with keys[index]'s time and points set to `key`'s; on a loop, the
// other end of the join takes the same points, so the loop stays closed.
std::vector<CameraKey> with_key(const std::vector<CameraKey>& keys, size_t index, const CameraKey& key);
// `t` as key `index` may take it: the first key stays at 0, every other key
// stays at least TIME_STEP_S after the one before it and before the one after.
float clamp_time(const std::vector<CameraKey>& keys, size_t index, float t);
// `keys` with a key added at `seconds` on the curve; after the last key, one
// spacing past it when `seconds` is no later than it. None when a key already
// sits at that millisecond (`seconds` is rounded to TIME_STEP_S). `*added` is the new key's index.
std::optional<std::vector<CameraKey>> with_key_added(const std::vector<CameraKey>& keys, float seconds, size_t* added);
// Why key `index` may not be deleted (the first, or one of the last two); none when it may.
std::optional<std::string> delete_refusal(const std::vector<CameraKey>& keys, size_t index);

} // namespace loupe::editor
