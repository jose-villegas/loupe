// What every Frames test does: open a scene, copy a frame's bytes, seek and compare.
#pragma once

#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "engine/frames.hpp"

namespace loupe::test {

// `scene_toml` under `view` (the provider's own when empty), writing its BMP to
// `out_bmp`; null, said on stderr under `test`'s name, when it cannot open.
inline std::unique_ptr<Frames>
open_scene(const char* test, const char* scene_toml, std::string out_bmp, std::string view = {}) {
    Options options;
    options.scene_toml = scene_toml;
    options.out_bmp = std::move(out_bmp);
    options.view = std::move(view);
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
