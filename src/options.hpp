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
    // A path's frame, and the A | B compare (src/path/compare.hpp): the side B
    // checkout, a git revision or a folder, and B's renderer flags, which
    // replace A's --object, --camera, --view and --set when given.
    std::optional<int> frame;                       // --frame
    std::string compare;                            // --compare
    std::optional<std::vector<std::string>> b_args; // --b-args, split into words
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
// Side B's options: A's, with --b-args' renderer flags in place of A's when
// given. False with `why` set for words --b-args cannot take.
bool b_options(const Options& a, Options* b, std::string* why);
// A whole decimal number from `least` to `most`; false for anything else.
bool parse_count(const std::string& text, long long least, long long most, long long* value);

} // namespace loupe
