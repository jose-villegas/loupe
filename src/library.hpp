// The scenes the window's start panel offers: the sample packs shipped beside
// loupe, and the scenes opened last.
#pragma once

#include <cstddef>
#include <filesystem>
#include <vector>

namespace loupe {

// How many scenes the recent list keeps.
constexpr size_t RECENT_KEPT = 10;

struct Library {
    std::vector<std::filesystem::path> samples, recent;
};

// Where the library is kept: the folder loupe's executable is in, and loupe's settings folder.
struct Folders {
    std::filesystem::path exe_dir, config;
};

// Where the samples are installed beside loupe: <exe_dir>/scenes on Windows,
// <exe_dir>/../share/loupe/scenes elsewhere (cmake/package.cmake).
std::filesystem::path samples_dir(const Folders& folders);
// The .apak files in samples_dir(), and the recent list in <config>/recent.txt
// without the files gone since.
Library library(const Folders& folders);
// Puts `scene` first in <config>/recent.txt, RECENT_KEPT kept; false when it
// cannot be written, or there is no config folder.
bool remember(const Folders& folders, const std::filesystem::path& scene);

} // namespace loupe
