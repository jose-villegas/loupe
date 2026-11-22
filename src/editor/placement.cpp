#include "editor/placement.hpp"

#include <cmath>

extern "C" {
#include "math/scalar/math_const.h"
}

namespace loupe::editor {

namespace {

vec3f_t
vec(const std::array<float, 3>& values) {
    return {values[0], values[1], values[2]};
}

std::array<float, 3>
values(vec3f_t v) {
    return {v.x, v.y, v.z};
}

} // namespace

float
rounded(float value, float step) {
    // Divided by whole steps per unit, not multiplied by the inexact float step: 7500 / 1000 is 7.5 exactly.
    const double per_unit = std::round(1.0 / static_cast<double>(step));
    const double steps = std::round(static_cast<double>(value) * per_unit);
    return steps == 0.0 ? 0.0F : static_cast<float>(steps / per_unit); // never -0
}

transformf_t
to_transform(const Placement& placement) {
    transformf_t transform = transformf_identity();
    transformf_set_position(&transform, vec(placement.position));
    transformf_set_rotation(&transform,
                            quatf_from_euler(vec3f_scale(vec(placement.rotation_deg), MATH_RADIANS_PER_DEGREE)));
    transformf_set_scale(&transform, vec(placement.scale));
    return transform;
}

mat4f_t
to_matrix(const Placement& placement) {
    const transformf_t transform = to_transform(placement);
    return transformf_compute_matrix(&transform);
}

std::optional<Placement>
to_placement(const mat4f_t& matrix) {
    transformf_t transform;
    if (!transformf_from_mat(matrix, &transform)) {
        return std::nullopt;
    }
    return Placement{.position = values(transform.position),
                     .rotation_deg = values(vec3f_scale(quatf_to_euler(transform.rotation), MATH_DEGREES_PER_RADIAN)),
                     .scale = values(transform.scale)};
}

GizmoMatrix
gizmo(const mat4f_t& matrix) {
    GizmoMatrix out{};
    for (size_t row = 0; row < 4; row++) {
        for (size_t column = 0; column < 4; column++) {
            out[(column * 4) + row] = matrix.m[row][column];
        }
    }
    return out;
}

mat4f_t
from_gizmo(const GizmoMatrix& values) {
    mat4f_t out{};
    for (size_t row = 0; row < 4; row++) {
        for (size_t column = 0; column < 4; column++) {
            out.m[row][column] = values[(column * 4) + row];
        }
    }
    return out;
}

mat4f_t
mirror_z(const mat4f_t& matrix) {
    mat4f_t out = matrix;
    for (int i = 0; i < 4; i++) {
        out.m[2][i] = -out.m[2][i]; // row 2
        out.m[i][2] = -out.m[i][2]; // column 2
    }
    return out;
}

mat4f_t
view_matrix(const ViewCamera& camera) {
    // Eye axes in the mirrored world: x right, y up, z back (OpenGL's).
    const auto mirrored = [](vec3f_t v) { return vec3f_t{v.x, v.y, -v.z}; };
    return mat4f_view_from_axes(mirrored(camera.right), mirrored(camera.up),
                                vec3f_scale(mirrored(camera.forward), -1.0F), mirrored(camera.eye));
}

mat4f_t
projection_matrix(const ViewCamera& camera, float far_z) {
    const auto width = static_cast<float>(camera.width), height = static_cast<float>(camera.height);
    const float near_z = camera.near_z;
    mat4f_t out{};
    out.m[0][0] = 2.0F * camera.fx / width;
    out.m[1][1] = 2.0F * camera.fy / height;
    // Off-centre: the optical centre (cx, cy) is where the eye's axis meets the frame.
    out.m[0][2] = 1.0F - 2.0F * camera.cx / width;
    out.m[1][2] = 2.0F * camera.cy / height - 1.0F;
    out.m[2][2] = -(far_z + near_z) / (far_z - near_z);
    out.m[3][2] = -1.0F;
    out.m[2][3] = -2.0F * far_z * near_z / (far_z - near_z);
    return out;
}

} // namespace loupe::editor
