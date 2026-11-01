// What every Frames test does: open a scene, copy a frame's bytes, seek and compare.
#pragma once

#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include "engine/frames.hpp"

namespace loupe::test {

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

inline std::vector<uint8_t>
pixels(const Frames& frames) {
    const Frame frame = frames.frame();
    return {frame.bgr.begin(), frame.bgr.end()};
}

// Seeks to frame `index` and compares its bytes with `expected`, said on stderr.
inline bool
same(const char* test, Frames& frames, int index, const std::vector<uint8_t>& expected, const char* what) {
    const bool sought = frames.seek(index);
    const bool ok = sought && pixels(frames) == expected;
    std::fprintf(stderr, "%s: %s frame %d %s\n", test, what, index, ok ? "same" : sought ? "DIFFERS" : "NOT SOUGHT");
    return ok;
}

} // namespace loupe::test
