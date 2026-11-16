// A stand-in for `autana record`: writes PAYLOAD's bytes to stdout in pieces,
// echoes each stdin line to stderr, and ends once stdin closes, as autana
// stops when its stdin ends. With --hang it never ends, as a stuck autana.
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>
#endif

namespace {

// A pipe-sized piece, so the reader sees the stream arrive in several reads.
constexpr size_t PIECE = 4096;
// What it writes last, which Recording::Status::last_line reads back.
constexpr const char DONE[] = "fake_record: done";
constexpr std::string_view HANG = "--hang";
constexpr auto HANG_NAP = std::chrono::seconds(1);

} // namespace

int
main(int argc, char** argv) {
    const bool hang = argc == 3 && argv[2] == HANG;
    if (argc != 2 && !hang) {
        std::fprintf(stderr, "usage: fake_record PAYLOAD [--hang]\n");
        return 2;
    }
#if defined(_WIN32)
    (void)_setmode(_fileno(stdout), _O_BINARY);
#endif
    std::ifstream file(argv[1], std::ios::binary);
    const std::vector<char> payload((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    for (size_t at = 0; at < payload.size(); at += PIECE) {
        const size_t count = std::min(PIECE, payload.size() - at);
        if (std::fwrite(payload.data() + at, 1, count, stdout) != count) {
            return 1;
        }
        std::fflush(stdout);
    }
    for (std::string line; std::getline(std::cin, line);) {
        std::fprintf(stderr, "%s\n", line.c_str());
        std::fflush(stderr);
    }
    if (hang) {
        for (;;) {
            std::this_thread::sleep_for(HANG_NAP);
        }
    }
    std::fprintf(stderr, "%s\n", DONE);
    return 0;
}
