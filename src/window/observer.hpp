// The observer: the person's own camera on a frozen frame, which the view's
// second draw is seen from (Frames::observe). It is Loupe's, never the scene's:
// the frame's camera stays where its path put it. autana's fly_motion flies it;
// this file only gathers its steering from ImGui.
#pragma once

#include "engine/frames.hpp"
#include "engine/pose.hpp"

namespace loupe::window {

// A flight starts at the speed that crosses the scene's box, corner to corner,
// in this many seconds, whatever units the scene is in.
constexpr float OBSERVER_CROSSING_S = 20.0F;

// FLY_MOTION_TUNING_DEFAULT, which is a C compound literal, spelled as C++ can.
constexpr fly_motion_tuning_t OBSERVER_TUNING = {
    .speed_step = FLY_MOTION_SPEED_STEP,
    .speed_min_ratio = FLY_MOTION_SPEED_MIN_RATIO,
    .speed_max_ratio = FLY_MOTION_SPEED_MAX_RATIO,
    .radians_per_pixel = FLY_MOTION_RADIANS_PER_PIXEL,
    .pitch_limit = FLY_MOTION_PITCH_LIMIT,
};

// `fly.pose` starts at Frames::observer_pose(), at observer_start_speed().
struct Observer {
    bool on = false;
    fly_motion_t fly = fly_motion_start(transformf_identity(), FLY_MOTION_START_SPEED, OBSERVER_TUNING);
};

// OBSERVER_CROSSING_S over `scene` corner to corner, or FLY_MOTION_START_SPEED
// over an empty box.
float observer_start_speed(box3f_t scene);

// What ImGui saw this tick as steering: WASD and Q/E unless a text field has the
// keyboard, a drag of the right button that began over the last item drawn (the
// frame), and the wheel over it.
fly_motion_steer_t observer_steer();

// fly_motion_update() on observer_steer(). True when the pose changed.
bool drive_observer(Observer& observer, float dt_seconds);

// The observer's checkbox, shown only when the view can observe; turning it on
// starts a flight at Frames::observer_pose() and the scene's speed.
void draw_observer_toggle(const Frames& frames, Observer& observer);

} // namespace loupe::window
