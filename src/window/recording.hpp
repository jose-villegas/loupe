// A board's frames as they are drawn: one `autana -b BOARD record` child, its
// stream read off stdout, console lines (FREEZE, STEP, RESUME) written to its
// stdin. Through SDL3's processes, so it lives in the window layer.
#pragma once

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <stop_token>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

struct SDL_Process;

namespace loupe::window {

// The line autana record writes once the board has started recording (its
// console's FRAMEREC_BEGIN, Console-Framing.md in autana's docs).
constexpr std::string_view RECORDING_BEGAN = "FRAMEREC_BEGIN";

// How long a recording may take to end once its input closes: autana gives
// the board's drain 30 s (RECORD_END_SECONDS in its device.py), and a margin.
constexpr std::chrono::seconds END_WITHIN{35};

// One window's recording: send, stop and status come from one thread; only
// take shares the bytes with the reader.
class Recording {
  public:
    // Starts `command` (autana_command({"-b", BOARD, "record"})), its stdout
    // copied to `save_to` as it arrives and its stderr to `save_to` + ".log".
    // `end_within` bounds the destructor's wait. Null, with `why` set, when
    // it did not start.
    static std::unique_ptr<Recording> start(const std::vector<std::string>& command,
                                            const std::filesystem::path& save_to, std::string* why,
                                            std::chrono::milliseconds end_within = END_WITHIN);
    // Stops the recording and waits for it to end, `end_within` at most; then
    // it kills the child and stops reading. On Windows that kill reaches only
    // cmd.exe, and autana goes on to end its drain by itself.
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
        // Running until the child has exited and both its pipes have closed.
        bool running = false, stopped = false;
        std::optional<int> exit; // once it has exited
        std::string last_line;   // the last line it wrote to stderr, once it is no longer running
        // While it runs: the last line it has written to stderr so far, whether
        // one of them was RECORDING_BEGAN, the stream bytes read so far, and
        // how long since it started.
        std::string latest;
        bool began = false;
        uint64_t received = 0;
        std::chrono::steady_clock::duration elapsed{};
        std::filesystem::path log;
    };

    Status status();
    const std::filesystem::path& saved() const;

  private:
    Recording() = default;
    // Copies stdout to the saved file and the pending bytes, and stderr to the
    // log and its lines, until both close or `stop` is asked.
    void read_pipes(const std::stop_token& stop);
    // Takes the whole lines in `bytes`, after what came before them, as stderr's latest.
    void take_lines(std::string_view bytes);

    SDL_Process* process_ = nullptr;
    std::filesystem::path saved_, log_;
    std::chrono::milliseconds end_within_{END_WITHIN};
    std::mutex mutex_; // guards pending_, latest_ and began_
    std::vector<uint8_t> pending_;
    std::string latest_;
    bool began_ = false;
    std::atomic<uint64_t> received_{0};
    // Both pipes closed: every line the child wrote is in latest_.
    std::atomic<bool> drained_{false};
    std::chrono::steady_clock::time_point started_ = std::chrono::steady_clock::now();
    std::string partial_; // the reader's: a stderr line not ended yet
    bool stopped_ = false;
    std::optional<int> exit_;
    std::jthread reader_;
};

} // namespace loupe::window
