// A board link against a fake `autana record`: every stream byte arrives
// through take(), stderr goes to the log, console lines reach the child's stdin, and
// stop() ends it; a child that never ends is let go within its bound, and a
// link that cannot start says so at once. A board's serial names its log. The
// child gets no console window of its own, and its exit code comes back.
// parse_boards reads `autana devices --json`'s shape, naming who holds a board
// a command holds; a listing is one autana call, and the last one kept reads back.
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#endif

#include "board/boards.hpp"
#include "paths.hpp"
#include "tools.hpp"
#include "window/board_link.hpp"

namespace fs = std::filesystem;

namespace {

constexpr const char TEST[] = "board_link_test";
// Several of the fake's pieces, and not a whole number of them.
constexpr size_t PAYLOAD_BYTES = 300 * 1024 + 17;
// How long the fake may take to end once stdin is closed.
constexpr auto END_WITHIN = std::chrono::seconds(20);
constexpr auto POLL = std::chrono::milliseconds(10);
// The bound a hung child is given, and how long letting it go may take in all.
constexpr auto HUNG_BOUND = std::chrono::milliseconds(500);
constexpr auto HUNG_LET_GO = std::chrono::seconds(10);
// How long a link that cannot start may take to say so.
constexpr auto REFUSED_WITHIN = std::chrono::seconds(1);
// A board's serial as autana lists it, and its file stem.
constexpr const char SERIAL[] = "90:70:69:FE:A3:08";
constexpr const char SERIAL_STEM[] = "907069FEA308";
// What fake_record.cpp writes last.
constexpr const char DONE[] = "fake_record: done";
// What fake_record.cpp --report writes and exits with when it has no console window.
constexpr const char NO_WINDOW[] = "fake_record: console window no";
constexpr int REPORT_EXIT = 3;
// `autana devices --json`'s shape after a note on stderr: a board plugged in
// here, one on another host, one held.
constexpr const char DEVICES[] =
    "gpu-runner could not answer\n"
    R"({"boards": [)"
    R"({"board": "90:70:69:FE:A3:08", "port": "COM5", "state": "unlocked", "holder": null}, )"
    R"({"board": "44:BD:8D:78:20:E8", "port": null, "state": "unlocked", "holder": null}, )"
    R"({"board": "AA:BB:CC:DD:EE:FF", "port": "COM7", "state": "held", )"
    R"("holder": {"owner": "sam@bench:board-job:8552", "purpose": "autana lock run"}, "estimated_free": 0}, )"
    R"({"board": "90:70:69:FE:A3:09", "port": "COM9", "state": "human", )"
    R"("holder": {"owner": "sam@bench:39780", "purpose": "checking the panel"}}]})"
    "\n";

// The boards DEVICES lists.
constexpr size_t BOARDS = 4;
// A lock token as autana prints it.
constexpr const char TOKEN[] = "0123456789abcdef0123456789abcdef";

bool
expect(bool ok, const char* what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, what, ok ? "ok" : "FAILED");
    return ok;
}

bool
boards_parse() {
    const loupe::BoardList list = loupe::parse_boards(DEVICES);
    const std::vector<loupe::Board>& boards = list.boards;
    using loupe::Holder;
    bool ok = expect(boards.size() == BOARDS && list.why.empty(), "every board listed past a stderr note");
    if (boards.size() != BOARDS) {
        return false;
    }
    ok &= expect(boards[0].id == "90:70:69:FE:A3:08" && boards[0].port == "COM5" && boards[0].holder == Holder::Nobody
                     && loupe::recordable(boards[0]),
                 "a free board plugged in here records, with its port");
    ok &= expect(boards[1].port.empty() && !loupe::recordable(boards[1]), "a board on another host has no port");
    ok &= expect(boards[2].holder == Holder::Command && !loupe::recordable(boards[2]),
                 "a board a command holds does not record");
    ok &= expect(boards[2].busy.starts_with("held by board-job: autana lock run, free ~"),
                 "it says which program holds it, for what, and when it should be free");
    ok &= expect(boards[0].busy.empty() && boards[3].busy.empty(), "a free or reserved board is not busy");
    ok &= expect(loupe::recordable(boards[2], true) && !loupe::recordable(boards[1], true),
                 "inside that command's hold it records, still only plugged in here");
    ok &= expect(boards[3].holder == Holder::Person && boards[3].note == "checking the panel"
                     && loupe::recordable(boards[3]),
                 "a person's reserved board records inside the reservation, its note kept");
    const loupe::BoardList broken = loupe::parse_boards(R"({"boards": [{"port": null}]})");
    ok &= expect(broken.boards.empty() && !broken.why.empty(), "a board without its fields is refused");
    return ok;
}

