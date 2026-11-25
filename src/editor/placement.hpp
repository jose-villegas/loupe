// A Placement both ways: to the engine's transform and the matrix the handles
// move, and back. The Euler convention is autana's (quatf_from_euler and
// quatf_to_euler): angles [pitch, yaw, roll], here in degrees.
#pragma once

#include <array>
#include <optional>

#include "editor/doc.hpp"
#include "engine/frames.hpp"
#include "engine/pose.hpp"

namespace loupe::editor {

transformf_t to_transform(const Placement& placement);
// position * rotation * scale (transformf_compute_matrix).
mat4f_t to_matrix(const Placement& placement);
// The placement `matrix` is, to rounding; none for one with shear, a mirror or
// a zero scale, which a placement cannot hold (transformf_from_mat refuses it).
std::optional<Placement> to_placement(const mat4f_t& matrix);

// ImGuizmo's float[16] is column-major, element [column * 4 + row]; mat4f_t is m[row][column].
using GizmoMatrix = std::array<float, 16>;
GizmoMatrix gizmo(const mat4f_t& matrix);
mat4f_t from_gizmo(const GizmoMatrix& values);
// The OpenGL-style view (world to eye, eye looking down -z) and projection for `camera`, in the mirrored world
// engine_frame() makes. A point the engine draws at frame pixel (px, py) lands at NDC (2 px / width - 1,
// 1 - 2 py / height). `far_z` is where the depth range ends.
mat4f_t view_matrix(const ViewCamera& camera);
mat4f_t projection_matrix(const ViewCamera& camera, float far_z);

// The steps a value the handles produce is rounded to, so a save writes 0.5,
// not 0.50000006. Distances and scales in scene units, angles in degrees.
constexpr float UNIT_STEP = 1e-4F;
constexpr float DEGREE_STEP = 1e-3F;
// `value` to the nearest multiple of `step` (1/n for a whole n), as the float nearest that decimal.
float rounded(float value, float step);

} // namespace loupe::editor
