// Seeking to frame N through a checkpoint draws the bytes stepping straight to N drew.
#include <cstdio>
#include <map>
#include <string>
#include <vector>

#include "engine/frames.hpp"

namespace {

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

std::vector<uint8_t>
pixels(const loupe::Frames& frames) {
    const loupe::Frame frame = frames.frame();
    return std::vector<uint8_t>(frame.begin(), frame.end());
}

bool
same(loupe::Frames& frames, int index, const std::vector<uint8_t>& expected, const char* what) {
    const bool sought = frames.seek(index);
    const bool ok = sought && pixels(frames) == expected;
    std::fprintf(stderr, "seek_test: %s frame %d %s\n", what, index, ok ? "same" : sought ? "DIFFERS" : "NOT SOUGHT");
    return ok;
}

} // namespace

int
main(int argc, char** argv) {
    if (argc != 3) {
        std::fprintf(stderr, "usage: seek_test SCENE.scene.toml out.bmp\n");
        return 2;
    }
    loupe::Options options;
    options.scene_toml = argv[1];
    options.out_bmp = argv[2];
    std::string why;
    std::unique_ptr<loupe::Frames> frames = loupe::Frames::open(options, &why);
    if (frames == nullptr) {
        std::fprintf(stderr, "seek_test: %s\n", why.c_str());
        return 1;
    }
    frames->keep_checkpoints();

    bool ok = true;
    std::map<int, std::vector<uint8_t>> straight;
    while (frames->index() < STRAIGHT_TO) {
        ok &= frames->step(frames->dt_ms());
        straight[frames->index() - 1] = pixels(*frames);
    }
    for (const int target : TARGETS) {
        ok &= same(*frames, target, straight[target], "straight");
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
    ok &= same(*frames, RETIMED_FROM + 1, retimed[RETIMED_FROM + 1], "retimed");
    ok &= same(*frames, RETIMED_TO, retimed[RETIMED_TO], "retimed");
    return frames->close() && ok ? 0 : 1;
}
