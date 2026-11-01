// A scene's asset pack, built by autana's own build_pack.py into the build's cache.
#pragma once

#include <string>

namespace loupe {

// The folder holding the scene's pack, rebuilt only when its inputs are newer.
// False with `why` set when the pack cannot be built.
bool scene_pack(const std::string& scene_toml, std::string* pack_dir, std::string* why);

}  // namespace loupe
