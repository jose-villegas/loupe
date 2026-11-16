// Recording against a fake `autana record`: every stream byte arrives through
// take() and in the saved file, console lines reach the child's stdin, and
// stop() ends it. parse_boards reads `autana devices --json`'s shape.
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <thread>
#include <vector>

#include "paths.hpp"
#include "window/recording.hpp"

namespace fs = std::filesystem;

namespace {

constexpr const char TEST[] = "recording_test";
// Several of the fake's pieces, and not a whole number of them.
constexpr size_t PAYLOAD_BYTES = 300 * 1024 + 17;
// How long the fake may take to end once stdin is closed.
constexpr auto END_WITHIN = std::chrono::seconds(20);
constexpr auto POLL = std::chrono::milliseconds(10);
// What fake_record.cpp writes last.
constexpr const char DONE[] = "fake_record: done";
// `autana devices --json`'s shape: a board plugged in here, one on another host, one held.
constexpr const char DEVICES[] = R"({"boards": [
  {"board": "90:70:69:FE:A3:08", "port": "COM5", "state": "unlocked", "holder": null},
  {"board": "44:BD:8D:78:20:E8", "port": null, "state": "unlocked", "holder": null},
  {"board": "AA:BB:CC:DD:EE:FF", "port": "COM7", "state": "locked", "holder": "someone"}],
 "heavy": {"board": "heavy", "port": null, "state": "unlocked"}})";

bool
expect(bool ok, const char* what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, what, ok ? "ok" : "FAILED");
    return ok;
}

std::vector<uint8_t>
read_bytes(const fs::path& file) {
    std::ifstream in(file, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

bool
boards_parse() {
    std::string why;
    const std::vector<loupe::window::Board> boards = loupe::window::parse_boards(DEVICES, &why);
    bool ok = expect(boards.size() == 3 && why.empty(), "three boards listed");
    ok &=
        expect(boards.size() == 3 && boards[0].id == "90:70:69:FE:A3:08" && boards[0].port == "COM5" && boards[0].free,
               "a board plugged in here is free with its port");
    ok &= expect(boards.size() == 3 && boards[1].port.empty() && boards[1].free, "a board on another host has no port");
    ok &= expect(boards.size() == 3 && !boards[2].free, "a held board is not free");
    std::string broken_why;
    ok &= expect(loupe::window::parse_boards("{\"boards\": [{\"port\": null}]}", &broken_why).empty()
                     && !broken_why.empty(),
                 "a board without its fields is refused");
    return ok;
}

} // namespace

int
main(int argc, char** argv) {
    if (argc != 3) {
        std::fprintf(stderr, "usage: %s FAKE_RECORD FOLDER\n", TEST);
        return 2;
    }
    const fs::path folder = argv[2];
    fs::create_directories(folder);
    std::vector<uint8_t> payload(PAYLOAD_BYTES);
    for (size_t i = 0; i < payload.size(); i++) {
        payload[i] = static_cast<uint8_t>((i * 31U) ^ (i >> 8U));
    }
    const fs::path payload_file = folder / "payload.bin";
    std::ofstream(payload_file, std::ios::binary)
        .write(reinterpret_cast<const char*>(payload.data()), static_cast<std::streamsize>(payload.size()));

    bool ok = boards_parse();
    std::string why;
    const fs::path saved = folder / "board.framerec";
    std::unique_ptr<loupe::window::Recording> recording =
        loupe::window::Recording::start({argv[1], loupe::utf8(payload_file)}, saved, &why);
    if (recording == nullptr) {
        std::fprintf(stderr, "%s: %s\n", TEST, why.c_str());
        return 1;
    }
    ok &= expect(recording->send("FREEZE") && recording->send("STEP"), "console lines sent");
    std::vector<uint8_t> taken;
    const auto deadline = std::chrono::steady_clock::now() + END_WITHIN;
    while (taken.size() < payload.size() && std::chrono::steady_clock::now() < deadline) {
        recording->take(taken);
        std::this_thread::sleep_for(POLL);
    }
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
    ok &= expect(log.size() == 3 && log[0] == "FREEZE" && log[1] == "STEP", "the child read the lines sent");
    recording.reset();
    ok &= expect(read_bytes(saved) == payload, "the saved file holds the stream");
    return ok ? 0 : 1;
}
