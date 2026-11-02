// loupe: opens a scene in the window, or the window's start panel without one;
// with -o or --video, writes the scene's frames and exits.
#include <cstdio>
#include <memory>
#include <string>

#include "engine/frames.hpp"
#include "options.hpp"
#include "window/run_window.hpp"

int
main(int argc, char** argv) {
    loupe::Options options;
    std::string why;
    if (!loupe::parse_options(argc, argv, &options, &why)) {
        std::fprintf(stderr, "loupe: %s\n%s", why.c_str(), loupe::usage());
        return 2;
    }
    if (options.out_bmp.empty() && options.out_video.empty()) {
        return loupe::run_window(options) ? 0 : 1;
    }
    std::unique_ptr<loupe::Frames> frames = loupe::Frames::open(options, &why);
    if (frames == nullptr) {
        std::fprintf(stderr, "loupe: %s\n", why.c_str());
        return 1;
    }
    const int count = frames->count();
    for (int i = 0; i < count; i++) {
        frames->step(frames->dt_ms());
    }
    return frames->close() ? 0 : 1;
}
