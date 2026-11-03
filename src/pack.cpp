#include "pack.hpp"

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <string>
#include <system_error>

#include "engine/autana.hpp"
#include "paths.hpp"
#include "tools.hpp"

namespace fs = std::filesystem;

namespace loupe {

namespace {

// One cache folder per scene file, wherever two scenes share a name.
uint64_t
path_hash(const std::string& text) {
    return name_hash64_n(NAME_HASH64_OFFSET_BASIS, text.data(), text.size());
}

// `path`'s file name without `suffix`, or empty when it does not end in it.
std::string
stem_before(const fs::path& path, std::string_view suffix) {
    const std::string name = utf8(path.filename());
    return name.size() > suffix.size() && name.ends_with(suffix) ? name.substr(0, name.size() - suffix.size())
                                                                 : std::string();
}

// Builds `scene_toml`'s pack `id`, one cache folder per scene file.
std::optional<ScenePack>
built_pack(const fs::path& scene_toml, const std::string& id, std::string* why) {
    std::error_code error;
    const fs::path scene = fs::canonical(scene_toml, error);
    if (error) {
        *why = "no scene file: " + utf8(scene_toml);
        return std::nullopt;
    }
    char tag[17];
    std::snprintf(tag, sizeof tag, "%016llx", static_cast<unsigned long long>(path_hash(scene.generic_string())));
    const fs::path folder = pack_cache() / (id + "-" + tag);
    if (!build_pack({.checkout = autana_dir(), .scene = scene, .folder = folder}, why)) {
        return std::nullopt;
    }
    return ScenePack{.folder = folder, .id = id};
}

} // namespace

std::filesystem::path
pack_file(const ScenePack& pack) {
    return pack.folder / (pack.id + std::string(PACK_SUFFIX));
}

std::string
scene_id(const fs::path& file) {
    const std::string pack = stem_before(file, PACK_SUFFIX);
    return pack.empty() ? stem_before(file, SCENE_SUFFIX) : pack;
}

std::optional<ScenePack>
resolve_scene(const fs::path& path, std::string* why) {
    if (const std::string id = stem_before(path, PACK_SUFFIX); !id.empty()) {
        std::error_code error;
        const fs::path pack = fs::canonical(path, error);
        if (error || !fs::is_regular_file(pack, error)) {
            *why = "no scene pack: " + utf8(path);
            return std::nullopt;
        }
        return ScenePack{.folder = pack.parent_path(), .id = id};
    }
    const std::string id = stem_before(path, SCENE_SUFFIX);
    if (id.empty()) {
        *why = "expected a .apak or .scene.toml file: " + utf8(path);
        return std::nullopt;
    }
    if (!builds_packs()) {
        *why =
            "this loupe opens a scene's .apak; it was built without the pack tool a .scene.toml needs: " + utf8(path);
        return std::nullopt;
    }
    return built_pack(path, id, why);
}

} // namespace loupe
