// loupe: opens a scene in the window, or the window's start panel without one;
// with -o or --video, writes the scene's frames and exits; with --frame or
// --compare, writes a frame of its camera path or an A | B video of it.
#include <cstdio>
#include <string>

#include "options.hpp"
#include "path/path.hpp"
#include "version.hpp"
#include "window/run_window.hpp"

int
main(int argc, char** argv) {
    loupe::Options options;
    std::string why;
    if (!loupe::parse_options(argc, argv, &options, &why)) {
        std::fprintf(stderr, "loupe: %s\n%s", why.c_str(), loupe::usage());
        return 2;
    }
    if (options.version) {
        std::printf("%s\n", loupe::version_line());
        return 0;
    }
    if (options.out_bmp.empty() && options.out_video.empty()) {
        return loupe::run_window(options) ? 0 : 1;
    }
    if (options.frame || !options.compare.empty()) {
        return loupe::run_path(options);
    }
    if (!loupe::render(options, &why)) {
        std::fprintf(stderr, "loupe: %s\n", why.c_str());
        return 1;
    }
    return 0;
}
