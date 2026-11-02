// A process spawn() starts runs on its own: this test starts itself again,
// found by exe_path(), with an argument holding a space and a quote, and the
// child writes that argument where the parent waits for it.
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <thread>

#include "process.hpp"

namespace fs = std::filesystem;

namespace {

constexpr const char TEST[] = "process_test";
constexpr const char CHILD[] = "--child";
constexpr const char ARGUMENT[] = "a \"quoted\" word";
constexpr auto WAIT = std::chrono::seconds(30);
constexpr auto POLL = std::chrono::milliseconds(50);

std::string
read_all(const fs::path& file) {
    std::ifstream in(file, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

} // namespace

int
main(int argc, char** argv) {
    if (argc == 4 && std::string(argv[1]) == CHILD) {
        const fs::path out = argv[2];
        std::ofstream(fs::path(out).concat(".part"), std::ios::binary) << argv[3];
        fs::rename(fs::path(out).concat(".part"), out);
        return 0;
    }
    if (argc != 2) {
        std::fprintf(stderr, "usage: %s OUT_FILE\n", TEST);
        return 2;
    }
    const fs::path out = fs::absolute(argv[1]);
    fs::remove(out);
    const fs::path self = loupe::exe_path();
    if (self.empty() || !loupe::spawn({self.string(), CHILD, out.string(), ARGUMENT})) {
        std::fprintf(stderr, "%s: could not start %s\n", TEST, self.string().c_str());
        return 1;
    }
    for (auto waited = std::chrono::milliseconds(0); !fs::exists(out) && waited < WAIT; waited += POLL) {
        std::this_thread::sleep_for(POLL);
    }
    const bool ok = read_all(out) == ARGUMENT;
    std::fprintf(stderr, "%s: the child %s\n", TEST, ok ? "got its argument" : "did not write its argument");
    return ok ? 0 : 1;
}
