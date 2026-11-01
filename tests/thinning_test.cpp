// Under a checkpoint budget the kept checkpoints stay within it, the interval
// doubles per thinning, and a seek still draws the bytes stepping straight drew.
#include <algorithm>
#include <cstdio>
#include <iterator>
#include <limits>
#include <map>
#include <string>
#include <vector>

#include "engine/frames.hpp"

namespace {

constexpr int EVERY = loupe::Frames::CHECKPOINT_EVERY;
// Room for three checkpoints and a half: the fourth thins them to every other.
constexpr size_t BUDGET_HALVES = 7;
// Two thinnings: four checkpoints at EVERY apart, then four at twice EVERY.
constexpr int THINNINGS = 2;
constexpr int STRAIGHT_TO = 7 * EVERY - 10;
// Both sides of each kept checkpoint and inside spans whose checkpoints were dropped.
const int TARGETS[] = {STRAIGHT_TO - 1, 4 * EVERY + 2, 4 * EVERY, 2 * EVERY + 5, EVERY + 1, 5};

std::vector<uint8_t>
pixels(const loupe::Frames& frames) {
    const loupe::Frame frame = frames.frame();
    return std::vector<uint8_t>(frame.bgr.begin(), frame.bgr.end());
}

} // namespace

int
main(int argc, char** argv) {
    if (argc != 3) {
        std::fprintf(stderr, "usage: thinning_test SCENE.scene.toml out.bmp\n");
        return 2;
    }
    loupe::Options options;
    options.scene_toml = argv[1];
    options.out_bmp = argv[2];
    std::string why;
    std::unique_ptr<loupe::Frames> frames = loupe::Frames::open(options, &why);
    if (frames == nullptr) {
        std::fprintf(stderr, "thinning_test: %s\n", why.c_str());
        return 1;
    }

    // One checkpoint's size, from the first, sets the budget.
    frames->keep_checkpoints(std::numeric_limits<size_t>::max());
    bool ok = frames->step(frames->dt_ms()) && frames->step(frames->dt_ms());
    const size_t one = frames->checkpoint_bytes();
    const size_t budget = one * BUDGET_HALVES / 2;
    frames->keep_checkpoints(budget);

    std::map<int, std::vector<uint8_t>> straight;
    while (frames->index() < STRAIGHT_TO) {
        ok &= frames->step(frames->dt_ms());
        if (std::ranges::find(TARGETS, frames->index() - 1) != std::end(TARGETS)) {
            straight[frames->index() - 1] = pixels(*frames);
        }
        if (frames->checkpoint_bytes() > budget) {
            std::fprintf(stderr, "thinning_test: %zu bytes kept at frame %d, over %zu\n", frames->checkpoint_bytes(),
                         frames->index() - 1, budget);
            ok = false;
        }
    }
    const int expected_every = EVERY << THINNINGS;
    if (frames->checkpoint_every() != expected_every) {
        std::fprintf(stderr, "thinning_test: every %d, not %d\n", frames->checkpoint_every(), expected_every);
        ok = false;
    }
    for (const int target : TARGETS) {
        const bool sought = frames->seek(target);
        const bool same = sought && pixels(*frames) == straight[target];
        std::fprintf(stderr, "thinning_test: frame %d %s\n", target, same ? "same" : sought ? "DIFFERS" : "NOT SOUGHT");
        ok &= same;
    }
    return frames->close() && ok ? 0 : 1;
}
