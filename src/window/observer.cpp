#include "window/observer.hpp"

#include <algorithm>
#include <cmath>

#include "imgui.h"

namespace loupe::window {

namespace {

// Below this, a forward has no horizontal part to take a heading from.
constexpr float LEVEL_EPSILON = 1e-4F;

vec3f_t
level(vec3f_t v) {
    return {.x = v.x, .y = 0.0F, .z = v.z};
}

float
length(vec3f_t v) {
    return std::sqrt(vec3f_dot(v, v));
}

// Where the observer faces across the ground. Looking straight up or down,
// forward has none, so the picture's top (or bottom, looking up) says.
vec3f_t
heading(const transformf_t& pose) {
    const vec3f_t forward = quatf_rotate(pose.rotation, vec3f_forward());
    if (length(level(forward)) >= LEVEL_EPSILON) {
        return level(forward);
    }
    const vec3f_t up = quatf_rotate(pose.rotation, vec3f_up());
    return vec3f_scale(level(up), forward.y < 0.0F ? 1.0F : -1.0F);
}

// The pose turned by the drag: yaw about the world's up, pitch within PITCH_LIMIT, no roll.
transformf_t
turned(const transformf_t& pose, float look_x, float look_y) {
    const vec3f_t ahead = heading(pose);
    const vec3f_t forward = quatf_rotate(pose.rotation, vec3f_forward());
    const float yaw = std::atan2(ahead.x, ahead.z) + (look_x * Observer::RADIANS_PER_PIXEL);
    const float pitch =
        std::clamp(std::asin(std::clamp(forward.y, -1.0F, 1.0F)) - (look_y * Observer::RADIANS_PER_PIXEL),
                   -Observer::PITCH_LIMIT, Observer::PITCH_LIMIT);
    const vec3f_t facing = {
        .x = std::cos(pitch) * std::sin(yaw), .y = std::sin(pitch), .z = std::cos(pitch) * std::cos(yaw)};
    return transformf_looking(pose.position, facing, vec3f_up());
}

// The step `move` asks for at `distance`: along the observer's right and forward and the world's up, no
// faster along a diagonal.
vec3f_t
stride(const transformf_t& pose, vec3f_t move, float distance) {
    const vec3f_t along = vec3f_add(
        vec3f_add(vec3f_scale(quatf_rotate(pose.rotation, vec3f_right()), move.x), vec3f_scale(vec3f_up(), move.y)),
        vec3f_scale(quatf_rotate(pose.rotation, vec3f_forward()), move.z));
    const float reach = length(along);
    return vec3f_scale(along, distance / std::max(reach, 1.0F));
}

} // namespace

bool
steer_observer(Observer& observer, const Steer& steer, float dt_seconds) {
    observer.speed = std::clamp(observer.speed * std::pow(Observer::SPEED_STEP, steer.wheel), Observer::SPEED_MIN,
                                Observer::SPEED_MAX);
    bool changed = false;
    if (steer.look_x != 0.0F || steer.look_y != 0.0F) {
        observer.pose = turned(observer.pose, steer.look_x, steer.look_y);
        changed = true;
    }
    const vec3f_t step = stride(observer.pose, steer.move, observer.speed * dt_seconds);
    if (vec3f_dot(step, step) > 0.0F) {
        transformf_translate(&observer.pose, step);
        changed = true;
    }
    return changed;
}

namespace {

float
axis(ImGuiKey positive, ImGuiKey negative) {
    return (ImGui::IsKeyDown(positive) ? 1.0F : 0.0F) - (ImGui::IsKeyDown(negative) ? 1.0F : 0.0F);
}

} // namespace

bool
drive_observer(Observer& observer, float dt_seconds) {
    const ImGuiIO& io = ImGui::GetIO();
    Steer steer;
    if (!io.WantTextInput) {
        steer.move = {
            .x = axis(ImGuiKey_D, ImGuiKey_A), .y = axis(ImGuiKey_E, ImGuiKey_Q), .z = axis(ImGuiKey_W, ImGuiKey_S)};
    }
    const ImVec2 began = io.MouseClickedPos[ImGuiMouseButton_Right];
    const ImVec2 min = ImGui::GetItemRectMin();
    const ImVec2 max = ImGui::GetItemRectMax();
    const bool began_over = began.x >= min.x && began.y >= min.y && began.x < max.x && began.y < max.y;
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && began_over) {
        steer.look_x = io.MouseDelta.x;
        steer.look_y = io.MouseDelta.y;
    }
    if (ImGui::IsItemHovered()) {
        steer.wheel = io.MouseWheel;
    }
    return steer_observer(observer, steer, dt_seconds);
}

void
draw_observer_toggle(const Frames& frames, Observer& observer) {
    if (!frames.can_observe()) {
        return;
    }
    const bool was = observer.on;
    ImGui::Checkbox("observer", &observer.on);
    if (observer.on && !was) {
        observer.pose = frames.observer_pose().value_or(observer.pose);
    }
}

} // namespace loupe::window
