#include "process.hpp"

#include <cstddef>
#include <cstdlib>
#include <optional>
#include <string_view>
#include <system_error>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
// After windows.h, which it needs.
#include <shellapi.h>
#else
#include <chrono>
#include <csignal>
#include <thread>

#include <fcntl.h>
#include <spawn.h>
#include <sys/wait.h>
extern char** environ;
#endif

#include "paths.hpp"

namespace fs = std::filesystem;

namespace loupe {

namespace {

#if defined(_WIN32)
// One argument as the C runtime splits a command line back into argv; one
// with no space, tab or quote as it is, since some programs read their
// command line raw (an NSIS setup takes /S only unquoted).
std::string
argv_quoted(const std::string& arg) {
    if (!arg.empty() && arg.find_first_of(" \t\n\v\"") == std::string::npos) {
        return arg;
    }
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

// `args` as one command line, quoted for the C runtime.
std::string
command_line(const std::vector<std::string>& args) {
    std::string line;
    for (const std::string& arg : args) {
        line += (line.empty() ? "" : " ") + argv_quoted(arg);
    }
    return line;
}

// Starts `args` with `startup`, its handles inherited; the process, or none.
std::optional<PROCESS_INFORMATION>
start(const std::vector<std::string>& args, STARTUPINFOA& startup) {
    std::string line = command_line(args);
    startup.cb = sizeof startup;
    PROCESS_INFORMATION process = {};
    if (!CreateProcessA(args[0].c_str(), line.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &startup,
                        &process)) {
        return std::nullopt;
    }
    CloseHandle(process.hThread);
    return process;
}
#else
// How often a waiting run_logged() looks for a stop request.
constexpr auto STOP_POLL = std::chrono::milliseconds(50);

// `args` as the null-ended argv posix_spawn takes; pointers into `args`.
std::vector<char*>
argv_of(const std::vector<std::string>& args) {
    std::vector<char*> argv;
    argv.reserve(args.size() + 1);
    for (const std::string& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);
    return argv;
}
#endif

} // namespace

bool
run_logged(const std::vector<std::string>& args, const fs::path& log, const std::stop_token& stop) {
#if defined(_WIN32)
    SECURITY_ATTRIBUTES inherit = {sizeof inherit, nullptr, TRUE};
    const HANDLE file = CreateFileA(utf8(log).c_str(), GENERIC_WRITE, FILE_SHARE_READ, &inherit, CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    STARTUPINFOA startup = {};
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = file;
    startup.hStdError = file;
    DWORD status = 1;
    if (const std::optional<PROCESS_INFORMATION> process = start(args, startup)) {
        {
            const std::stop_callback end(stop, [&process] { TerminateProcess(process->hProcess, 1); });
            WaitForSingleObject(process->hProcess, INFINITE);
        }
        GetExitCodeProcess(process->hProcess, &status);
        CloseHandle(process->hProcess);
    }
    CloseHandle(file);
    return status == 0;
#else
    std::vector<char*> argv = argv_of(args);
    posix_spawn_file_actions_t actions;
    posix_spawn_file_actions_init(&actions);
    posix_spawn_file_actions_addopen(&actions, 1, log.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    posix_spawn_file_actions_adddup2(&actions, 1, 2);
    pid_t pid = 0;
    int status = 0;
    const bool started = posix_spawn(&pid, args[0].c_str(), &actions, nullptr, argv.data(), environ) == 0;
    posix_spawn_file_actions_destroy(&actions);
    if (!started) {
        return false;
    }
    // Polled, not killed from the stopping thread: a pid already waited for may be another process's.
    pid_t waited = 0;
    while ((waited = waitpid(pid, &status, WNOHANG)) == 0) {
        if (stop.stop_requested()) {
            kill(pid, SIGKILL);
            waited = waitpid(pid, &status, 0);
            break;
        }
        std::this_thread::sleep_for(STOP_POLL);
    }
    return waited == pid && WIFEXITED(status) && WEXITSTATUS(status) == 0;
#endif
}

fs::path
exe_path() {
#if defined(_WIN32)
    std::wstring path(MAX_PATH, L'\0');
    for (;;) {
        const DWORD length = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
        if (length == 0) {
            return {};
        }
        if (length < path.size()) {
            path.resize(length);
            return path;
        }
        path.resize(path.size() * 2);
    }
#else
    std::error_code error;
    const fs::path path = fs::read_symlink("/proc/self/exe", error);
    return error ? fs::path() : path;
#endif
}

fs::path
system_program(const std::string& name) {
#if defined(_WIN32)
    std::wstring dir(MAX_PATH, L'\0');
    const UINT length = GetSystemDirectoryW(dir.data(), static_cast<UINT>(dir.size()));
    if (length == 0 || length >= dir.size()) {
        return {};
    }
    dir.resize(length);
    const fs::path program = fs::path(dir) / (name + ".exe");
#else
    const char* path = std::getenv("PATH");
    fs::path program;
    std::error_code error;
    for (std::string_view rest = path != nullptr ? path : ""; program.empty() && !rest.empty();) {
        const size_t colon = rest.find(':');
        const fs::path candidate = fs::path(rest.substr(0, colon)) / name;
        if (!candidate.parent_path().empty() && fs::is_regular_file(candidate, error)) {
            program = candidate;
        }
        rest = colon == std::string_view::npos ? std::string_view() : rest.substr(colon + 1);
    }
#endif
    std::error_code missing;
    return fs::is_regular_file(program, missing) ? program : fs::path();
}

bool
start_unwaited(const fs::path& program, const std::vector<std::string>& args) {
#if defined(_WIN32)
    const std::wstring parameters = from_utf8(command_line(args)).wstring();
    const std::wstring file = program.wstring();
    SHELLEXECUTEINFOW info = {};
    info.cbSize = sizeof info;
    info.fMask = SEE_MASK_NOASYNC;
    info.lpFile = file.c_str();
    info.lpParameters = parameters.c_str();
    info.nShow = SW_SHOWNORMAL;
    return ShellExecuteExW(&info) != FALSE;
#else
    std::vector<std::string> line = {program.string()};
    line.insert(line.end(), args.begin(), args.end());
    std::vector<char*> argv = argv_of(line);
    pid_t pid = 0;
    return posix_spawn(&pid, line[0].c_str(), nullptr, nullptr, argv.data(), environ) == 0;
#endif
}

} // namespace loupe
