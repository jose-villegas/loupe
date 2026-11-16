#include "editor/placement.hpp"

#include <cmath>

extern "C" {
#include "math/scalar/math_const.h"
}

namespace loupe::editor {

namespace {

// Past this |sin(pitch)| the pitch is +-90 degrees and yaw and roll turn about the same axis.
constexpr float GIMBAL_LOCK = 0.99999F;

// Rows of autana's rotation for `degrees` (import_settings.rotation_matrix): Ry * Rx * Rz.
std::array<std::array<float, 3>, 3>
rotation_rows(const std::array<float, 3>& degrees) {
    const float pitch = degrees[0] * MATH_RADIANS_PER_DEGREE;
    const float yaw = degrees[1] * MATH_RADIANS_PER_DEGREE;
    const float roll = degrees[2] * MATH_RADIANS_PER_DEGREE;
    const float sx = std::sin(pitch), cx = std::cos(pitch);
    const float sy = std::sin(yaw), cy = std::cos(yaw);
    const float sz = std::sin(roll), cz = std::cos(roll);
    return {{{cy * cz + sy * sx * sz, -cy * sz + sy * sx * cz, sy * cx},
             {cx * sz, cx * cz, -sx},
             {-sy * cz + cy * sx * sz, sy * sz + cy * sx * cz, cy * cx}}};
}

} // namespace

float
rounded(float value, float step) {
    const float steps = std::round(value / step);
    return steps == 0.0F ? 0.0F : steps * step; // never -0
}

transformf_t
to_transform(const Placement& placement) {
    const auto& [x, y, z] = placement.position;
    const auto& [pitch, yaw, roll] = placement.rotation_deg;
    const auto& [sx, sy, sz] = placement.scale;
    transformf_t transform = transformf_identity();
    transformf_set_position(&transform, vec3f_t{x, y, z});
    transformf_set_rotation(&transform,
                            quatf_from_euler(vec3f_t{pitch * MATH_RADIANS_PER_DEGREE, yaw * MATH_RADIANS_PER_DEGREE,
                                                     roll * MATH_RADIANS_PER_DEGREE}));
    transformf_set_scale(&transform, vec3f_t{sx, sy, sz});
    return transform;
}

Matrix
to_matrix(const Placement& placement) {
    const auto rows = rotation_rows(placement.rotation_deg);
    Matrix matrix{};
    for (int column = 0; column < 3; column++) {
        for (int row = 0; row < 3; row++) {
            matrix[column * 4 + row] = rows[row][column] * placement.scale[column];
        }
        matrix[12 + column] = placement.position[column];
    }
    matrix[15] = 1.0F;
    return matrix;
}

Placement
to_placement(const Matrix& matrix) {
    Placement placement;
    std::array<std::array<float, 3>, 3> rows{};
    for (int column = 0; column < 3; column++) {
        const float* basis = &matrix[column * 4];
        const float length = std::sqrt(basis[0] * basis[0] + basis[1] * basis[1] + basis[2] * basis[2]);
        placement.scale[column] = length;
        for (int row = 0; row < 3; row++) {
            rows[row][column] = length > 0.0F ? basis[row] / length : 0.0F;
        }
        placement.position[column] = matrix[12 + column];
    }
    const float sin_pitch = -rows[1][2];
    float pitch = 0.0F, yaw = 0.0F, roll = 0.0F;
    if (std::fabs(sin_pitch) < GIMBAL_LOCK) {
        pitch = std::asin(sin_pitch);
        yaw = std::atan2(rows[0][2], rows[2][2]);
        roll = std::atan2(rows[1][0], rows[1][1]);
    } else {
        pitch = std::copysign(MATH_PI / 2, sin_pitch);
        yaw = std::atan2(-rows[2][0], rows[0][0]);
    }
    placement.rotation_deg = {pitch * MATH_DEGREES_PER_RADIAN, yaw * MATH_DEGREES_PER_RADIAN,
                              roll * MATH_DEGREES_PER_RADIAN};
    return placement;
}

Matrix
multiply(const Matrix& a, const Matrix& b) {
    Matrix out{};
    for (int column = 0; column < 4; column++) {
        for (int row = 0; row < 4; row++) {
            float sum = 0.0F;
            for (int k = 0; k < 4; k++) {
                sum += a[k * 4 + row] * b[column * 4 + k];
            }
            out[column * 4 + row] = sum;
        }
    }
    return out;
}

Matrix
inverse_affine(const Matrix& matrix) {
    // The 3x3 part's inverse by cofactors; m(r, c) reads row r, column c.
    const auto m = [&matrix](int row, int column) { return matrix[column * 4 + row]; };
    const float c00 = m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1);
    const float c01 = m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2);
    const float c02 = m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0);
    const float determinant = m(0, 0) * c00 + m(0, 1) * c01 + m(0, 2) * c02;
    Matrix out{};
    out[0] = out[5] = out[10] = out[15] = 1.0F;
    if (determinant == 0.0F) {
        return out;
    }
    const float r = 1.0F / determinant;
    const std::array<std::array<float, 3>, 3> inverse = {{
        {c00 * r, (m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2)) * r, (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) * r},
        {c01 * r, (m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0)) * r, (m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2)) * r},
        {c02 * r, (m(0, 1) * m(2, 0) - m(0, 0) * m(2, 1)) * r, (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) * r},
    }};
    for (int row = 0; row < 3; row++) {
        float moved = 0.0F;
        for (int column = 0; column < 3; column++) {
            out[column * 4 + row] = inverse[row][column];
            moved += inverse[row][column] * m(column, 3);
        }
        out[12 + row] = -moved;
    }
    return out;
}

Matrix
mirror_z(const Matrix& matrix) {
    Matrix out = matrix;
    for (int i = 0; i < 4; i++) {
        out[2 * 4 + i] = -out[2 * 4 + i]; // column 2
        out[i * 4 + 2] = -out[i * 4 + 2]; // row 2
    }
    return out;
}

Matrix
view_matrix(const ViewCamera& camera) {
    // Eye axes in the mirrored world: x right, y up, z back (OpenGL's).
    const auto mirrored = [](vec3f_t v) { return vec3f_t{v.x, v.y, -v.z}; };
    const vec3f_t right = mirrored(camera.right), up = mirrored(camera.up);
    const vec3f_t back = vec3f_scale(mirrored(camera.forward), -1.0F), eye = mirrored(camera.eye);
    Matrix out{};
    const std::array<vec3f_t, 3> rows = {right, up, back};
    for (int row = 0; row < 3; row++) {
        out[0 * 4 + row] = rows[row].x;
        out[1 * 4 + row] = rows[row].y;
        out[2 * 4 + row] = rows[row].z;
        out[12 + row] = -vec3f_dot(rows[row], eye);
    }
    out[15] = 1.0F;
    return out;
}

Matrix
projection_matrix(const ViewCamera& camera, float far_z) {
    const auto width = static_cast<float>(camera.width), height = static_cast<float>(camera.height);
    const float near_z = camera.near_z;
    Matrix out{};
    out[0] = 2.0F * camera.fx / width;
    out[5] = 2.0F * camera.fy / height;
    // Off-centre: the optical centre (cx, cy) is where the eye's axis meets the frame.
    out[8] = 1.0F - 2.0F * camera.cx / width;
    out[9] = 2.0F * camera.cy / height - 1.0F;
    out[10] = -(far_z + near_z) / (far_z - near_z);
    out[11] = -1.0F;
    out[14] = -2.0F * far_z * near_z / (far_z - near_z);
    return out;
}

} // namespace loupe::editor
