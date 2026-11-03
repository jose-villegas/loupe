// Which Loupe this is: its version and commit, the channel it was built for,
// whether it was installed, and the autana it was built against.
#pragma once

#include <filesystem>
#include <string>

namespace loupe {

// Where a build's updates come from (the LOUPE_CHANNEL CMake option): the
// GitHub release of each v<version> tag, or the rolling build of main. None
// for a local or test build, which never updates.
enum class Channel { None, Releases, Main };

struct BuildId {
    std::string version;       // 0.1.0, the project version
    std::string commit;        // Loupe's own commit, or "unknown" outside git
    std::string autana_commit; // the autana commit compiled
    Channel built_for = Channel::None;
    bool installed = false; // the setup.exe copy; else portable
};

// This build's id; `installed` when the installer's uninstaller sits in `exe_dir`.
BuildId build_id(const std::filesystem::path& exe_dir);
// "loupe 0.1.0 (1a2b3c4d5, main build, portable), autana 262e7d0ab", for --version and About.
std::string version_line(const BuildId& id);

} // namespace loupe