// A listing runs autana once, `devices --json`, whatever the boards; what it
// found is kept and reads back, and nothing kept reads as no boards.
bool
one_call_a_listing(const fs::path& folder) {
    int calls = 0;
    std::vector<std::string> ran;
    const loupe::Run fake = [&](const std::vector<std::string>& args, const fs::path& log) {
        ++calls;
        ran = args;
        std::ofstream(log, std::ios::binary) << DEVICES;
        return true;
    };
    const loupe::BoardList list = loupe::list_boards(folder / "devices.log", fake);
    if (!loupe::builds_packs()) {
        return expect(calls == 0 && list.boards.empty() && !list.why.empty(),
                      "without the pack tool there is no autana to ask, and the list says so");
    }
    bool ok = expect(calls == 1 && list.boards.size() == BOARDS, "four boards come from one autana call");
    ok &= expect(ran.size() >= 4 && ran[ran.size() - 2] == "devices" && ran.back() == "--json",
                 "the call is autana devices --json");
    // The pinned checkout's own launcher, by its whole path: no autana from PATH.
#if defined(_WIN32)
    const fs::path cli = loupe::autana_dir() / "tools" / "autana.cmd";
#else
    const fs::path cli = loupe::autana_dir() / "tools" / "autana";
#endif
    ok &= expect(!ran.empty() && fs::path(ran[0]).is_absolute() && std::ranges::find(ran, loupe::utf8(cli)) != ran.end()
                     && fs::exists(cli),
                 "it runs the pinned autana's CLI, never one on PATH");
    const fs::path kept = folder / "kept" / "boards.json";
    std::error_code error;
    fs::remove(kept, error);
    ok &= expect(loupe::kept_boards(kept).boards.empty(), "nothing kept yet reads as no boards");
    ok &= expect(loupe::keep_boards(list, kept), "the listing is kept");
    const loupe::BoardList back = loupe::kept_boards(kept);
    ok &= expect(back.boards.size() == BOARDS && back.boards[2].busy == list.boards[2].busy,
                 "the kept listing reads back, holders and all");
    return ok;
}

// The reservation's token is the line `lock hand` prints alone; an autana that
// prints none gives none.
bool
token_found() {
    const std::string renewed = std::string("human reservation renewed; it expires in 1h\r\n") + TOKEN + "\r\n";
    bool ok = expect(loupe::token_in(renewed) == TOKEN, "the token line, past the renewal message");
    ok &= expect(loupe::token_in("human reservation recorded; it expires in 1h\n").empty(),
                 "an autana that prints no token gives none");
    ok &= expect(loupe::token_in(std::string(TOKEN) + "0\n").empty(), "a longer hex line is not a token");
    return ok;
}

// A link whose child ends on stop: every byte, the lines sent, its last line.
bool
ends_on_stop(const fs::path& fake, const fs::path& folder, const std::vector<uint8_t>& payload) {
    std::string why;
    const fs::path log_file = folder / "board.log";
    std::unique_ptr<loupe::window::BoardLink> link =
        loupe::window::BoardLink::start({loupe::utf8(fake), loupe::utf8(folder / "payload.bin")}, log_file, &why);
    if (link == nullptr) {
        std::fprintf(stderr, "%s: %s\n", TEST, why.c_str());
        return false;
    }
    bool ok = expect(link->send("FREEZE") && link->send("STEP"), "console lines sent");
    std::vector<uint8_t> taken;
    const auto deadline = std::chrono::steady_clock::now() + END_WITHIN;
    while (taken.size() < payload.size() && std::chrono::steady_clock::now() < deadline) {
        link->take(taken);
        std::this_thread::sleep_for(POLL);
    }
    // While it runs, the log's lines come in as written: STEP, echoed after
    // FRAMEREC_BEGIN, is the latest, and the begin stays seen past it.
    loupe::window::BoardLink::Status running = link->status();
    while (running.latest != "STEP" && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(POLL);
        running = link->status();
    }
    ok &= expect(running.running && running.latest == "STEP" && running.began,
                 "the latest log line is read while it runs, and the begin before it is kept");
    ok &= expect(running.received == payload.size(), "every stream byte is counted as received");
    ok &= expect(running.log == log_file, "the status names the log");
    link->stop();
    ok &= expect(!link->send("RESUME"), "nothing is sent after stop");
    loupe::window::BoardLink::Status status = link->status();
    while (status.running && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(POLL);
        status = link->status();
    }
    link->take(taken);
    ok &= expect(!status.running && status.exit == 0 && status.stopped, "it ended on stop, exit 0");
    ok &= expect(taken == payload, "take() gave every stream byte, in order");
    ok &= expect(status.last_line == DONE, "the last stderr line is read back");
    const std::vector<std::string> log = loupe::read_lines(log_file);
    ok &= expect(log.size() == 4 && log[1] == "FREEZE" && log[2] == "STEP", "the child read the lines sent");
    return ok;
}

