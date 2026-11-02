#include "library.hpp"

#include <algorithm>
#include <fstream>
#include <string>
#include <system_error>

namespace fs = std::filesystem;

namespace loupe {

namespace {

constexpr const char RECENT_FILE[] = "recent.txt";
constexpr const char PACK_EXTENSION[] = ".apak";

// The .apak files in `folder`, by name; none when there is no such folder.
std::vector<fs::path>
packs_in(const fs::path& folder) {
    std::vector<fs::path> packs;
    std::error_code error;
    for (const fs::directory_entry& entry : fs::directory_iterator(folder, error)) {
        if (entry.is_regular_file(error) && entry.path().extension() == PACK_EXTENSION) {
            packs.push_back(entry.path());
        }
    }
    std::ranges::sort(packs);
    return packs;
}

// <config>/recent.txt's scenes, one UTF-8 path a line, most recent first.
std::vector<fs::path>
read_recent(const fs::path& config) {
    std::vector<fs::path> recent;
    std::ifstream in(config / RECENT_FILE, std::ios::binary);
    std::string line;
    while (std::getline(in, line)) {
        if (line.ends_with('\r')) {
            line.pop_back();
        }
        if (!line.empty()) {
            recent.push_back(from_utf8(line));
        }
    }
    return recent;
}

} // namespace

std::string
utf8(const fs::path& path) {
    const std::u8string text = path.u8string();
    return {text.begin(), text.end()};
}

fs::path
from_utf8(std::string_view text) {
    return {std::u8string(text.begin(), text.end())};
}

Library
library(const Folders& folders) {
    Library found;
    for (const fs::path& folder : {folders.exe_dir / ".." / "share" / "loupe" / "scenes", folders.exe_dir / "scenes"}) {
        const std::vector<fs::path> packs = packs_in(folder);
        found.samples.insert(found.samples.end(), packs.begin(), packs.end());
    }
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
    std::error_code error;
    const fs::path absolute = fs::absolute(scene, error).lexically_normal();
    if (error) {
        return false;
    }
    std::vector<fs::path> recent = read_recent(config);
    std::erase(recent, absolute);
    recent.insert(recent.begin(), absolute);
    recent.resize(std::min(recent.size(), RECENT_KEPT));
    fs::create_directories(config, error);
    std::ofstream out(config / RECENT_FILE, std::ios::binary | std::ios::trunc);
    for (const fs::path& path : recent) {
        const std::u8string text = path.u8string();
        out << std::string(text.begin(), text.end()) << '\n';
    }
    return out.good();
}

} // namespace loupe
