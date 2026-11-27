// Loupe's log (src/log): the ring keeps the latest lines, oldest first; the
// sink splits text into lines in one order, hands the Console those after the
// last it took, echoes only Loupe's own lines and stderr's, and writes its
// file from the lines kept so far, the last run's file set aside; a level is
// read from a program's words; a tail reads a growing file by whole lines and
// starts over when the file does; what stderr is written while captured
// reaches the sink; and the Console's lines are what the sink gave it.
#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "checks.hpp"
#include "log/log.hpp"
#include "window/console.hpp"

namespace fs = std::filesystem;
using loupe::log::Level;
using loupe::log::Line;
using loupe::log::Sink;

namespace {

constexpr const char TEST[] = "log_test";
constexpr loupe::test::Checks check{TEST};

// A line of `text` alone.
Line
text_line(const std::string& text) {
    Line line;
    line.text = text;
    return line;
}

// The texts of every line `sink` keeps after `seq`.
std::vector<std::string>
texts(const Sink& sink, uint64_t seq = 0) {
    std::vector<std::string> out;
    (void)sink.read_since(seq, [&out](const Line& line) { out.push_back(line.text); });
    return out;
}

void
append(const fs::path& file, const std::string& text) {
    std::ofstream(file, std::ios::binary | std::ios::app) << text;
}

bool
ring_keeps_the_latest() {
    constexpr size_t CAPACITY = 3;
    loupe::log::Ring ring(CAPACITY);
    for (const char* text : {"a", "b", "c", "d", "e"}) {
        ring.push(text_line(text));
    }
    bool ok = check(ring.size() == CAPACITY && ring[0].text == "c" && ring[1].text == "d" && ring[2].text == "e",
                    "the ring keeps the latest three, oldest first");
    ring.clear();
    ring.push(text_line("f"));
    ok &= check(ring.size() == 1 && ring[0].text == "f", "a cleared ring starts again");
    return ok;
}

bool
sink_orders_and_bounds() {
    constexpr size_t CAPACITY = 4;
    Sink sink(CAPACITY);
    sink.write(Level::Info, "tool", "one\ntwo\r\nthree\n");
    bool ok = check(texts(sink) == std::vector<std::string>{"one", "two", "three"}, "text split at its line ends");
    const uint64_t seen = sink.read_since(0, [](const Line&) {});
    sink.write(Level::Error, "tool", "four");
    sink.write(Level::Warning, "tool", "five");
    ok &= check(texts(sink, seen) == std::vector<std::string>{"four", "five"}, "only the lines after the last taken");
    ok &= check(texts(sink) == std::vector<std::string>{"two", "three", "four", "five"}, "the oldest dropped first");
    std::vector<uint64_t> seqs;
    (void)sink.read_since(0, [&seqs](const Line& line) { seqs.push_back(line.seq); });
    ok &= check(seqs == std::vector<uint64_t>{2, 3, 4, 5}, "each line numbered in order");
    return ok;
}

bool
levels_from_words() {
    return check(loupe::log::level_of("Error: no such file", Level::Info) == Level::Error
                     && loupe::log::level_of("  Traceback (most recent call last):", Level::Info) == Level::Error
                     && loupe::log::level_of("scene.toml:3: error: bad key", Level::Info) == Level::Error
                     && loupe::log::level_of("ModuleNotFoundError: No module named 'x'", Level::Info) == Level::Error
                     && loupe::log::level_of("no Error: here", Level::Info) == Level::Info
                     && loupe::log::level_of("WARNING: no bake", Level::Info) == Level::Warning
                     && loupe::log::level_of("x.cpp:1: warning: unused", Level::Info) == Level::Warning
                     && loupe::log::level_of("built 3 meshes", Level::Info) == Level::Info
                     && loupe::log::level_of("built 3 meshes", Level::Warning) == Level::Warning
                     && loupe::log::level_of("no errors", Level::Info) == Level::Info,
                 "levels read from a program's words");
}

bool
echoes_loupe_and_stderr(const fs::path& out) {
    const fs::path echoed = out / "echo.txt";
    bool ok = true;
    {
        const std::unique_ptr<std::FILE, int (*)(std::FILE*)> file(std::fopen(echoed.string().c_str(), "wb"),
                                                                   &std::fclose);
        Sink sink(loupe::log::KEPT_LINES, fileno(file.get()));
        sink.write(Level::Error, loupe::log::LOUPE, "mine");
        sink.write(Level::Warning, loupe::log::STDERR, "the engine's");
        sink.write(Level::Info, "build_pack", "a program's");
        ok &= check(sink.echo() == fileno(file.get()), "echoing to the descriptor given");
    }
    return check(loupe::test::read_file(echoed) == "loupe: mine\nthe engine's\n",
                 "Loupe's lines echoed as loupe: ..., stderr's as they were, a program's not");
}

bool
file_from_the_kept_lines(const fs::path& out) {
    const fs::path file = out / "logs" / "loupe.log";
    fs::create_directories(file.parent_path());
    loupe::test::write_file(file, "the last run\n");
    Sink sink;
    sink.write(Level::Warning, loupe::log::LOUPE, "before");
    bool ok = check(sink.open_file(file), "the log file opens");
    sink.write(Level::Error, "git", "after");
    sink.close_file();
    const std::vector<std::string> lines = loupe::test::lines(loupe::test::read_file(file));
    ok &= check(lines.size() == 2 && lines[0].ends_with(" W [loupe] before") && lines[1].ends_with(" E [git] after"),
                "the file holds the lines kept before it opened, then the later ones");
    // 2026-11-27T12:34:56.789Z: the date, T, the time to the millisecond, Z.
    constexpr size_t STAMP = 24;
    ok &= check(!lines.empty() && lines[0].size() > STAMP && lines[0][10] == 'T' && lines[0][STAMP - 1] == 'Z',
                "each line stamped in UTC to the millisecond");
    ok &= check(loupe::test::read_file(out / "logs" / "loupe-prev.log") == "the last run\n",
                "the last run's file set aside");
    return ok;
}

bool
tail_reads_whole_lines(const fs::path& out) {
    const fs::path file = out / "tool.log";
    Sink sink;
    loupe::log::Tail tail(file, "tool", sink);
    tail.poll();
    bool ok = check(texts(sink).empty(), "no file, no lines");
    append(file, "first\nsec");
    tail.poll();
    ok &= check(texts(sink) == std::vector<std::string>{"first"}, "a line not ended waits");
    append(file, "ond\nerror: third\n");
    tail.poll();
    std::vector<Level> levels;
    (void)sink.read_since(0, [&levels](const Line& line) { levels.push_back(line.level); });
    ok &= check(texts(sink) == std::vector<std::string>{"first", "second", "error: third"}
                    && levels == std::vector<Level>{Level::Info, Level::Info, Level::Error},
                "the rest as it comes, each line's level from its words");
    loupe::test::write_file(file, "again");
    tail.poll(true);
    ok &= check(texts(sink).back() == "again", "a file written anew read from its start, its last line unended");
    return ok;
}

bool
stderr_reaches_the_sink() {
    Sink sink;
    const int before = sink.echo();
    {
        const loupe::log::StderrCapture captured(sink);
        std::fprintf(stderr, "from C's stderr\npartial");
        std::fflush(stderr);
    }
    std::vector<std::string> sources;
    (void)sink.read_since(0, [&sources](const Line& line) { sources.push_back(line.source); });
    return check(texts(sink) == std::vector<std::string>{"from C's stderr", "partial"}
                     && sources == std::vector<std::string>(2, std::string(loupe::log::STDERR))
                     && sink.echo() == before,
                 "stderr written while captured reaches the sink as stderr's lines, the echo put back after");
}

bool
console_shows_the_sinks_lines() {
    Sink sink;
    loupe::window::Console console;
    sink.write(Level::Info, loupe::log::LOUPE, "one");
    loupe::window::pull(console, sink);
    console.lines.clear();
    sink.write(Level::Warning, "bake_list", "two");
    loupe::window::pull(console, sink);
    bool ok = check(console.lines.size() == 1 && console.lines[0].text == "two",
                    "a cleared Console takes only the lines after it");
    ok &= check(loupe::window::shown_line(console.lines[0], false) == "warning [bake_list] two",
                "a line shown with its level and source");
    const std::string timed = loupe::window::shown_line(console.lines[0], true);
    // 12:34:56.789 and a space before the level.
    constexpr size_t CLOCK = 13;
    ok &= check(timed.size() > CLOCK && timed[2] == ':' && timed[8] == '.'
                    && timed.substr(CLOCK) == "warning [bake_list] two",
                "and with its time of day, to the millisecond");
    return ok;
}

} // namespace

int
main(int argc, char** argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: %s OUT_DIR\n", TEST);
        return 2;
    }
    const fs::path out = fs::absolute(argv[1]);
    fs::remove_all(out);
    fs::create_directories(out);
    bool ok = ring_keeps_the_latest();
    ok &= sink_orders_and_bounds();
    ok &= levels_from_words();
    ok &= echoes_loupe_and_stderr(out);
    ok &= file_from_the_kept_lines(out);
    ok &= tail_reads_whole_lines(out);
    ok &= stderr_reaches_the_sink();
    ok &= console_shows_the_sinks_lines();
    return ok ? 0 : 1;
}
