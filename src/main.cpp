// loupe: opens a scene's frames; with -o or --video, writes them and exits.
#include <cstdio>
#include <string>

#include "engine/frames.h"
#include "options.h"

int main(int argc, char** argv) {
    loupe::Options options;
    std::string why;
    if (!loupe::parse_options(argc, argv, &options, &why)) {
        std::fprintf(stderr, "loupe: %s\n%s", why.c_str(), loupe::usage());
        return 2;
    }
    if (options.out_bmp.empty() && options.out_video.empty()) {
        std::fprintf(stderr, "loupe: the window is not built yet; give -o or --video\n%s", loupe::usage());
        return 2;
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
