// What one Loupe run is asked for: the scene viewer's own flags, same names.
#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace loupe {

constexpr size_t MIB = size_t{1} << 20;
// The window's checkpoint budget when --checkpoint-mb is left out.
constexpr size_t DEFAULT_CHECKPOINT_MB = 1024;

// The renderer's flags: what a run draws and how it looks.
struct RenderFlags {
    std::vector<std::string> objects; // --object, repeatable
    std::string camera;               // --camera
    std::string view;                 // --view
    std::vector<std::string> sets;    // --set owner.name=value, repeatable

    // Sets the flag `flag` names to `value`; false for a flag not the renderer's.
    bool set(std::string_view flag, const std::string& value);
    // The flags given, in the provider's command-line spelling.
    std::vector<std::string> args() const;
};

// A flag left out keeps the provider's own default.
struct Options {
    // A .apak or a .scene.toml (resolve_scene); none opens the window on its start panel.
    std::filesystem::path scene;
    RenderFlags render;
    std::optional<int> frames;      // --frames
    std::optional<uint32_t> dt_ms;  // --dt
    std::string out_bmp, out_video; // -o, --video
    // A path's frame, and the A | B compare (src/path/path.hpp): side B's
    // checkout, a git revision or a folder, and B's renderer flags in place of
    // A's when given.
    std::optional<int> frame;            // --frame
    std::string compare;                 // --compare
    std::optional<RenderFlags> b_render; // --b-args "FLAGS"
    // Loupe's own, for the window only: the checkpoints' memory budget
    // (DEFAULT_CHECKPOINT_MB), and how many ticks it runs before it closes itself
    // (scripted runs; until closed by default).
    std::optional<size_t> checkpoint_mb; // --checkpoint-mb
    std::optional<int> ticks;            // --ticks
    bool version = false;                // --version: print the build's version_line() and exit
};

// False with `why` set for a command line it cannot run.
bool parse_options(int argc, char** argv, Options* options, std::string* why);
const char* usage();
// A whole decimal number from `least` to `most`; false for anything else.
bool parse_count(const std::string& text, long long least, long long most, long long* value);

} // namespace loupe
