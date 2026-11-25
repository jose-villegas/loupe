#include "engine/engine_frame.hpp"

namespace loupe {

vec3f_t
engine_frame(const vec3f_t& point) {
    return {point.x, point.y, -point.z};
}

transformf_t
engine_frame(const transformf_t& pose) {
    transformf_t mirrored = transformf_identity();
    transformf_set_position(&mirrored, engine_frame(pose.position));
    transformf_set_rotation(&mirrored, quatf_t{-pose.rotation.x, -pose.rotation.y, pose.rotation.z, pose.rotation.w});
    transformf_set_scale(&mirrored, pose.scale);
    return mirrored;
}

mat4f_t
engine_frame(const mat4f_t& matrix) {
    mat4f_t out = matrix;
    for (int i = 0; i < 4; i++) {
        out.m[2][i] = -out.m[2][i]; // row 2
        out.m[i][2] = -out.m[i][2]; // column 2
    }
    return out;
}

} // namespace loupe
