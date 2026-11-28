// The programs a build with the pack tool runs: autana's tools (build_pack.py,
// render_compare.py, scene_edit.py, bake.py) and git. A build without it
// refuses each call, saying so.
#pragma once

#include <filesystem>
#include <optional>
#include <stop_token>
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
// The command line that runs that autana's CLI with `args`: its own
// launcher, tools/autana (autana.cmd on Windows), never an autana on PATH,
// so the boards, their packs and Record share one version. Empty in a build
// without the pack tool.
std::vector<std::string> autana_cli(const std::vector<std::string>& args);
// Where packs built from a .scene.toml are kept, one folder per scene file.
std::filesystem::path pack_cache();
// Runs `build`; false with `why` set when it failed.
bool build_pack(const PackBuild& build, std::string* why);
// Git with `args`, its output into `log`; the output, trailing newlines
// dropped, or none with `why` set.
std::optional<std::string> git(const std::vector<std::string>& args, const std::filesystem::path& log,
                               std::string* why);
// autana's tool `script` (a path under launcher/tools) on the pack tool's
// Python with `args`, its output into `log`; the output, trailing newlines
// dropped, or none with `why` set to the script and its output. A request on
// `stop` ends it, and it counts as failed.
std::optional<std::string> autana_tool(const std::filesystem::path& script, const std::vector<std::string>& args,
                                       const std::filesystem::path& log, std::string* why,
                                       const std::stop_token& stop = {});
// autana's render_compare.py with `args`, its output into `log`; false with `why` set.
bool render_compare(const std::vector<std::string>& args, const std::filesystem::path& log, std::string* why);

} // namespace loupe
