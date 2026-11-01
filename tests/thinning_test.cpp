// Under a checkpoint budget the kept checkpoints stay within it, the interval
// doubles per thinning, and a seek still draws the bytes stepping straight drew.
#include <algorithm>
#include <climits>
#include <cstdio>
#include <iterator>
#include <limits>
#include <map>
#include <vector>

#include "frames_fixture.hpp"

namespace {

constexpr const char TEST[] = "thinning_test";
constexpr int EVERY = loupe::Frames::CHECKPOINT_EVERY;
constexpr int STRAIGHT_TO = 7 * EVERY - 10;
// Both sides of each checkpoint that can stay, and inside spans whose checkpoints were dropped.
const int TARGETS[] = {STRAIGHT_TO - 1, 4 * EVERY + 2, 4 * EVERY, 2 * EVERY + 5, 2 * EVERY, EVERY + 1, 5};
// How far a checkpoint's size may stray from the first's, in parts of it: past
// this, a budget counted in first checkpoints would thin at other frames.
constexpr size_t SIZE_SLACK_PARTS = 8;
// Past this, EVERY doubled that many times overflows an int.
constexpr long long MAX_THINNINGS = 24;

} // namespace

int
main(int argc, char** argv) {
    // Room for HALVES / 2 checkpoints, which by STRAIGHT_TO doubles the interval THINNINGS times.
    long long halves = 0;
    long long thinnings = 0;
    if (argc != 5 || !loupe::parse_count(argv[3], 1, INT_MAX, &halves)
        || !loupe::parse_count(argv[4], 0, MAX_THINNINGS, &thinnings)) {
        std::fprintf(stderr, "usage: thinning_test SCENE.scene.toml out.bmp HALVES THINNINGS\n");
        return 2;
    }
    std::unique_ptr<loupe::Frames> frames = loupe::test::open_scene(TEST, argv[1], argv[2]);
    if (frames == nullptr) {
        return 1;
    }

    // The first checkpoint's size sets the budget.
    frames->keep_checkpoints(std::numeric_limits<size_t>::max());
    bool ok = frames->step(frames->dt_ms()) && frames->step(frames->dt_ms());
    const size_t one = frames->checkpoint_bytes();
    const size_t budget = one * static_cast<size_t>(halves) / 2;
    frames->keep_checkpoints(budget);

    std::map<int, std::vector<uint8_t>> straight;
    while (frames->index() < STRAIGHT_TO) {
        const size_t before = frames->checkpoint_bytes();
        ok &= frames->step(frames->dt_ms());
        const int shown = frames->index() - 1;
        if (std::ranges::find(TARGETS, shown) != std::end(TARGETS)) {
            straight[shown] = loupe::test::pixels(*frames);
        }
        const size_t after = frames->checkpoint_bytes();
        if (after > budget) {
            std::fprintf(stderr, "%s: %zu bytes kept at frame %d, over %zu\n", TEST, after, shown, budget);
            ok = false;
        }
        // A step that saved without thinning grows the total by exactly that checkpoint.
        const size_t added = after > before ? after - before : 0;
        if (added != 0 && (added > one ? added - one : one - added) * SIZE_SLACK_PARTS > one) {
            std::fprintf(stderr, "%s: checkpoint before frame %d is %zu bytes, the first %zu\n", TEST, shown, added,
                         one);
            ok = false;
        }
    }
    const int expected_every = EVERY << static_cast<int>(thinnings);
    if (frames->checkpoint_every() != expected_every) {
        std::fprintf(stderr, "%s: every %d, not %d\n", TEST, frames->checkpoint_every(), expected_every);
        ok = false;
    }
    for (const int target : TARGETS) {
        ok &= loupe::test::same(TEST, *frames, target, straight[target], "thinned");
    }
    return frames->close() && ok ? 0 : 1;
}
