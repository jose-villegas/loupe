// A scene's asset pack: a .apak as it is, or one autana's build_pack.py builds
// from a .scene.toml into the build's cache, in builds with the pack tool.
#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace loupe {

// The two scene files loupe opens: autana's asset pack, and the scene it is built from.
inline constexpr std::string_view PACK_SUFFIX = ".apak";
inline constexpr std::string_view SCENE_SUFFIX = ".scene.toml";

// What a run opens: <folder>/<id>.apak, the asset dir and scene id autana loads.
struct ScenePack {
    std::filesystem::path folder;
    std::string id;
};

// The pack's file: <folder>/<id>.apak.
std::filesystem::path pack_file(const ScenePack& pack);
// The scene id a .apak or .scene.toml file names: its file name without the
// suffix; empty for any other file.
std::string scene_id(const std::filesystem::path& file);

// A .apak opens as it is. A .scene.toml is built first, and brought up to date
// on every call, where builds_packs() (tools.hpp); elsewhere it is refused. None, with
// `why` set, for a scene that cannot open.
std::optional<ScenePack> resolve_scene(const std::filesystem::path& path, std::string* why);

} // namespace loupe
