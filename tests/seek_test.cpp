// Seeking to frame N through a checkpoint draws the bytes stepping straight to N drew.
#include <cstdio>

#include "frames_fixture.hpp"

namespace {

constexpr const char TEST[] = "seek_test";
constexpr int EVERY = loupe::Frames::CHECKPOINT_EVERY;
// Past three checkpoints (before frames 1, 1 + EVERY, 1 + 2 * EVERY), so a seek back can start from each.
constexpr int STRAIGHT_TO = 2 * EVERY + 10;
// Within the second span, onto a checkpoint, the frame before one, inside the
// first span, the first checkpoint, forward into the third span, then back to
// the third checkpoint after the others' replays.
const int TARGETS[] = {EVERY + 14, EVERY + 1, EVERY, 5, 1, 2 * EVERY + 3, 2 * EVERY + 2};
// After a seek, these frames step at another dt; seeking back into them replays that dt.
constexpr int RETIMED_FROM = EVERY + 5;
constexpr int RETIMED_TO = EVERY + 12;
const int RETIMED[] = {RETIMED_FROM + 1, RETIMED_TO};

} // namespace

int
main(int argc, char** argv) {
    loupe::test::Straight straight;
    bool ok = false;
    const std::unique_ptr<loupe::Frames> frames =
        loupe::test::open_stepped(TEST, std::span(argv, static_cast<size_t>(argc)), STRAIGHT_TO, &straight, &ok);
    if (frames == nullptr) {
        return 1;
    }
    ok &= loupe::test::same_each(TEST, *frames, TARGETS, straight, "straight");

    const uint32_t retimed_dt = 2 * frames->dt_ms();
    loupe::test::Straight retimed;
    ok &= frames->seek(RETIMED_FROM - 1);
    while (frames->index() <= RETIMED_TO) {
        ok &= frames->step(retimed_dt);
        retimed[frames->index() - 1] = loupe::test::pixels(*frames);
    }
    if (retimed[RETIMED_TO] == straight[RETIMED_TO]) {
        std::fprintf(stderr, "seek_test: the other dt drew the same frame, so it proves nothing\n");
        ok = false;
    }
    ok &= loupe::test::same_each(TEST, *frames, RETIMED, retimed, "retimed");

    // Each frame's time of play: the dts through it, the retimed ones replacing those first drawn.
    const uint32_t dt = frames->dt_ms();
    const uint32_t retimed_ms = (RETIMED_FROM * dt) + ((RETIMED_TO - RETIMED_FROM + 1) * retimed_dt);
    const bool timed =
        frames->time_ms(-1) == 0 && frames->time_ms(0) == dt && frames->time_ms(RETIMED_FROM - 1) == RETIMED_FROM * dt
        && frames->time_ms(RETIMED_FROM) == (RETIMED_FROM * dt) + retimed_dt
        && frames->time_ms(RETIMED_TO) == retimed_ms && frames->time_ms(RETIMED_TO + 3) == retimed_ms + (3 * dt);
    if (!timed) {
        std::fprintf(stderr, "seek_test: a frame's time of play is not the dts drawn through it\n");
        ok = false;
    }
    return frames->close() && ok ? 0 : 1;
}
