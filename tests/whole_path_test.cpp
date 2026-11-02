// A window's run covers the scene's whole camera path when --frames is not
// given, and exactly --frames when it is.
#include <cstdint>
#include <cstdio>
#include <memory>
#include <optional>
#include <span>
#include <string>

#include "frames_fixture.hpp"

namespace {

constexpr const char TEST[] = "whole_path_test";
constexpr const char USAGE[] = "SCENE.apak out.bmp";
// Any count shorter than the path and past the window's first seekable frame.
constexpr int GIVEN_FRAMES = loupe::Frames::FIRST_SEEKABLE + 3;

std::unique_ptr<loupe::Frames>
open_whole(const loupe::Options& options) {
    std::string why;
    std::unique_ptr<loupe::Frames> frames = loupe::open_whole_path(options, &why);
    if (frames == nullptr) {
        std::fprintf(stderr, "%s: %s\n", TEST, why.c_str());
    }
    return frames;
}

} // namespace

int
main(int argc, char** argv) {
    const std::span<char*> args(argv, static_cast<size_t>(argc));
    if (args.size() != 3) {
        std::fprintf(stderr, "usage: %s %s\n", TEST, USAGE);
        return 2;
    }
    loupe::Options options;
    options.scene = args[1];
    options.out_bmp = args[2];

    int own = 0;
    if (std::unique_ptr<loupe::Frames> plain = loupe::test::open_or_report(TEST, options)) {
        own = plain->count();
    } else {
        return 1;
    }

    std::unique_ptr<loupe::Frames> whole = open_whole(options);
    if (whole == nullptr) {
        return 1;
    }
    const std::optional<uint32_t> path_ms = whole->path_ms();
    if (!path_ms) {
        std::fprintf(stderr, "%s: %s has no camera path\n", TEST, args[1]);
        return 1;
    }
    const int expected = loupe::frames_on_path(*path_ms, whole->dt_ms());
    if (whole->count() != expected || expected <= own) {
        std::fprintf(stderr, "%s: %d frames over a %u ms path at %u ms, %d expected; the provider's own run is %d\n",
                     TEST, whole->count(), *path_ms, whole->dt_ms(), expected, own);
        return 1;
    }
    whole.reset();

    options.frames = GIVEN_FRAMES;
    const std::unique_ptr<loupe::Frames> given = open_whole(options);
    if (given == nullptr) {
        return 1;
    }
    if (given->count() != GIVEN_FRAMES) {
        std::fprintf(stderr, "%s: --frames %d gave %d frames\n", TEST, GIVEN_FRAMES, given->count());
        return 1;
    }
    return 0;
}
