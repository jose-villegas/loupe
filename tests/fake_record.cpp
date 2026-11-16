// A stand-in for `autana record`: writes PAYLOAD's bytes to stdout in pieces,
// echoes each stdin line to stderr, and ends once stdin closes, as autana
// stops when its stdin ends.
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
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

} // namespace

int
main(int argc, char** argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: fake_record PAYLOAD\n");
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
    std::fprintf(stderr, "%s\n", DONE);
    return 0;
}
