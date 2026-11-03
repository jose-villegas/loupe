// A build's id: version_line() names its version, commit, channel and whether
// it is installed; a copy is installed only with the installer's uninstaller
// (UNINSTALLER, given on Windows only) beside it.
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

#include "version.hpp"

namespace fs = std::filesystem;

namespace {

constexpr const char TEST[] = "build_id_test";

bool
check(bool ok, const std::string& what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, ok ? "ok" : "FAILED", what.c_str());
    return ok;
}

bool
check_lines() {
    using loupe::Channel;

    struct Case {
        Channel channel;
        bool installed;
        const char* line;
    };

    const Case cases[] = {
        {.channel = Channel::None,
         .installed = false,
         .line = "loupe 0.1.0 (1a2b3c4d5, local build, portable), autana 262e7d0ab"},
        {.channel = Channel::Releases,
         .installed = true,
         .line = "loupe 0.1.0 (1a2b3c4d5, release, installed), autana 262e7d0ab"},
        {.channel = Channel::Main,
         .installed = false,
         .line = "loupe 0.1.0 (1a2b3c4d5, main build, portable), autana 262e7d0ab"},
    };
    bool ok = true;
    for (const Case& c : cases) {
        const loupe::BuildId id = {.version = "0.1.0",
                                   .commit = "1a2b3c4d5",
                                   .autana_commit = "262e7d0ab",
                                   .built_for = c.channel,
                                   .installed = c.installed};
        const std::string line = loupe::version_line(id);
        ok &= check(line == c.line, line);
    }
    return ok;
}

bool
check_installed(const fs::path& out, const char* uninstaller) {
    fs::remove_all(out);
    fs::create_directories(out);
    bool ok = check(!loupe::build_id(out).installed, "an empty folder is portable");
    ok &= check(!loupe::build_id({}).installed, "no folder is portable");
    // Another file beside loupe, and a folder of the uninstaller's name, are not the uninstaller.
    const std::string name = uninstaller != nullptr ? uninstaller : "Uninstall.exe";
    std::ofstream(out / "LICENSE").put('\0');
    fs::create_directories(out / "nested" / name);
    ok &= check(!loupe::build_id(out).installed, "a folder without the uninstaller is portable");
    ok &= check(!loupe::build_id(out / "nested").installed, "a folder named as the uninstaller is not one");
    std::ofstream(out / name).put('\0');
    const bool installed = loupe::build_id(out).installed;
    if (uninstaller != nullptr) {
        ok &= check(installed, std::string("a folder with ") + name + " is installed");
    } else {
        ok &= check(!installed, "without an installer, nothing is installed");
    }
    return ok;
}

} // namespace

int
main(int argc, char** argv) {
    if (argc != 2 && argc != 3) {
        std::fprintf(stderr, "usage: %s OUT_DIR [UNINSTALLER]\n", TEST);
        return 2;
    }
    const bool lines = check_lines();
    const bool installed = check_installed(fs::absolute(argv[1]), argc == 3 ? argv[2] : nullptr);
    return lines && installed ? 0 : 1;
}
