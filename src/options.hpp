// What one Loupe run is asked for: the scene viewer's own flags, same names.
#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace loupe {

constexpr size_t MIB = size_t{1} << 20;
// The window's checkpoint budget when --checkpoint-mb is left out.
constexpr size_t DEFAULT_CHECKPOINT_MB = 1024;

// A flag left out keeps the provider's own default.
struct Options {
    // A .apak or a .scene.toml (resolve_scene); none opens the window on its start panel.
    std::filesystem::path scene;
    std::vector<std::string> objects; // --object, repeatable
    std::string camera;               // --camera
    std::string view;                 // --view
    std::optional<int> frames;        // --frames
    std::optional<uint32_t> dt_ms;    // --dt
    std::vector<std::string> sets;    // --set owner.name=value, repeatable
    std::string out_bmp, out_video;   // -o, --video
    // Loupe's own, for the window only: the checkpoints' memory budget
    // (DEFAULT_CHECKPOINT_MB), and how many ticks it runs before it closes itself
    // (scripted runs; until closed by default).
    std::optional<size_t> checkpoint_mb; // --checkpoint-mb
    std::optional<int> ticks;            // --ticks
    bool version = false;                // --version: print version_line() and exit
};

// False with `why` set for a command line it cannot run.
bool parse_options(int argc, char** argv, Options* options, std::string* why);
const char* usage();
// The command line that opens the window on `scene` picked from the start
// panel: the provider's defaults, and the window's checkpoint budget when
// one was given; `exe` is the loupe to start.
std::vector<std::string> window_args(const std::filesystem::path& exe, const std::filesystem::path& scene,
                                     std::optional<size_t> checkpoint_mb);
// A whole decimal number from `least` to `most`; false for anything else.
bool parse_count(const std::string& text, long long least, long long most, long long* value);

} // namespace loupe
