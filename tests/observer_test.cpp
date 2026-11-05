// steer_observer(): the keys move the observer along its own right and forward
// and the world's up at its speed, a drag turns it with a level horizon, and the
// wheel scales its speed within bounds.
#include <cmath>
#include <cstdio>
#include <numbers>

#include "window/observer.hpp"

namespace {

using loupe::window::Observer;
using loupe::window::Steer;
using loupe::window::steer_observer;

constexpr float TOLERANCE = 1e-4F;
constexpr float HALF_PI = std::numbers::pi_v<float> / 2;
// A drag that turns a quarter turn.
constexpr float QUARTER_DRAG = HALF_PI / Observer::RADIANS_PER_PIXEL;

int failures = 0;

void
expect(bool ok, const char* what) {
    std::fprintf(stderr, "observer_test: %s %s\n", what, ok ? "ok" : "FAILED");
    failures += ok ? 0 : 1;
}

bool
near(vec3f_t a, vec3f_t b) {
    return std::fabs(a.x - b.x) < TOLERANCE && std::fabs(a.y - b.y) < TOLERANCE && std::fabs(a.z - b.z) < TOLERANCE;
}

vec3f_t
forward(const Observer& observer) {
    return quatf_rotate(observer.pose.rotation, vec3f_forward());
}

// At the origin, facing `facing` with the world's up above.
Observer
facing(vec3f_t direction) {
    Observer observer;
    observer.pose = transformf_looking(vec3f_zero(), direction, vec3f_up());
    return observer;
}

void
still() {
    Observer observer = facing(vec3f_forward());
    const transformf_t before = observer.pose;
    expect(!steer_observer(observer, Steer{}, 1.0F), "no input leaves the pose");
    expect(near(observer.pose.position, before.position) && near(forward(observer), vec3f_forward()),
           "no input keeps where it stands and faces");
    Steer keys;
    keys.move = vec3f_forward();
    expect(!steer_observer(observer, keys, 0.0F), "keys over no time leave the pose");
}

void
moves() {
    const float dt = 0.5F;
    // Facing +x: forward is +x and right is -z.
    Observer observer = facing(vec3f_right());
    const float distance = observer.speed * dt;
    Steer keys;
    keys.move = vec3f_forward();
    expect(steer_observer(observer, keys, dt), "W changes the pose");
    expect(near(observer.pose.position, vec3f_scale(vec3f_right(), distance)), "W moves speed * dt along forward");

    observer = facing(vec3f_right());
    keys.move = vec3f_right();
    (void)steer_observer(observer, keys, dt);
    expect(near(observer.pose.position, vec3f_scale(vec3f_unit_z(), -distance)), "D moves along the observer's right");

    // Looking down at a slant, E still climbs straight up.
    observer = facing(vec3f_normalize({0.0F, -1.0F, 1.0F}));
    keys.move = vec3f_up();
    (void)steer_observer(observer, keys, dt);
    expect(near(observer.pose.position, vec3f_scale(vec3f_up(), distance)), "E moves along the world's up");

    observer = facing(vec3f_forward());
    keys.move = {1.0F, 0.0F, 1.0F};
    (void)steer_observer(observer, keys, dt);
    const vec3f_t at = observer.pose.position;
    expect(std::fabs(std::sqrt(vec3f_dot(at, at)) - distance) < TOLERANCE, "a diagonal is no faster");
}

void
looks() {
    Observer observer = facing(vec3f_forward());
    Steer drag;
    drag.look_x = QUARTER_DRAG;
    expect(steer_observer(observer, drag, 0.0F), "a drag changes the pose");
    expect(near(forward(observer), vec3f_right()), "a quarter drag right turns to face right");
    expect(near(quatf_rotate(observer.pose.rotation, vec3f_up()), vec3f_up()), "the horizon stays level");

    observer = facing(vec3f_forward());
    drag = Steer{};
    drag.look_y = 4 * QUARTER_DRAG;
    (void)steer_observer(observer, drag, 0.0F);
    expect(std::fabs(forward(observer).y + std::sin(Observer::PITCH_LIMIT)) < TOLERANCE,
           "dragging far down stops at the pitch limit");

    // The top spectator's pose: straight down, the picture's top towards +z.
    // Looking up from it tips forward towards the picture's top.
    observer.pose = transformf_looking(vec3f_zero(), vec3f_down(), vec3f_unit_z());
    drag = Steer{};
    drag.look_y = -QUARTER_DRAG / 2;
    (void)steer_observer(observer, drag, 0.0F);
    const vec3f_t tipped = forward(observer);
    expect(tipped.z > 0.0F && std::fabs(tipped.x) < TOLERANCE, "looking up from straight down tips to the top");
}

void
wheel() {
    Observer observer = facing(vec3f_forward());
    Steer notch;
    notch.wheel = 1.0F;
    expect(!steer_observer(observer, notch, 1.0F), "the wheel leaves the pose");
    expect(std::fabs(observer.speed - (Observer::START_SPEED * Observer::SPEED_STEP)) < TOLERANCE,
           "a notch up scales the speed by SPEED_STEP");
    notch.wheel = -1000.0F;
    (void)steer_observer(observer, notch, 0.0F);
    expect(observer.speed == Observer::SPEED_MIN, "the speed stops at SPEED_MIN");
    notch.wheel = 1000.0F;
    (void)steer_observer(observer, notch, 0.0F);
    expect(observer.speed == Observer::SPEED_MAX, "the speed stops at SPEED_MAX");
}

} // namespace

int
main() {
    still();
    moves();
    looks();
    wheel();
    return failures == 0 ? 0 : 1;
}
