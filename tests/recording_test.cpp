// Recording against a fake `autana record`: every stream byte arrives through
// take() and in the saved file, console lines reach the child's stdin, and
// stop() ends it; a child that never ends is let go within its bound.
// parse_boards reads `autana devices --json`'s shape.
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

#include "board/boards.hpp"
#include "paths.hpp"
#include "read_bytes.hpp"
#include "window/recording.hpp"

namespace fs = std::filesystem;

namespace {

constexpr const char TEST[] = "recording_test";
// Several of the fake's pieces, and not a whole number of them.
constexpr size_t PAYLOAD_BYTES = 300 * 1024 + 17;
// How long the fake may take to end once stdin is closed.
constexpr auto END_WITHIN = std::chrono::seconds(20);
constexpr auto POLL = std::chrono::milliseconds(10);
// The bound a hung child is given, and how long letting it go may take in all.
constexpr auto HUNG_BOUND = std::chrono::milliseconds(500);
constexpr auto HUNG_LET_GO = std::chrono::seconds(10);
// What fake_record.cpp writes last.
constexpr const char DONE[] = "fake_record: done";
// `autana devices --json`'s shape after a note on stderr: a board plugged in
// here, one on another host, one held.
constexpr const char DEVICES[] =
    "gpu-runner could not answer\n"
    R"({"boards": [)"
    R"({"board": "90:70:69:FE:A3:08", "port": "COM5", "state": "unlocked", "holder": null}, )"
    R"({"board": "44:BD:8D:78:20:E8", "port": null, "state": "unlocked", "holder": null}, )"
    R"({"board": "AA:BB:CC:DD:EE:FF", "port": "COM7", "state": "held", "holder": "someone"}, )"
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
    ok &= expect(boards[3].holder == Holder::Person && boards[3].note == "checking the panel"
                     && loupe::recordable(boards[3]),
                 "a person's reserved board records inside the reservation, its note kept");
    const loupe::BoardList broken = loupe::parse_boards(R"({"boards": [{"port": null}]})");
    ok &= expect(broken.boards.empty() && !broken.why.empty(), "a board without its fields is refused");
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

// A recording whose child ends on stop: every byte, the lines sent, its last line.
bool
ends_on_stop(const fs::path& fake, const fs::path& folder, const std::vector<uint8_t>& payload) {
    std::string why;
    const fs::path saved = folder / "board.framerec";
    std::unique_ptr<loupe::window::Recording> recording =
        loupe::window::Recording::start({loupe::utf8(fake), loupe::utf8(folder / "payload.bin")}, saved, &why);
    if (recording == nullptr) {
        std::fprintf(stderr, "%s: %s\n", TEST, why.c_str());
        return false;
    }
    bool ok = expect(recording->send("FREEZE") && recording->send("STEP"), "console lines sent");
    std::vector<uint8_t> taken;
    const auto deadline = std::chrono::steady_clock::now() + END_WITHIN;
    while (taken.size() < payload.size() && std::chrono::steady_clock::now() < deadline) {
        recording->take(taken);
        std::this_thread::sleep_for(POLL);
    }
    // While it runs, the log's lines come in as written: STEP, echoed after
    // FRAMEREC_BEGIN, is the latest, and the begin stays seen past it.
    loupe::window::Recording::Status running = recording->status();
    while (running.latest != "STEP" && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(POLL);
        running = recording->status();
    }
    ok &= expect(running.running && running.latest == "STEP" && running.began,
                 "the latest log line is read while it runs, and the begin before it is kept");
    ok &= expect(running.received == payload.size(), "every stream byte is counted as received");
    ok &= expect(running.log == fs::path(saved).concat(".log"), "the status names the log");
    recording->stop();
    ok &= expect(!recording->send("RESUME"), "nothing is sent after stop");
    loupe::window::Recording::Status status = recording->status();
    while (status.running && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(POLL);
        status = recording->status();
    }
    recording->take(taken);
    ok &= expect(!status.running && status.exit == 0 && status.stopped, "it ended on stop, exit 0");
    ok &= expect(taken == payload, "take() gave every stream byte, in order");
    ok &= expect(status.last_line == DONE, "the last stderr line is read back");
    const std::vector<std::string> log = loupe::read_lines(fs::path(saved).concat(".log"));
    ok &= expect(log.size() == 4 && log[1] == "FREEZE" && log[2] == "STEP", "the child read the lines sent");
    recording.reset();
    ok &= expect(loupe::test::read_bytes(saved) == payload, "the saved file holds the stream");
    return ok;
}

// A child that never ends: dropping its recording returns within the bound.
bool
hung_is_let_go(const fs::path& fake, const fs::path& folder) {
    std::string why;
    std::unique_ptr<loupe::window::Recording> recording = loupe::window::Recording::start(
        {loupe::utf8(fake), loupe::utf8(folder / "payload.bin"), "--hang"}, folder / "hung.framerec", &why, HUNG_BOUND);
    if (recording == nullptr) {
        std::fprintf(stderr, "%s: %s\n", TEST, why.c_str());
        return false;
    }
    const auto began = std::chrono::steady_clock::now();
    recording.reset();
    return expect(std::chrono::steady_clock::now() - began < HUNG_LET_GO, "a child that never ends is let go");
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
    ok &= ends_on_stop(fake, folder, payload);
    ok &= hung_is_let_go(fake, folder);
    return ok ? 0 : 1;
}
