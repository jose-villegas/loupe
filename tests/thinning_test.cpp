// Under a checkpoint budget the kept checkpoints stay within it, the interval
// doubles per thinning, and a seek still draws the bytes stepping straight drew.
#include <climits>
#include <cstdio>

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
constexpr const char USAGE[] = "SCENE.apak out.bmp HALVES THINNINGS";

// Judges each step: the kept checkpoints within the budget, and each one saved
// about the size of the first, the one kept when the watch starts.
class BudgetWatch {
  public:
    BudgetWatch(const loupe::Frames& frames, size_t budget)
        : frames_(frames), one_(frames.checkpoint_bytes()), budget_(budget), last_(one_) {}

    bool
    operator()(int shown) {
        const size_t now = frames_.checkpoint_bytes();
        // A step that saved without thinning grows the total by exactly that checkpoint.
        const size_t added = now > last_ ? now - last_ : 0;
        last_ = now;
        if (now > budget_) {
            std::fprintf(stderr, "%s: %zu bytes kept at frame %d, over %zu\n", TEST, now, shown, budget_);
            return false;
        }
        if (added != 0 && (added > one_ ? added - one_ : one_ - added) * SIZE_SLACK_PARTS > one_) {
            std::fprintf(stderr, "%s: checkpoint before frame %d is %zu bytes, the first %zu\n", TEST, shown, added,
                         one_);
            return false;
        }
        return true;
    }

  private:
    const loupe::Frames& frames_;
    size_t one_, budget_, last_;
};

} // namespace

int
main(int argc, char** argv) {
    // Room for HALVES / 2 checkpoints, which by STRAIGHT_TO doubles the interval THINNINGS times.
    long long halves = 0;
    long long thinnings = 0;
    if (argc != 5 || !loupe::parse_count(argv[3], 1, INT_MAX, &halves)
        || !loupe::parse_count(argv[4], 0, MAX_THINNINGS, &thinnings)) {
        std::fprintf(stderr, "usage: %s %s\n", TEST, USAGE);
        return 2;
    }
    // The first checkpoint's size sets the budget.
    std::unique_ptr<loupe::Frames> frames =
        loupe::test::open_test(TEST, std::span(argv, static_cast<size_t>(argc)), 5, USAGE);
    if (frames == nullptr) {
        return 1;
    }
    bool ok = frames->step(frames->dt_ms()) && frames->step(frames->dt_ms());
    const size_t one = frames->checkpoint_bytes();
    const size_t budget = one * static_cast<size_t>(halves) / 2;
    frames->keep_checkpoints(budget);

    loupe::test::Straight straight;
    ok &= loupe::test::step_straight(*frames, STRAIGHT_TO, TARGETS, &straight, BudgetWatch(*frames, budget));
    const int expected_every = EVERY << static_cast<int>(thinnings);
    if (frames->checkpoint_every() != expected_every) {
        std::fprintf(stderr, "%s: every %d, not %d\n", TEST, frames->checkpoint_every(), expected_every);
        ok = false;
    }
    ok &= loupe::test::same_each(TEST, *frames, TARGETS, straight, "thinned");
    return frames->close() && ok ? 0 : 1;
}
