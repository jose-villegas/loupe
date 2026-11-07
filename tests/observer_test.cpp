// The observer's own part of flying, fly_motion being autana's: the start speed
// read from the scene's box, and observer_steer() reading ImGui as steering. Keys
// map to the axes unless a text field has them, a right drag counts only when it
// began over the frame, and the wheel only over it.
#include <cmath>
#include <cstdio>
#include <functional>
#include <initializer_list>

#include "imgui.h"
#include "window/observer.hpp"

namespace {

using loupe::window::OBSERVER_CROSSING_S;
using loupe::window::observer_start_speed;
using loupe::window::observer_steer;

constexpr float TOLERANCE = 1e-5F;
// The frame's item, from the window's top left, and points over it and past it.
constexpr float FRAME_SIDE = 100.0F;
constexpr float WINDOW_SIDE = 2.0F * FRAME_SIDE;
const ImVec2 OVER = {FRAME_SIDE / 2, FRAME_SIDE / 2};
const ImVec2 PAST = {FRAME_SIDE * 1.5F, FRAME_SIDE * 1.5F};
const ImVec2 DRAG = {10.0F, -5.0F};
// Points a drag crosses the frame's right edge between, just inside and just past it.
const ImVec2 INSIDE_EDGE = {FRAME_SIDE - 5.0F, FRAME_SIDE / 2};
const ImVec2 PAST_EDGE = {FRAME_SIDE + 10.0F, FRAME_SIDE / 2};
constexpr float NOTCHES = 2.0F;

int failures = 0;

void
expect(bool ok, const char* what) {
    std::fprintf(stderr, "observer_test: %s %s\n", what, ok ? "ok" : "FAILED");
    failures += ok ? 0 : 1;
}

bool
is_zero(vec3f_t v) {
    return v.x == 0.0F && v.y == 0.0F && v.z == 0.0F;
}

// One ImGui frame with the frame drawn as an item at the window's top left;
// `before` runs after NewFrame, as the window's own reads of io would see it.
// Returns what observer_steer() read just after the item.
fly_motion_steer_t
tick(const std::function<void()>& before = {}) {
    ImGui::NewFrame();
    if (before) {
        before();
    }
    ImGui::SetNextWindowPos({0.0F, 0.0F});
    ImGui::SetNextWindowSize({WINDOW_SIDE, WINDOW_SIDE});
    ImGui::Begin("frame", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
    ImGui::SetCursorScreenPos({0.0F, 0.0F});
    ImGui::InvisibleButton("frame", {FRAME_SIDE, FRAME_SIDE});
    const fly_motion_steer_t steer = observer_steer();
    ImGui::End();
    ImGui::Render();
    return steer;
}

void
keys_down(std::initializer_list<ImGuiKey> which, bool down) {
    for (const ImGuiKey key : which) {
        ImGui::GetIO().AddKeyEvent(key, down);
    }
}

void
start_speed() {
    const box3f_t metres = {.lo = {.x = 0.0F, .y = 0.0F, .z = 0.0F},
                            .hi = {.x = 3.0F, .y = 4.0F, .z = 12.0F}}; // 13 corner to corner
    expect(std::fabs(observer_start_speed(metres) - (13.0F / OBSERVER_CROSSING_S)) < TOLERANCE,
           "the box is crossed corner to corner in the crossing time");
    expect(observer_start_speed(box3f_empty()) == FLY_MOTION_START_SPEED, "an empty box starts at the start speed");
}

void
keys() {
    keys_down({ImGuiKey_W, ImGuiKey_D, ImGuiKey_Q}, true);
    const fly_motion_steer_t ahead = tick();
    expect(ahead.move.x == 1.0F && ahead.move.y == -1.0F && ahead.move.z == 1.0F, "D, Q and W: right, down, forward");
    keys_down({ImGuiKey_W, ImGuiKey_D, ImGuiKey_Q}, false);
    keys_down({ImGuiKey_S, ImGuiKey_A, ImGuiKey_E}, true);
    const fly_motion_steer_t back = tick();
    expect(back.move.x == -1.0F && back.move.y == 1.0F && back.move.z == -1.0F, "A, E and S: left, up, back");
    const fly_motion_steer_t typing = tick([] { ImGui::GetIO().WantTextInput = true; });
    expect(is_zero(typing.move), "a text field keeps the keys");
    keys_down({ImGuiKey_S, ImGuiKey_A, ImGuiKey_E}, false);
    expect(is_zero(tick().move), "no key, no move");
}

// A right drag from `from` to `to`: what the frame that sees the move reads.
vec2f_t
drag(ImVec2 from, ImVec2 to) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(from.x, from.y);
    (void)tick();
    io.AddMouseButtonEvent(ImGuiMouseButton_Right, true);
    (void)tick();
    io.AddMousePosEvent(to.x, to.y);
    const fly_motion_steer_t steer = tick();
    io.AddMouseButtonEvent(ImGuiMouseButton_Right, false);
    (void)tick();
    return steer.look;
}

void
drags() {
    const vec2f_t over = drag(OVER, {OVER.x + DRAG.x, OVER.y + DRAG.y});
    expect(over.x == DRAG.x && over.y == DRAG.y, "a right drag begun over the frame looks by its pixels");
    const vec2f_t out = drag(INSIDE_EDGE, PAST_EDGE);
    expect(out.x == PAST_EDGE.x - INSIDE_EDGE.x && out.y == 0.0F,
           "a right drag begun over the frame looks past its edge");
    const vec2f_t in = drag(PAST_EDGE, INSIDE_EDGE);
    expect(in.x == 0.0F && in.y == 0.0F, "a right drag begun past the frame does not look over it");
    const vec2f_t past = drag(PAST, {PAST.x + DRAG.x, PAST.y + DRAG.y});
    expect(past.x == 0.0F && past.y == 0.0F, "a right drag begun past the frame does not look");
}

// The wheel turned with the mouse at `at`: what that frame reads.
float
wheel_at(ImVec2 at) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(at.x, at.y);
    (void)tick();
    io.AddMouseWheelEvent(0.0F, NOTCHES);
    return tick().wheel;
}

void
wheel() {
    expect(wheel_at(OVER) == NOTCHES, "the wheel over the frame scales by its notches");
    expect(wheel_at(PAST) == 0.0F, "the wheel past the frame does not");
}

} // namespace

int
main() {
    start_speed();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = {WINDOW_SIDE, WINDOW_SIDE};
    // Nothing is drawn, so the atlas needs no renderer to build it up front.
    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
    (void)tick();
    keys();
    drags();
    wheel();
    ImGui::DestroyContext();
    return failures == 0 ? 0 : 1;
}
