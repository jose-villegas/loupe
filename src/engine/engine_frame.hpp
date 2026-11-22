// The z mirror between autana's frame and glTF's. autana's camera looks down
// +z with x right and y up; glTF's and OpenGL's look down -z. The pack mirrors
// every glTF entry in (asset/engine_frame.py), so a camera path's keys are
// mirrored to meet the engine's camera, and the handles, which work in
// OpenGL's convention, see every matrix mirrored. The mirror is its own inverse.
#pragma once

#include "engine/pose.hpp"

namespace loupe {

// `point` with z negated.
vec3f_t engine_frame(const vec3f_t& point);
// `pose` mirrored: its position's z flips, and with it its turn's x and y.
transformf_t engine_frame(const transformf_t& pose);
// `matrix` with z negated on both sides: S * matrix * S, S = diag(1, 1, -1, 1).
mat4f_t engine_frame(const mat4f_t& matrix);

} // namespace loupe
