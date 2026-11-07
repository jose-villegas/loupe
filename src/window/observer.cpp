#include "window/observer.hpp"

#include "imgui.h"

namespace loupe::window {

namespace {

float
axis(ImGuiKey positive, ImGuiKey negative) {
    return (ImGui::IsKeyDown(positive) ? 1.0F : 0.0F) - (ImGui::IsKeyDown(negative) ? 1.0F : 0.0F);
}

} // namespace

float
observer_start_speed(box3f_t scene) {
    if (box3f_is_empty(scene)) {
        return FLY_MOTION_START_SPEED;
    }
    return vec3f_length(vec3f_sub(scene.hi, scene.lo)) / OBSERVER_CROSSING_S;
}

fly_motion_steer_t
observer_steer() {
    const ImGuiIO& io = ImGui::GetIO();
    fly_motion_steer_t steer{};
    if (!io.WantTextInput) {
        steer.move = {
            .x = axis(ImGuiKey_D, ImGuiKey_A), .y = axis(ImGuiKey_E, ImGuiKey_Q), .z = axis(ImGuiKey_W, ImGuiKey_S)};
    }
    const ImVec2 began = io.MouseClickedPos[ImGuiMouseButton_Right];
    const ImVec2 min = ImGui::GetItemRectMin();
    const ImVec2 max = ImGui::GetItemRectMax();
    const bool began_over = began.x >= min.x && began.y >= min.y && began.x < max.x && began.y < max.y;
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && began_over) {
        steer.look = {.x = io.MouseDelta.x, .y = io.MouseDelta.y};
    }
    if (ImGui::IsItemHovered()) {
        steer.wheel = io.MouseWheel;
    }
    return steer;
}

bool
drive_observer(Observer& observer, float dt_seconds) {
    const fly_motion_steer_t steer = observer_steer();
    return fly_motion_update(&observer.fly, &steer, dt_seconds);
}

void
draw_observer_toggle(const Frames& frames, Observer& observer) {
    if (!frames.can_observe()) {
        return;
    }
    const bool was = observer.on;
    ImGui::Checkbox("observer", &observer.on);
    if (observer.on && !was) {
        observer.fly = fly_motion_start(frames.observer_pose().value_or(observer.fly.pose),
                                        observer_start_speed(frames.scene_box()), OBSERVER_TUNING);
    }
}

} // namespace loupe::window
