// What one Loupe run is asked for: the scene viewer's own flags, same names.
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace loupe {

constexpr size_t MIB = size_t{1024} * 1024;
// The window's checkpoint budget when --checkpoint-mb is left out.
constexpr size_t DEFAULT_CHECKPOINT_MB = 1024;

// A flag left out keeps the provider's own default.
struct Options {
    std::string scene_toml;
    std::vector<std::string> objects; // --object, repeatable
    std::string camera;               // --camera
    std::string view;                 // --view
    std::optional<int> frames;        // --frames
    std::optional<uint32_t> dt_ms;    // --dt
    std::vector<std::string> sets;    // --set owner.name=value, repeatable
    std::string out_bmp, out_video;   // -o, --video
    // Loupe's own, for the window: the checkpoints' memory budget, and how many
    // ticks it runs before it closes itself (scripted runs; until closed by default).
    size_t checkpoint_mb = DEFAULT_CHECKPOINT_MB; // --checkpoint-mb
    std::optional<int> ticks;                     // --ticks
};

// False with `why` set for a command line it cannot run.
bool parse_options(int argc, char** argv, Options* options, std::string* why);
const char* usage();
// The scene's id, its file name without .scene.toml, as autana's packs name it.
std::string scene_id(const std::string& scene_toml);
// A whole decimal number from `least` to `most`; false for anything else.
bool parse_count(const std::string& text, long long least, long long most, long long* value);

} // namespace loupe
