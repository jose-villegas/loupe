// A process spawn() starts runs on its own, and paths cross into it intact:
// this test copies itself into a folder named outside any one Windows code
// page, starts the copy with an argument holding a space and a quote, and the
// copy writes it with C's fopen, as autana opens its packs, beside itself.
// The child first waits for the parent to say spawn() returned, so a spawn()
// that waited for its child fails too.
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <thread>

#include "paths.hpp"
#include "process.hpp"

namespace fs = std::filesystem;

namespace {

constexpr const char TEST[] = "process_test";
constexpr const char CHILD[] = "--child";
constexpr const char ARGUMENT[] = "a \"quoted\" word";
// "é 漢", spelled in UTF-8 bytes: no one Windows code page holds both.
constexpr const char FOLDER[] = "\xC3\xA9 \xE6\xBC\xA2";
constexpr const char ACK[] = ".ack";
constexpr auto WAIT = std::chrono::seconds(30);
constexpr auto POLL = std::chrono::milliseconds(50);

// True once `file` exists, false after WAIT without it.
bool
wait_for(const fs::path& file) {
    for (auto waited = std::chrono::milliseconds(0); waited < WAIT; waited += POLL) {
        if (fs::exists(file)) {
            return true;
        }
        std::this_thread::sleep_for(POLL);
    }
    return false;
}

// The child: waits for the parent's ack, then writes `text` to `out` with C's narrow file calls.
int
child(const std::string& out, const std::string& text) {
    if (!wait_for(loupe::from_utf8(out + ACK))) {
        return 1;
    }
    const std::string part = out + ".part";
    {
        const std::unique_ptr<std::FILE, int (*)(std::FILE*)> file(std::fopen(part.c_str(), "wb"), &std::fclose);
        if (file == nullptr || std::fputs(text.c_str(), file.get()) < 0 || std::fflush(file.get()) != 0) {
            return 1;
        }
    }
    return std::rename(part.c_str(), out.c_str()) == 0 ? 0 : 1;
}

std::string
read_all(const fs::path& file) {
    std::ifstream in(file, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

} // namespace

int
main(int argc, char** argv) {
    if (argc == 4 && std::string(argv[1]) == CHILD) {
        return child(argv[2], argv[3]);
    }
    if (argc != 2) {
        std::fprintf(stderr, "usage: %s OUT_DIR\n", TEST);
        return 2;
    }
    const fs::path folder = fs::absolute(argv[1]) / loupe::from_utf8(FOLDER);
    fs::remove_all(folder);
    fs::create_directories(folder);
    const fs::path copy = folder / loupe::exe_path().filename();
    const fs::path out = folder / "out.txt";
    fs::copy_file(loupe::exe_path(), copy);
    if (!loupe::spawn({loupe::utf8(copy), CHILD, loupe::utf8(out), ARGUMENT})) {
        std::fprintf(stderr, "%s: could not start %s\n", TEST, loupe::utf8(copy).c_str());
        return 1;
    }
    const std::ofstream ack(fs::path(out).concat(ACK));
    const bool ok = wait_for(out) && read_all(out) == ARGUMENT;
    std::fprintf(stderr, "%s: the child %s\n", TEST, ok ? "wrote its argument" : "did not write its argument");
    return ok ? 0 : 1;
}
