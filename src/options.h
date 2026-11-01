// What one Loupe run is asked for: the scene viewer's own flags, same names.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace loupe {

struct Options {
    std::string scene_toml;
    std::vector<std::string> objects;  // --object, repeatable
    std::string camera;                // --camera
    std::string view = "shaded";       // --view
    int frames = 30;                   // --frames
    uint32_t dt_ms = 16;               // --dt
    std::vector<std::string> sets;     // --set owner.name=value, repeatable
    std::string out_bmp, out_video;    // -o, --video
};

// False with `why` set for a command line it cannot run.
bool parse_options(int argc, char** argv, Options* options, std::string* why);
const char* usage();

}  // namespace loupe
