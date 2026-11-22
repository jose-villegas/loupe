#include "engine/path_curve.hpp"

#include <array>
#include <cstdint>

#include "engine/autana.hpp"

namespace loupe {

namespace {

constexpr uint8_t VEC3 = 3, QUAT = 4;

} // namespace

transformf_t
pose_on(const PathCurve& curve, float seconds) {
    const auto count = static_cast<uint16_t>(curve.times.size());
    const anim_track_t translation = {.times = curve.times.data(),
                                      .values = curve.translations.data(),
                                      .count = count,
                                      .width = VEC3,
                                      .interp = ANIM_CUBIC,
                                      .quaternion = 0};
    const anim_track_t rotation = {.times = curve.times.data(),
                                   .values = curve.rotations.data(),
                                   .count = count,
                                   .width = QUAT,
                                   .interp = ANIM_LINEAR,
                                   .quaternion = 1};
    std::array<float, ANIM_WIDTH_MAX> eye{}, turn{};
    anim_track_sample(&translation, seconds, eye.data());
    anim_track_sample(&rotation, seconds, turn.data());
    transformf_t pose = transformf_identity();
    transformf_set_position(&pose, vec3f_t{eye[0], eye[1], eye[2]});
    transformf_set_rotation(&pose, quatf_t{turn[0], turn[1], turn[2], turn[3]});
    return pose;
}

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

} // namespace loupe
