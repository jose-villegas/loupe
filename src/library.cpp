#include "library.hpp"

#include "pack.hpp"
#include "paths.hpp"

#include <algorithm>
#include <string>
#include <system_error>
#include <utility>

namespace fs = std::filesystem;

namespace loupe {

namespace {

constexpr const char RECENT_FILE[] = "recent.txt";

// The .apak files in `folder`, by name; none when there is no such folder.
std::vector<fs::path>
packs_in(const fs::path& folder) {
    std::vector<fs::path> packs;
    std::error_code error;
    for (const fs::directory_entry& entry : fs::directory_iterator(folder, error)) {
        if (entry.is_regular_file(error) && entry.path().extension() == PACK_SUFFIX) {
            packs.push_back(entry.path());
        }
    }
    std::ranges::sort(packs);
    return packs;
}

// <config>/recent.txt's scenes, one UTF-8 path a line, most recent first;
// none without a config folder.
std::vector<fs::path>
read_recent(const fs::path& config) {
    std::vector<fs::path> recent;
    if (config.empty()) {
        return recent;
    }
    for (const std::string& line : read_lines(config / RECENT_FILE)) {
        if (!line.empty()) {
            recent.push_back(from_utf8(line));
        }
    }
    return recent;
}

} // namespace

fs::path
samples_dir(const Folders& folders) {
    return folders.exe_dir / LOUPE_SAMPLES_DIR;
}

Library
library(const Folders& folders) {
    Library found;
    found.samples = packs_in(samples_dir(folders));
    std::error_code error;
    for (const fs::path& scene : read_recent(folders.config)) {
        if (fs::is_regular_file(scene, error)) {
            found.recent.push_back(scene);
        }
    }
    return found;
}

bool
remember(const Folders& folders, const fs::path& scene) {
    const fs::path& config = folders.config;
    if (config.empty()) {
        return false;
    }
    std::error_code error;
    const fs::path absolute = fs::absolute(scene, error).lexically_normal();
    if (error) {
        return false;
    }
    std::vector<fs::path> recent = {absolute};
    for (fs::path& path : read_recent(config)) {
        if (path != absolute && recent.size() < RECENT_KEPT) {
            recent.push_back(std::move(path));
        }
    }
    std::vector<std::string> lines;
    lines.reserve(recent.size());
    for (const fs::path& path : recent) {
        lines.push_back(utf8(path));
    }
    return write_lines(config / RECENT_FILE, lines);
}

} // namespace loupe
