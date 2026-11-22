// A camera path as the clip autana bakes from it: the eye on a cubic curve
// (glTF CUBICSPLINE, an in-tangent, value and out-tangent per key) and the
// rotation slerped between keys, both sampled by autana's own anim_track, so
// the curve drawn is the curve the camera flies.
#pragma once

#include <vector>

#include "engine/pose.hpp"

namespace loupe {

struct PathCurve {
    std::vector<float> times;        // seconds, strictly increasing
    std::vector<float> translations; // three runs of xyz per key: in-tangent, eye, out-tangent
    std::vector<float> rotations;    // xyzw per key
};

// The camera's local pose on `curve` at `seconds`, in the keys' (glTF) frame:
// before the first key and after the last, the end key's (anim_track_sample,
// as glTF defines).
transformf_t pose_on(const PathCurve& curve, float seconds);

// A point or pose in the keys' frame mirrored into the engine's, as the pack
// boundary mirrors every entry (asset/engine_frame.py): z flips, and with it
// the turn's x and y. The mirror is its own inverse.
vec3f_t engine_frame(const vec3f_t& point);
transformf_t engine_frame(const transformf_t& pose);

} // namespace loupe
