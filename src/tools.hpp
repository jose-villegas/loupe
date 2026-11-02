// The programs a build with the pack tool runs: autana's build_pack.py and
// render_compare.py, and git. A build without it refuses each call, saying so.
#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace loupe {

// One build_pack.py run: the checkout whose script builds, the .scene.toml it
// builds, and the folder the pack and its build_pack.log go to.
struct PackBuild {
    std::filesystem::path checkout;
    std::filesystem::path scene;
    std::filesystem::path folder;
};

// True in builds with the pack tool; every call below refuses elsewhere.
bool builds_packs();
// The autana checkout loupe is built against.
std::filesystem::path autana_dir();
// Where packs built from a .scene.toml are kept, one folder per scene file.
std::filesystem::path pack_cache();
// Runs `build`; false with `why` set when it failed.
bool build_pack(const PackBuild& build, std::string* why);
// Git with `args`, its output into `log`; the output, trailing newlines
// dropped, or none with `why` set.
std::optional<std::string> git(const std::vector<std::string>& args, const std::filesystem::path& log,
                               std::string* why);
// autana's render_compare.py with `args`, its output into `log`; false with `why` set.
bool render_compare(const std::vector<std::string>& args, const std::filesystem::path& log, std::string* why);

} // namespace loupe
