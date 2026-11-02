// A scene's asset pack: a .apak as it is, or one autana's build_pack.py builds
// from a .scene.toml into the build's cache, in builds with the pack tool.
#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace loupe {

// What a run opens: <folder>/<id>.apak, the asset dir and scene id autana loads.
struct ScenePack {
    std::filesystem::path folder;
    std::string id;
};

// True in builds with LOUPE_PACK_TOOL, which build a .scene.toml's pack.
bool builds_packs();

// A .apak opens as it is. A .scene.toml is built first, and brought up to date
// on every call, where builds_packs(); elsewhere it is refused. None, with
// `why` set, for a scene that cannot open.
std::optional<ScenePack> resolve_scene(const std::filesystem::path& path, std::string* why);

} // namespace loupe
