#include "pack.h"

#include "options.h"

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <system_error>
#include <vector>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <fcntl.h>
#include <spawn.h>
#include <sys/wait.h>
extern char** environ;
#endif

namespace fs = std::filesystem;

namespace loupe {

namespace {

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

#if defined(_WIN32)
// One argument as the C runtime splits a command line back into argv.
std::string argv_quoted(const std::string& arg) {
    std::string out = "\"";
    size_t slashes = 0;
    for (const char c : arg) {
        if (c == '\\') {
            slashes++;
            continue;
        }
        out.append(c == '"' ? 2 * slashes + 1 : slashes, '\\');
        slashes = 0;
        out += c;
    }
    out.append(2 * slashes, '\\');
    return out + "\"";
}
#endif

// Runs `args` with no shell between, its output into `log`; true when it exits 0.
bool run_logged(const std::vector<std::string>& args, const fs::path& log) {
#if defined(_WIN32)
    std::string line;
    for (const std::string& arg : args) {
        line += (line.empty() ? "" : " ") + argv_quoted(arg);
    }
    SECURITY_ATTRIBUTES inherit = {sizeof inherit, nullptr, TRUE};
    const HANDLE file = CreateFileA(log.string().c_str(), GENERIC_WRITE, FILE_SHARE_READ, &inherit, CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    STARTUPINFOA startup = {};
    startup.cb = sizeof startup;
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = file;
    startup.hStdError = file;
    PROCESS_INFORMATION process = {};
    DWORD status = 1;
    if (CreateProcessA(args[0].c_str(), line.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &startup,
                       &process)) {
        WaitForSingleObject(process.hProcess, INFINITE);
        GetExitCodeProcess(process.hProcess, &status);
        CloseHandle(process.hThread);
        CloseHandle(process.hProcess);
    }
    CloseHandle(file);
    return status == 0;
#else
    std::vector<char*> argv;
    for (const std::string& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);
    posix_spawn_file_actions_t actions;
    posix_spawn_file_actions_init(&actions);
    posix_spawn_file_actions_addopen(&actions, 1, log.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    posix_spawn_file_actions_adddup2(&actions, 1, 2);
    pid_t pid = 0;
    int status = 0;
    const bool ok = posix_spawn(&pid, args[0].c_str(), &actions, nullptr, argv.data(), environ) == 0 &&
                    waitpid(pid, &status, 0) == pid && WIFEXITED(status) && WEXITSTATUS(status) == 0;
    posix_spawn_file_actions_destroy(&actions);
    return ok;
#endif
}

}  // namespace

bool scene_pack(const std::string& scene_toml, std::string* pack_dir, std::string* why) {
    std::error_code error;
    const fs::path scene = fs::canonical(scene_toml, error);
    if (error) {
        *why = "no scene file: " + scene_toml;
        return false;
    }
    const std::string id = scene_id(scene_toml);
    char tag[17];
    std::snprintf(tag, sizeof tag, "%016llx", static_cast<unsigned long long>(path_hash(scene.generic_string())));
    const fs::path folder = fs::path(LOUPE_PACK_CACHE) / (id + "-" + tag);
    *pack_dir = folder.string();

    const fs::path script = fs::path(LOUPE_AUTANA_DIR) / "launcher" / "tools" / "r3d" / "build_pack.py";
    const std::vector<std::string> args = {LOUPE_PYTHON, script.string(), "-o", folder.string(), scene.string()};
    if (fs::create_directories(folder, error), error) {
        *why = "cannot make " + folder.string() + ": " + error.message();
        return false;
    }
    if (!run_logged(args, folder / "build_pack.log")) {
        *why = "build_pack.py failed; see " + (folder / "build_pack.log").string();
        return false;
    }
    return true;
}

}  // namespace loupe