// A child that never ends: dropping its link returns within the bound.
bool
hung_is_let_go(const fs::path& fake, const fs::path& folder) {
    std::string why;
    std::unique_ptr<loupe::window::BoardLink> link = loupe::window::BoardLink::start(
        {loupe::utf8(fake), loupe::utf8(folder / "payload.bin"), "--hang"}, folder / "hung.log", &why, HUNG_BOUND);
    if (link == nullptr) {
        std::fprintf(stderr, "%s: %s\n", TEST, why.c_str());
        return false;
    }
    const auto began = std::chrono::steady_clock::now();
    link.reset();
    return expect(std::chrono::steady_clock::now() - began < HUNG_LET_GO, "a child that never ends is let go");
}

// A log that cannot be written refuses the link at once, with no child to
// wait for: the window waited the whole END_WITHIN for one that never ran.
bool
unwritable_log_refused_at_once(const fs::path& fake, const fs::path& folder) {
    const fs::path not_a_folder = folder / "not-a-folder";
    std::ofstream(not_a_folder) << "a file";
    std::string why;
    const auto began = std::chrono::steady_clock::now();
    const std::unique_ptr<loupe::window::BoardLink> link = loupe::window::BoardLink::start(
        {loupe::utf8(fake), loupe::utf8(folder / "payload.bin")}, not_a_folder / "board.log", &why);
    const auto took = std::chrono::steady_clock::now() - began;
    return expect(link == nullptr && !why.empty() && took < REFUSED_WITHIN,
                  "a log that cannot be written refuses the link at once");
}

// A board's serial names its log: the colons go, and the link starts with it.
bool
serial_names_a_log(const fs::path& fake, const fs::path& folder) {
    const std::string stem = loupe::board_file_stem(SERIAL);
    bool ok = expect(stem == SERIAL_STEM, "a serial's file stem drops its colons");
    std::string why;
    const std::unique_ptr<loupe::window::BoardLink> link = loupe::window::BoardLink::start(
        {loupe::utf8(fake), loupe::utf8(folder / "payload.bin")}, folder / (stem + ".log"), &why, HUNG_BOUND);
    ok &= expect(link != nullptr, "a link logs to its board's file stem");
    if (link == nullptr) {
        std::fprintf(stderr, "%s: %s\n", TEST, why.c_str());
    }
    return ok;
}

// The child runs with no console window of its own, which Windows would open
// on the desktop for a console program started from a GUI one; and its exit
// code is the one it gave.
bool
no_console_window(const fs::path& fake, const fs::path& folder) {
#if defined(_WIN32)
    // As Loupe, a GUI program: no console for the child to share. Its output
    // still reaches ctest, which reads it from a pipe.
    FreeConsole();
#endif
    std::string why;
    const std::unique_ptr<loupe::window::BoardLink> link =
        loupe::window::BoardLink::start({loupe::utf8(fake), "--report"}, folder / "report.log", &why);
    if (link == nullptr) {
        std::fprintf(stderr, "%s: %s\n", TEST, why.c_str());
        return false;
    }
    const auto deadline = std::chrono::steady_clock::now() + END_WITHIN;
    loupe::window::BoardLink::Status status = link->status();
    while (status.running && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(POLL);
        status = link->status();
    }
    std::fprintf(stderr, "%s: the child said \"%s\", exit %d\n", TEST, status.last_line.c_str(),
                 status.exit.value_or(-1));
    bool ok = expect(status.last_line == NO_WINDOW, "the child has no console window");
    ok &= expect(status.exit == REPORT_EXIT, "the child's exit code comes back");
    return ok;
}

} // namespace

int
main(int argc, char** argv) {
    if (argc != 3) {
        std::fprintf(stderr, "usage: %s FAKE_RECORD FOLDER\n", TEST);
        return 2;
    }
    const fs::path fake = argv[1];
    const fs::path folder = argv[2];
    fs::create_directories(folder);
    std::vector<uint8_t> payload(PAYLOAD_BYTES);
    for (size_t i = 0; i < payload.size(); i++) {
        payload[i] = static_cast<uint8_t>((i * 31U) ^ (i >> 8U));
    }
    std::ofstream(folder / "payload.bin", std::ios::binary)
        .write(reinterpret_cast<const char*>(payload.data()), static_cast<std::streamsize>(payload.size()));

    bool ok = boards_parse();
    ok &= token_found();
    ok &= one_call_a_listing(folder);
    ok &= ends_on_stop(fake, folder, payload);
    ok &= hung_is_let_go(fake, folder);
    ok &= unwritable_log_refused_at_once(fake, folder);
    ok &= serial_names_a_log(fake, folder);
    ok &= no_console_window(fake, folder);
    return ok ? 0 : 1;
}
