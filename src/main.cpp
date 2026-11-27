// loupe: opens a scene in the window, or the window's start panel without one;
// with -o or --video, writes the scene's frames and exits; with --frame or
// --compare, writes a frame of its camera path or an A | B video of it.
#include <cstdio>
#include <string>

#include "engine/frames.hpp"
#include "log/log.hpp"
#include "options.hpp"
#include "path/path.hpp"
#include "process.hpp"
#include "version.hpp"
#include "window/run_window.hpp"

int
main(int argc, char** argv) {
    loupe::log::attach_terminal();
    loupe::Options options;
    std::string why;
    if (!loupe::parse_options(argc, argv, &options, &why)) {
        loupe::log::error(why);
        std::fputs(loupe::usage(), stderr);
        return 2;
    }
    if (options.version) {
        std::printf("%s\n", loupe::version_line(loupe::build_id(loupe::exe_path().parent_path())).c_str());
        return 0;
    }
    if (options.out_bmp.empty() && options.out_video.empty()) {
        return loupe::run_window(options) ? 0 : 1;
    }
    if (options.frame || !options.compare.empty()) {
        return loupe::run_path(options);
    }
    if (!loupe::render(options, &why)) {
        loupe::log::error(why);
        return 1;
    }
    return 0;
}
