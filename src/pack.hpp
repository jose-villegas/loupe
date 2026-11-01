// A scene's asset pack, built by autana's own build_pack.py into the build's cache.
#pragma once

#include <optional>
#include <string>

namespace loupe {

// The folder holding the scene's pack, which build_pack.py brings up to date
// on every call. None, with `why` set, when the pack cannot be built.
std::optional<std::string> scene_pack(const std::string& scene_toml, std::string* why);

} // namespace loupe
