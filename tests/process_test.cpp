// A tool run_logged() starts gets its arguments intact, and its output lands in
// the log: this test copies itself into a folder named outside any one Windows
// code page, runs the copy with an argument holding a space and a quote, and
// the copy writes it with C's fopen, as autana opens its packs, beside itself,
// then says so on stdout.
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "paths.hpp"
#include "process.hpp"

namespace fs = std::filesystem;

namespace {

constexpr const char TEST[] = "process_test";
constexpr const char CHILD[] = "--child";
constexpr const char ARGUMENT[] = "a \"quoted\" word";
constexpr const char SAID[] = "the child wrote its argument";
// "é 漢", spelled in UTF-8 bytes: no one Windows code page holds both.
constexpr const char FOLDER[] = "\xC3\xA9 \xE6\xBC\xA2";

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

std::string
read_all(const fs::path& file) {
    const std::ifstream in(file, std::ios::binary);
    std::ostringstream text;
    text << in.rdbuf();
    return text.str();
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
    const fs::path log = folder / "child.log";
    fs::copy_file(loupe::exe_path(), copy);
    const bool ran = loupe::run_logged({loupe::utf8(copy), CHILD, loupe::utf8(out), ARGUMENT}, log);
    const bool wrote = ran && read_all(out) == ARGUMENT;
    const bool logged = read_all(log).starts_with(SAID);
    std::fprintf(stderr, "%s: the child %s, its output %s\n", TEST, wrote ? "wrote its argument" : "DID NOT write it",
                 logged ? "in the log" : "NOT in the log");
    return wrote && logged ? 0 : 1;
}
