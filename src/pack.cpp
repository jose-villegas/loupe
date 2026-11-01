#include "pack.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <system_error>

namespace fs = std::filesystem;

namespace loupe {

namespace {

constexpr const char SCENE_SUFFIX[] = ".scene.toml";
constexpr const char STAMP_NAME[] = "stamp";
constexpr uint64_t FNV_OFFSET = 14695981039346656037ull;
constexpr uint64_t FNV_PRIME = 1099511628211ull;

// One cache folder per scene file, wherever two scenes share a name.
uint64_t path_hash(const std::string& text) {
    uint64_t hash = FNV_OFFSET;
    for (const unsigned char c : text) {
        hash = (hash ^ c) * FNV_PRIME;
    }
    return hash;
}

fs::file_time_type newest(const fs::path& folder, const char* extension, fs::file_time_type since) {
    std::error_code error;
    for (fs::recursive_directory_iterator it(folder, error), end; !error && it != end; it.increment(error)) {
        if (it->is_regular_file(error) && (extension == nullptr || it->path().extension() == extension)) {
            since = std::max(since, it->last_write_time(error));
        }
    }
    return since;
}

// What the pack is made from: the scene's folder, autana's bake lock and the
// Python of its tools, which build_pack.py imports.
fs::file_time_type inputs_time(const fs::path& scene) {
    const fs::path launcher = fs::path(LOUPE_AUTANA_DIR) / "launcher";
    std::error_code error;
    const fs::file_time_type lock = fs::last_write_time(launcher / "bakes.lock", error);
    const fs::file_time_type time = newest(scene.parent_path(), nullptr, error ? fs::file_time_type::min() : lock);
    return newest(launcher / "tools", ".py", time);
}

std::string quoted(const std::string& text) {
    return "\"" + text + "\"";
}

}  // namespace

bool scene_pack(const std::string& scene_toml, std::string* pack_dir, std::string* why) {
    std::error_code error;
    const fs::path scene = fs::canonical(scene_toml, error);
    if (error) {
        *why = "no scene file: " + scene_toml;
        return false;
    }
    const std::string name = scene.filename().string();
    const std::string stem = name.substr(0, name.size() - (sizeof SCENE_SUFFIX - 1));
    char tag[17];
    std::snprintf(tag, sizeof tag, "%016llx", static_cast<unsigned long long>(path_hash(scene.generic_string())));
    const fs::path folder = fs::path(LOUPE_PACK_CACHE) / (stem + "-" + tag);
    *pack_dir = folder.string();

    const fs::path script = fs::path(LOUPE_AUTANA_DIR) / "launcher" / "tools" / "r3d" / "build_pack.py";
    std::string command = quoted(LOUPE_PYTHON) + " " + quoted(script.string()) + " -o " + quoted(folder.string()) +
                          " " + quoted(scene.string());
    const fs::path stamp = folder / STAMP_NAME;
    std::ifstream stamp_in(stamp);
    std::stringstream built;
    built << stamp_in.rdbuf();
    if (stamp_in && built.str() == command && fs::last_write_time(stamp, error) >= inputs_time(scene) && !error) {
        return true;
    }

    fs::create_directories(folder, error);
    std::fprintf(stderr, "loupe: building %s's pack\n", stem.c_str());
    std::string run = command + " > " + quoted((folder / "build_pack.log").string()) + " 2>&1";
#if defined(_WIN32)
    run = "\"" + run + "\"";  // cmd.exe drops the outer pair of quotes
#endif
    if (std::system(run.c_str()) != 0) {
        *why = "build_pack.py failed; see " + (folder / "build_pack.log").string();
        return false;
    }
    std::ofstream(stamp) << command;
    return true;
}

}  // namespace loupe
