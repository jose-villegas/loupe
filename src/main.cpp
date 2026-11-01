// loupe: opens a scene's frames in the window; with -o or --video, writes them and exits.
#include <cstdio>
#include <filesystem>
#include <format>
#include <optional>
#include <random>
#include <string>
#include <system_error>

#include "engine/frames.hpp"
#include "options.hpp"
#include "window/run_window.hpp"

namespace {

// A window run's stand-in for -o. autana's close writes the last frame, to stdout
// without -o, and with it captures the engine's stdout beside the image
// (out.bmp.console). The window wants neither, so both go to a scratch file this
// removes (autana-kl3ab: a close that writes no image).
class ScratchImage {
  public:
    ScratchImage()
        : path_(std::filesystem::temp_directory_path() / std::format("loupe-{}.bmp", std::random_device{}())) {}

    ~ScratchImage() {
        std::error_code ignored;
        std::filesystem::remove(path_, ignored);
        std::filesystem::remove(path_.string() + ".console", ignored);
    }

    ScratchImage(const ScratchImage&) = delete;
    ScratchImage& operator=(const ScratchImage&) = delete;
    ScratchImage(ScratchImage&&) = delete;
    ScratchImage& operator=(ScratchImage&&) = delete;

    std::string
    path() const {
        return path_.string();
    }

  private:
    std::filesystem::path path_;
};

} // namespace

int
main(int argc, char** argv) {
    loupe::Options options;
    std::string why;
    if (!loupe::parse_options(argc, argv, &options, &why)) {
        std::fprintf(stderr, "loupe: %s\n%s", why.c_str(), loupe::usage());
        return 2;
    }
    std::optional<ScratchImage> scratch;
    if (options.out_bmp.empty() && options.out_video.empty()) {
        options.out_bmp = scratch.emplace().path();
    }
    std::unique_ptr<loupe::Frames> frames = loupe::Frames::open(options, &why);
    if (frames == nullptr) {
        std::fprintf(stderr, "loupe: %s\n", why.c_str());
        return 1;
    }
    if (scratch) {
        const bool ok = loupe::run_window(*frames, options);
        return frames->close() && ok ? 0 : 1;
    }
    const int count = frames->count();
    for (int i = 0; i < count; i++) {
        frames->step(frames->dt_ms());
    }
    return frames->close() ? 0 : 1;
}
