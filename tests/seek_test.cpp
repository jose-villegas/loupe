// Seeking to frame N through a checkpoint draws the bytes stepping straight to N drew.
#include <cstdio>
#include <limits>

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
    if (argc != 3) {
        std::fprintf(stderr, "usage: seek_test SCENE.scene.toml out.bmp\n");
        return 2;
    }
    std::unique_ptr<loupe::Frames> frames = loupe::test::open_or_report(TEST, loupe::test::scene_args(argv));
    if (frames == nullptr) {
        return 1;
    }
    frames->keep_checkpoints(std::numeric_limits<size_t>::max());

    loupe::test::Straight straight;
    bool ok = loupe::test::step_straight(*frames, STRAIGHT_TO, {}, &straight);
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
    return frames->close() && ok ? 0 : 1;
}
