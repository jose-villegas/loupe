// Seeking to frame N through a checkpoint draws the bytes stepping straight to N drew.
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "engine/frames.h"

namespace {

constexpr int EVERY = loupe::Frames::CHECKPOINT_EVERY;
// Past three checkpoints (before frames 1, 1 + EVERY, 1 + 2 * EVERY), so a seek back can start from each.
constexpr int STRAIGHT_TO = 2 * EVERY + 10;
// Within the second span, onto a checkpoint, the frame before one, inside the
// first span, the first checkpoint, then forward again into the third span.
const int TARGETS[] = {EVERY + 14, EVERY + 1, EVERY, 5, 1, 2 * EVERY + 3};

std::vector<uint8_t> pixels(const loupe::Frames& frames) {
    const loupe::Frame frame = frames.frame();
    return std::vector<uint8_t>(frame.pixels, frame.pixels + frame.bytes);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc != 3) {
        std::fprintf(stderr, "usage: seek_test SCENE.scene.toml out.bmp\n");
        return 2;
    }
    loupe::Options options;
    options.scene_toml = argv[1];
    options.out_bmp = argv[2];
    options.frames = STRAIGHT_TO;
    std::string why;
    std::unique_ptr<loupe::Frames> frames = loupe::Frames::open(options, &why);
    if (frames == nullptr) {
        std::fprintf(stderr, "seek_test: %s\n", why.c_str());
        return 1;
    }

    std::map<int, std::vector<uint8_t>> straight;
    while (frames->index() < STRAIGHT_TO) {
        frames->step(options.dt_ms);
        straight[frames->index() - 1] = pixels(*frames);
    }
    bool ok = true;
    for (const int target : TARGETS) {
        const bool sought = frames->seek(target);
        const bool same = sought && pixels(*frames) == straight[target];
        std::fprintf(stderr, "seek_test: frame %d %s\n", target, same ? "same" : sought ? "DIFFERS" : "NOT SOUGHT");
        ok &= same;
    }
    return frames->close() && ok ? 0 : 1;
}
