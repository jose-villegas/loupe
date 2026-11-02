#include "tools.hpp"

#include <filesystem>
#include <string>
#include <system_error>
#include <vector>

#include "paths.hpp"
#include "process.hpp"

namespace fs = std::filesystem;

namespace loupe {

namespace {

#if LOUPE_PACK_TOOL
constexpr const char* AUTANA_DIR = LOUPE_AUTANA_DIR;
constexpr const char* PACK_CACHE = LOUPE_PACK_CACHE;
constexpr const char* PYTHON = LOUPE_PYTHON;
constexpr const char* GIT = LOUPE_GIT;
#else
constexpr const char* AUTANA_DIR = "";
constexpr const char* PACK_CACHE = "";
constexpr const char* PYTHON = "";
constexpr const char* GIT = "";
#endif

// False with `why` set in a build without the pack tool.
bool
has_tool(std::string* why) {
    if (!LOUPE_PACK_TOOL) {
        *why = "this loupe was built without the pack tool";
    }
    return LOUPE_PACK_TOOL != 0;
}

// `program` with `args` after it, its output into `log`.
bool
run(const std::string& program, const std::vector<std::string>& args, const fs::path& log) {
    std::vector<std::string> line = {program};
    line.insert(line.end(), args.begin(), args.end());
    return run_logged(line, log);
}

} // namespace

bool
builds_packs() {
    return LOUPE_PACK_TOOL != 0;
}

fs::path
autana_dir() {
    return AUTANA_DIR;
}

fs::path
pack_cache() {
    return PACK_CACHE;
}

bool
build_pack(const PackBuild& build, std::string* why) {
    if (!has_tool(why)) {
        return false;
    }
    std::error_code error;
    if (fs::create_directories(build.folder, error), error) {
        *why = "cannot make " + utf8(build.folder) + ": " + error.message();
        return false;
    }
    const fs::path script = build.checkout / "launcher" / "tools" / "r3d" / "build_pack.py";
    const fs::path log = build.folder / "build_pack.log";
    if (!run(PYTHON, {utf8(script), "-o", utf8(build.folder), utf8(build.scene)}, log)) {
        *why = "build_pack.py failed:\n" + read_text(log);
        return false;
    }
    return true;
}

std::optional<std::string>
git(const std::vector<std::string>& args, const fs::path& log, std::string* why) {
    if (!has_tool(why)) {
        return std::nullopt;
    }
    const bool ok = run(GIT, args, log);
    std::string out = read_text(log);
    if (!ok) {
        *why = "git failed:\n" + out;
        return std::nullopt;
    }
    out.erase(out.find_last_not_of("\r\n") + 1);
    return out;
}

bool
render_compare(const std::vector<std::string>& args, const fs::path& log, std::string* why) {
    if (!has_tool(why)) {
        return false;
    }
    const fs::path script = autana_dir() / "launcher" / "tools" / "render" / "render_compare.py";
    std::vector<std::string> line = {utf8(script)};
    line.insert(line.end(), args.begin(), args.end());
    if (!run(PYTHON, line, log)) {
        *why = "render_compare.py failed";
        return false;
    }
    return true;
}

} // namespace loupe
