// A Placement both ways: to the engine's transform, and to and from the 4x4
// matrix the handles move. The Euler convention is autana's one: angles
// [pitch, yaw, roll] in degrees, roll about z first, then pitch about x, then
// yaw about y (quatf_from_euler, r3d/import_settings.py rotation_matrix).
#pragma once

#include <array>

#include "editor/doc.hpp"
#include "engine/frames.hpp"
#include "engine/pose.hpp"

namespace loupe::editor {

// Column-major, as ImGuizmo takes it: element [column * 4 + row].
using Matrix = std::array<float, 16>;

transformf_t to_transform(const Placement& placement);
// position * rotation * scale.
Matrix to_matrix(const Placement& placement);
// The placement `matrix` is, to rounding: scale from the basis lengths,
// rotation from the normalised basis. At pitch +-90 degrees, roll is 0 and yaw
// takes the whole turn about y.
Placement to_placement(const Matrix& matrix);

// a * b: b applied first.
Matrix multiply(const Matrix& a, const Matrix& b);
// The inverse of an affine `matrix` (last row 0 0 0 1); identity when it is singular.
Matrix inverse_affine(const Matrix& matrix);
// `matrix` with world z negated on both sides (S * matrix * S, S = diag(1, 1, -1, 1)). autana's camera looks down
// +z with x right and y up, OpenGL's down -z; the handles work in OpenGL's convention, so every matrix they see is
// mirrored through z, which keeps each one a proper rotation, and mirrored back after.
Matrix mirror_z(const Matrix& matrix);
// The OpenGL-style view (world to eye, eye looking down -z) and projection for `camera`, in the mirrored world
// mirror_z() makes. A point the engine draws at frame pixel (px, py) lands at NDC (2 px / width - 1,
// 1 - 2 py / height). `far_z` is where the depth range ends.
Matrix view_matrix(const ViewCamera& camera);
Matrix projection_matrix(const ViewCamera& camera, float far_z);

// The steps a value the handles produce is rounded to, so a save writes 0.5,
// not 0.50000006. Distances and scales in scene units, angles in degrees.
constexpr float UNIT_STEP = 1e-4F;
constexpr float DEGREE_STEP = 1e-3F;
// `value` to the nearest multiple of `step`.
float rounded(float value, float step);

} // namespace loupe::editor
