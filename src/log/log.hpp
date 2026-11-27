// Loupe's log: one sink every line goes through, Loupe's own messages, the
// output of the programs it runs and what anything writes to stderr. The sink
// keeps the latest lines for the Console panel, appends each to a log file
// once one is open, and echoes Loupe's own lines and stderr's to the stderr it
// started with, so a run from a terminal still says what it did there.
#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace loupe::log {

enum class Level : uint8_t { Info, Warning, Error };

// Where Loupe's own lines come from, and the lines anything wrote to stderr.
constexpr std::string_view LOUPE = "loupe";
constexpr std::string_view STDERR = "stderr";
// How many lines the sink and the Console keep: the latest.
constexpr size_t KEPT_LINES = 10000;

// One line: when, how bad, who said it (LOUPE, STDERR, or the program Loupe
// ran), what, and its place in the sink's order, from 1.
struct Line {
    std::chrono::system_clock::time_point time;
    Level level = Level::Info;
    std::string source;
    std::string text;
    uint64_t seq = 0;
};

// The latest `capacity` lines, [0] the oldest: a line pushed past it drops the oldest.
class Ring {
  public:
    explicit Ring(size_t capacity);
    void push(Line line);
    void clear();
    size_t size() const;
    const Line& operator[](size_t i) const;

  private:
    std::vector<Line> lines_;
    size_t capacity_;
    size_t oldest_ = 0;
};

// A line's level from its words, each a whole word: one that starts with
// "error", "fatal" or "traceback", holds ": error" or a field that is an
// exception's name (ValueError:), is an error; "warning" or a warning's name
// (UserWarning:) so, a warning;
// anything else, `otherwise`. For the output of programs, which says no level.
Level level_of(std::string_view text, Level otherwise);

// Lines from anywhere, in one order, from any thread. A sink keeps the latest
// `capacity` lines and echoes Loupe's own and stderr's to the file descriptor
// `echo`, or to none when negative.
class Sink {
  public:
    explicit Sink(size_t capacity = KEPT_LINES, int echo = -1);
    // `text` as lines, split at its line ends.
    void write(Level level, std::string_view source, std::string_view text);
    // Calls `take` with each kept line after `seq`, oldest first; the last one's seq, or `seq` for none.
    uint64_t read_since(uint64_t seq, const std::function<void(const Line&)>& take) const;
    // Appends every later line to `file`, after the lines kept so far, the
    // last run's file renamed to <stem>-prev<ext> first; false when it cannot be opened.
    bool open_file(const std::filesystem::path& file);
    void close_file();
    void echo_to(int fd);
    int echo() const;

  private:
    void put(const Line& line);

    mutable std::mutex lock_;
    Ring ring_;
    uint64_t last_ = 0;
    std::ofstream file_;
    int echo_;
};

// `line` as the log file has it: UTC time to the millisecond, the level's letter, the source, the text.
std::string file_line(const Line& line);

// The sink Loupe writes to, echoing to stderr.
Sink& sink();
void info(std::string_view text);
void warn(std::string_view text);
void error(std::string_view text);

// A file another program writes, read as it grows: each whole line goes to
// the sink from `source`, its level from level_of(). A file that shrinks was
// started again, and is read from its start. poll() reads what was written
// since the last poll; with `last`, the end of a line not ended too.
class Tail {
  public:
    Tail(std::filesystem::path file, std::string source, Sink& to = sink());
    void poll(bool last = false);

  private:
    std::filesystem::path file_;
    std::string source_;
    Sink& to_;
    uintmax_t read_ = 0;
    std::string partial_;
};

// While it lives, what anything in the process writes to stderr, the
// engine's C and the libraries too, goes to `to` as lines from STDERR, and
// `to` echoes to the stderr there was before, `saved_`; `echo_` is what it
// echoed to before. A reader thread reads the pipe until `done_`.
class StderrCapture {
  public:
    explicit StderrCapture(Sink& to = sink());
    StderrCapture(const StderrCapture&) = delete;
    StderrCapture& operator=(const StderrCapture&) = delete;
    StderrCapture(StderrCapture&&) = delete;
    StderrCapture& operator=(StderrCapture&&) = delete;
    ~StderrCapture();

  private:
    Sink& to_;
    int fd_ = -1;
    int saved_ = -1;
    int echo_ = -1;
    std::thread reader_;
    std::future<void> done_;
};

// A program started from a terminal on Windows, which shows a GUI program no
// console, writes its stdout and stderr to that terminal when they are not
// redirected, and to nowhere when there is no terminal. Elsewhere it does nothing.
void attach_terminal();

} // namespace loupe::log
