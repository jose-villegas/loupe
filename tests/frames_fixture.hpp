// What every Frames test does: open a scene, step it straight while keeping
// frames' bytes, then seek and compare.
#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <map>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include "engine/frames.hpp"

namespace loupe::test {

// Frame index to the bytes stepping straight to it drew.
using Straight = std::map<int, std::vector<uint8_t>>;

// The run `options` asks for; null, said on stderr under `test`'s name, when it cannot open.
inline std::unique_ptr<Frames>
open_or_report(const char* test, const Options& options) {
    std::string why;
    std::unique_ptr<Frames> frames = Frames::open(options, &why);
    if (frames == nullptr) {
        std::fprintf(stderr, "%s: %s\n", test, why.c_str());
    }
    return frames;
}

// The scene `argv[1]`, writing its BMP to `argv[2]`.
inline Options
scene_args(char** argv) {
    Options options;
    options.scene_toml = argv[1];
    options.out_bmp = argv[2];
    return options;
}

inline std::vector<uint8_t>
pixels(const Frames& frames) {
    const Frame frame = frames.frame();
    return {frame.bgr.begin(), frame.bgr.end()};
}

// Steps until frame `to` - 1 is shown, keeping the bytes of each frame in
// `keep` (every frame when empty) and judging each step with `each`, given the
// shown frame; false when a step or a judgement failed.
inline bool
step_straight(Frames& frames, int to, std::span<const int> keep, Straight* straight,
              const std::function<bool(int shown)>& each = {}) {
    bool ok = true;
    while (frames.index() < to) {
        ok &= frames.step(frames.dt_ms());
        const int shown = frames.index() - 1;
        if (keep.empty() || std::ranges::find(keep, shown) != keep.end()) {
            (*straight)[shown] = pixels(frames);
        }
        ok &= !each || each(shown);
    }
    return ok;
}

// Seeks to frame `index` and compares its bytes with `expected`, said on stderr.
inline bool
same(const char* test, Frames& frames, int index, const std::vector<uint8_t>& expected, const char* what) {
    const bool sought = frames.seek(index);
    const bool ok = sought && pixels(frames) == expected;
    std::fprintf(stderr, "%s: %s frame %d %s\n", test, what, index, ok ? "same" : sought ? "DIFFERS" : "NOT SOUGHT");
    return ok;
}

// same() for every frame of `targets`, in order, against `straight`.
inline bool
same_each(const char* test, Frames& frames, std::span<const int> targets, const Straight& straight, const char* what) {
    bool ok = true;
    for (const int target : targets) {
        const auto kept = straight.find(target);
        ok &= kept != straight.end() && same(test, frames, target, kept->second, what);
    }
    return ok;
}

} // namespace loupe::test
