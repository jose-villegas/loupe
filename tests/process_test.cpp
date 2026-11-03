// Other processes: a tool run_logged() starts gets its arguments intact, and
// its output lands in the log (this test copies itself into a folder named
// outside any one Windows code page, runs the copy with an argument holding a
// space and a quote, and the copy writes it with C's fopen, as autana opens
// its packs, beside itself, then says so on stdout); a stop request ends a
// tool still running; and start_unwaited() passes a plain switch as it is,
// unquoted, as NSIS's /S must arrive.
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <stop_token>
#include <string>
#include <thread>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include "paths.hpp"
#include "process.hpp"

namespace fs = std::filesystem;
using std::chrono::steady_clock;

namespace {

constexpr const char TEST[] = "process_test";
constexpr const char CHILD[] = "--child";
constexpr const char SLEEP[] = "--sleep";
constexpr const char LINE[] = "--line";
constexpr const char ARGUMENT[] = "a \"quoted\" word";
constexpr const char SAID[] = "the child wrote its argument";
// "é 漢", spelled in UTF-8 bytes: no one Windows code page holds both.
constexpr const char FOLDER[] = "\xC3\xA9 \xE6\xBC\xA2";
// How long the sleeping child would run, how soon it is asked to stop, and
// how long the test waits for anything.
constexpr auto CHILD_SLEEP = std::chrono::seconds(60);
constexpr auto STOP_AFTER = std::chrono::milliseconds(200);
constexpr auto PATIENCE = std::chrono::seconds(10);
constexpr auto POLL = std::chrono::milliseconds(50);

// The child: writes `text` to `out` with C's narrow file calls, and says so.
int
child(const std::string& out, const std::string& text) {
    const std::unique_ptr<std::FILE, int (*)(std::FILE*)> file(std::fopen(out.c_str(), "wb"), &std::fclose);
    if (file == nullptr || std::fputs(text.c_str(), file.get()) < 0) {
        return 1;
    }
    std::puts(SAID);
    return 0;
}

// The child: writes the command line it was started with to `out`, as the
// system handed it over: raw on Windows, its arguments by spaces elsewhere.
int
line_child(int argc, char** argv, const fs::path& out) {
#if defined(_WIN32)
    (void)argc;
    (void)argv;
    const std::string line = loupe::utf8(fs::path(GetCommandLineW()));
#else
    std::string line;
    for (int i = 0; i < argc; i++) {
        line += (i == 0 ? "" : " ") + std::string(argv[i]);
    }
#endif
    const fs::path temp = fs::path(out) += ".tmp";
    std::ofstream(temp, std::ios::binary) << line;
    std::error_code error;
    fs::rename(temp, out, error);
    return error ? 1 : 0;
}

std::string
read_all(const fs::path& file) {
    const std::ifstream in(file, std::ios::binary);
    std::ostringstream text;
    text << in.rdbuf();
    return text.str();
}

// This test's copy in `folder`, which main() makes.
fs::path
copy_in(const fs::path& folder) {
    return folder / loupe::exe_path().filename();
}

bool
check(bool ok, const std::string& what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, ok ? "ok" : "FAILED", what.c_str());
    return ok;
}

bool
check_logged(const fs::path& folder) {
    const fs::path copy = copy_in(folder);
    const fs::path out = folder / "out.txt";
    const fs::path log = folder / "child.log";
    const bool ran = loupe::run_logged({loupe::utf8(copy), CHILD, loupe::utf8(out), ARGUMENT}, log);
    bool ok = check(ran && read_all(out) == ARGUMENT, "the child wrote its argument");
    ok &= check(read_all(log).starts_with(SAID), "its output is in the log");
    return ok;
}

bool
check_stopped(const fs::path& folder) {
    const fs::path copy = copy_in(folder);
    std::stop_source stop;
    const steady_clock::time_point start = steady_clock::now();
    const std::jthread asker([&stop] {
        std::this_thread::sleep_for(STOP_AFTER);
        stop.request_stop();
    });
    const bool ran = loupe::run_logged({loupe::utf8(copy), SLEEP}, folder / "sleep.log", stop.get_token());
    return check(!ran && steady_clock::now() - start < PATIENCE, "a stop request ends a child still running");
}

bool
check_unwaited(const fs::path& folder) {
    const fs::path copy = copy_in(folder);
    const fs::path out = folder / "line.txt";
    bool ok = check(loupe::start_unwaited(copy, {LINE, loupe::utf8(out), "/S"}), "start_unwaited starts the child");
    const steady_clock::time_point deadline = steady_clock::now() + PATIENCE;
    while (!fs::exists(out) && steady_clock::now() < deadline) {
        std::this_thread::sleep_for(POLL);
    }
    const std::string line = read_all(out);
    ok &= check(line.ends_with(" /S"), "a plain switch arrives unquoted: " + line);
    return ok;
}

} // namespace

int
main(int argc, char** argv) {
    if (argc == 4 && std::string(argv[1]) == CHILD) {
        return child(argv[2], argv[3]);
    }
    if (argc == 2 && std::string(argv[1]) == SLEEP) {
        std::this_thread::sleep_for(CHILD_SLEEP);
        return 0;
    }
    if (argc >= 3 && std::string(argv[1]) == LINE) {
        return line_child(argc, argv, loupe::from_utf8(argv[2]));
    }
    if (argc != 2) {
        std::fprintf(stderr, "usage: %s OUT_DIR\n", TEST);
        return 2;
    }
    const fs::path folder = fs::absolute(argv[1]) / loupe::from_utf8(FOLDER);
    fs::remove_all(folder);
    fs::create_directories(folder);
    fs::copy_file(loupe::exe_path(), copy_in(folder));
    bool ok = check_logged(folder);
    ok &= check_stopped(folder);
    ok &= check_unwaited(folder);
    return ok ? 0 : 1;
}
