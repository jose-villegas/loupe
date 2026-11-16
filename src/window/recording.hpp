// A board's frames as they are drawn: one `autana -b BOARD record` child, its
// stream read off stdout, console lines (FREEZE, STEP, RESUME) written to its
// stdin. Through SDL3's processes, so it lives in the window layer.
#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

struct SDL_Process;

namespace loupe::window {

// One board the board pool knows: its serial, its port when plugged in here,
// and whether no one holds it.
struct Board {
    std::string id, port;
    bool free = false;
};

// The command line that runs autana with `args`: through cmd.exe on Windows,
// where autana is a .cmd; autana from PATH elsewhere.
std::vector<std::string> autana_command(const std::vector<std::string>& args);
// The boards in `autana devices --json`'s text; none, with `why` set, when it does not parse.
std::vector<Board> parse_boards(const std::string& json, std::string* why);
// The pool's boards, from `autana devices --json`; none, with `why` set, when it did not run.
std::vector<Board> list_boards(std::string* why);

// One window's recording: send, stop and status come from one thread; only
// take shares the bytes with the reader.
class Recording {
  public:
    // Starts `command` (autana_command({"-b", BOARD, "record"})), its stdout
    // copied to `save_to` as it arrives and its stderr to `save_to` + ".log".
    // Null, with `why` set, when it did not start.
    static std::unique_ptr<Recording> start(const std::vector<std::string>& command,
                                            const std::filesystem::path& save_to, std::string* why);
    // Stops the recording and waits for it to end: the board drains its ring
    // first, so this can take as long as FRAMEREC STOP does.
    ~Recording();
    Recording(const Recording&) = delete;
    Recording& operator=(const Recording&) = delete;
    Recording(Recording&&) = delete;
    Recording& operator=(Recording&&) = delete;

    // Appends the stream bytes read since the last take to `out`; how many.
    size_t take(std::vector<uint8_t>& out);
    // One console line to the board; false once stdin is closed or the child is gone.
    bool send(std::string_view line);
    // Closes stdin: autana sends FRAMEREC STOP, drains the board and exits.
    void stop();

    struct Status {
        bool running = false, stopped = false;
        std::optional<int> exit; // once it has exited
        std::string last_line;   // the last line it wrote to stderr
    };

    Status status();
    const std::filesystem::path& saved() const;

  private:
    Recording() = default;
    // Copies stdout to the file and the pending bytes until it ends.
    void read_stdout();

    SDL_Process* process_ = nullptr;
    std::filesystem::path saved_, log_;
    std::mutex mutex_; // guards pending_ alone
    std::vector<uint8_t> pending_;
    bool stopped_ = false;
    std::optional<int> exit_;
    std::jthread reader_;
};

} // namespace loupe::window
