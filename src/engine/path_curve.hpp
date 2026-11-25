// A camera path as the clip autana bakes from it: the eye on a cubic curve
// (glTF CUBICSPLINE, an in-tangent, value and out-tangent per key) and the
// rotation slerped between keys, both sampled by autana's own anim_track, so
// the curve drawn is the curve the camera flies.
#pragma once

#include <cstddef>
#include <vector>

#include "engine/pose.hpp"

namespace loupe {

// The most keys a curve holds: anim_track's key count.
extern const size_t MAX_CURVE_KEYS;

struct PathCurve {
    std::vector<float> times;        // seconds, strictly increasing
    std::vector<float> translations; // three runs of xyz per key: in-tangent, eye, out-tangent
    std::vector<float> rotations;    // xyzw per key
};

// The camera's local pose on `curve` at `seconds`, in the keys' (glTF) frame:
// before the first key and after the last, the end key's (anim_track_sample,
// as glTF defines).
transformf_t pose_on(const PathCurve& curve, float seconds);

} // namespace loupe
