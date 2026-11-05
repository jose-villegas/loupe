// Frames::observe under the cull view, which draws a second view: an observer
// redraws the shown frame from elsewhere at one engine frame a move, and leaves
// nothing behind; turned off, and after any seek, the bytes are a straight run's.
#include <cstdio>

#include "frames_fixture.hpp"

namespace {

constexpr const char TEST[] = "observe_test";
constexpr const char VIEW[] = "cull";
constexpr int EVERY = loupe::Frames::CHECKPOINT_EVERY;
// Well past the second checkpoint (before frame 1 + EVERY), so redrawing the
// shown frame from that checkpoint would cost OBSERVED - EVERY frames, not one.
constexpr int OBSERVED = EVERY + 20;
constexpr int STRAIGHT_TO = OBSERVED + 5;
// After observing: the observed frame, the next, one in the first span, back to the observed.
const int AFTER[] = {OBSERVED, OBSERVED + 1, 5, OBSERVED};
constexpr int MOVES = 100;
// The observer's step back from where it starts, per move, in metres.
constexpr float BACK = 0.05F;

bool
check(bool ok, const char* what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, what, ok ? "ok" : "FAILED");
    return ok;
}

int
view_named(const char* name) {
    for (const loupe::DebugView& view : loupe::debug_views()) {
        if (view.name == name) {
            return view.id;
        }
    }
    return -1;
}

// `pose` moved `metres` behind where it faces.
transformf_t
backed(transformf_t pose, float metres) {
    transformf_translate(&pose, vec3f_scale(quatf_rotate(pose.rotation, vec3f_forward()), -metres));
    return pose;
}

} // namespace

int
main(int argc, char** argv) {
    std::unique_ptr<loupe::Frames> frames =
        loupe::test::open_test(TEST, std::span(argv, static_cast<size_t>(argc)), 3, "SCENE.apak out.bmp");
    if (frames == nullptr) {
        return 1;
    }
    bool ok = check(!frames->can_observe(), "shaded cannot observe");
    ok &= check(frames->set_view(view_named(VIEW)) && frames->can_observe(), "the cull view can observe");

    loupe::test::Straight straight;
    ok &= loupe::test::step_straight(*frames, STRAIGHT_TO, {}, &straight);
    ok &= frames->seek(OBSERVED);
    const std::optional<transformf_t> start = frames->observer_pose();
    ok &= check(start.has_value(), "the observer has a pose to start from");
    if (!start) {
        return 1;
    }

    const int64_t before = frames->drawn();
    std::vector<uint8_t> first;
    for (int move = 1; move <= MOVES; move++) {
        ok &= frames->observe(backed(*start, BACK * static_cast<float>(move)));
        if (move == 1) {
            first = loupe::test::pixels(*frames);
        }
    }
    const int64_t cost = frames->drawn() - before;
    std::fprintf(stderr, "%s: %d moves drew %lld frames\n", TEST, MOVES, static_cast<long long>(cost));
    // The first move goes back to the checkpoint before OBSERVED once; each move after it is one frame.
    ok &= check(cost <= MOVES + EVERY, "each move costs one frame");
    ok &= check(frames->index() == OBSERVED + 1, "observing stays on the shown frame");
    ok &= check(first != straight[OBSERVED], "the observer draws from elsewhere");
    ok &= frames->observe(backed(*start, BACK));
    ok &= check(loupe::test::pixels(*frames) == first, "the same pose draws the same bytes");

    ok &= check(frames->observe(std::nullopt) && loupe::test::pixels(*frames) == straight[OBSERVED],
                "turned off, the shown frame is the straight run's");
    ok &= frames->observe(backed(*start, BACK));
    ok &= loupe::test::same_each(TEST, *frames, AFTER, straight, "after observing");
    return frames->close() && ok ? 0 : 1;
}
