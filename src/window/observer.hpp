// The observer: the person's own camera on a frozen frame, which the view's
// second draw is seen from (Frames::observe). It is Loupe's, never the scene's:
// the frame's camera stays where its path put it.
#pragma once

#include <numbers>

#include "engine/frames.hpp"
#include "engine/pose.hpp"

namespace loupe::window {

// Starts at START_SPEED metres per second; a wheel notch scales that by
// SPEED_STEP, within SPEED_MIN and SPEED_MAX. A pixel of drag turns it
// RADIANS_PER_PIXEL, up or down at most PITCH_LIMIT: short of straight, where
// forward would meet the world's up. Its pose starts at Frames::observer_pose().
struct Observer {
    static constexpr float START_SPEED = 4.0F;
    static constexpr float SPEED_STEP = 1.25F;
    static constexpr float SPEED_MIN = 0.05F;
    static constexpr float SPEED_MAX = 500.0F;
    static constexpr float RADIANS_PER_PIXEL = 0.004F;
    static constexpr float PITCH_LIMIT = (std::numbers::pi_v<float> / 2) - 0.01F;

    bool on = false;
    transformf_t pose = transformf_identity();
    float speed = START_SPEED;
};

// What the person asks of the observer over one tick, with no SDL or window
// types, so any input can steer it. `move` is each -1, 0 or 1: x along the
// observer's right (D, A), y along the world's up (E, Q), z along its forward
// (W, S). `look_x` and `look_y` are the drag in pixels, right and down;
// `wheel` is notches, up positive.
struct Steer {
    vec3f_t move = {0.0F, 0.0F, 0.0F};
    float look_x = 0.0F, look_y = 0.0F;
    float wheel = 0.0F;
};

// Turns `observer` by the drag, keeping its horizon level, and moves it at its
// speed for `dt_seconds`, after the wheel scales the speed. True when the pose changed.
bool steer_observer(Observer& observer, const Steer& steer, float dt_seconds);

// steer_observer() on what ImGui saw this tick: WASD and Q/E unless a text field
// has the keyboard, a drag of the right button that began over the last item
// drawn (the frame), and the wheel over it. True when the pose changed.
bool drive_observer(Observer& observer, float dt_seconds);

// The observer's checkbox, shown only when the view can observe; turning it on
// starts the pose at Frames::observer_pose().
void draw_observer_toggle(const Frames& frames, Observer& observer);

} // namespace loupe::window
