// Seeking to frame N through a checkpoint draws the bytes stepping straight to N drew.
#include <cstdio>
#include <limits>
#include <map>
#include <vector>

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

} // namespace

int
main(int argc, char** argv) {
    using loupe::test::pixels;
    using loupe::test::same;
    if (argc != 3) {
        std::fprintf(stderr, "usage: seek_test SCENE.scene.toml out.bmp\n");
        return 2;
    }
    std::unique_ptr<loupe::Frames> frames = loupe::test::open_scene(TEST, argv[1], argv[2]);
    if (frames == nullptr) {
        return 1;
    }
    frames->keep_checkpoints(std::numeric_limits<size_t>::max());

    bool ok = true;
    std::map<int, std::vector<uint8_t>> straight;
    while (frames->index() < STRAIGHT_TO) {
        ok &= frames->step(frames->dt_ms());
        straight[frames->index() - 1] = pixels(*frames);
    }
    for (const int target : TARGETS) {
        ok &= same(TEST, *frames, target, straight[target], "straight");
    }

    const uint32_t retimed_dt = 2 * frames->dt_ms();
    std::map<int, std::vector<uint8_t>> retimed;
    ok &= frames->seek(RETIMED_FROM - 1);
    while (frames->index() <= RETIMED_TO) {
        ok &= frames->step(retimed_dt);
        retimed[frames->index() - 1] = pixels(*frames);
    }
    if (retimed[RETIMED_TO] == straight[RETIMED_TO]) {
        std::fprintf(stderr, "seek_test: the other dt drew the same frame, so it proves nothing\n");
        ok = false;
    }
    ok &= same(TEST, *frames, RETIMED_FROM + 1, retimed[RETIMED_FROM + 1], "retimed");
    ok &= same(TEST, *frames, RETIMED_TO, retimed[RETIMED_TO], "retimed");
    return frames->close() && ok ? 0 : 1;
}
