#include "window/board_link.hpp"

#include <array>
#include <exception>
#include <format>
#include <fstream>
#include <utility>

#include <SDL3/SDL.h>

#include "log/log.hpp"
#include "paths.hpp"
#include "process.hpp"

namespace fs = std::filesystem;

namespace loupe::window {

namespace {

// One read of the child's stdout at most.
constexpr size_t READ_BYTES = size_t{64} * 1024;
// How long the reader waits before reading a pipe that had nothing again.
constexpr auto IDLE_WAIT = std::chrono::milliseconds(2);
// How often the destructor looks whether the child has ended.
constexpr auto END_POLL = std::chrono::milliseconds(20);

// Reads what `pipe` holds now into `buffer`; how many bytes, and false once it has closed.
bool
read_some(SDL_IOStream* pipe, std::array<uint8_t, READ_BYTES>& buffer, size_t* count) {
    *count = pipe == nullptr ? 0 : SDL_ReadIO(pipe, buffer.data(), buffer.size());
    return *count > 0 || (pipe != nullptr && SDL_GetIOStatus(pipe) == SDL_IO_STATUS_NOT_READY);
}

} // namespace

std::unique_ptr<BoardLink>
BoardLink::start(const std::vector<std::string>& command, const fs::path& log, std::string* why,
                 std::chrono::milliseconds end_within) {
    std::error_code error;
    fs::create_directories(log.parent_path(), error);
    if (!std::ofstream(log)) {
        *why = "cannot write " + utf8(log);
        return nullptr;
    }
    std::vector<char*> argv = argv_of(command);
    const SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_PROCESS_CREATE_ARGS_POINTER, static_cast<void*>(argv.data()));
    SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDIN_NUMBER, SDL_PROCESS_STDIO_APP);
    SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDOUT_NUMBER, SDL_PROCESS_STDIO_APP);
    // Read here, not redirected to the log: a file SDL opens to write is
    // not readable by anyone else on Windows, so its lines could not be
    // followed while the child runs.
    SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDERR_NUMBER, SDL_PROCESS_STDIO_APP);
#if defined(_WIN32)
    // No console window of its own on the desktop (CREATE_NO_WINDOW): Loupe
    // has no console to share. SDL then reports its exit code as 0; exit_code_ reads it.
    SDL_SetBooleanProperty(props, SDL_PROP_PROCESS_CREATE_BACKGROUND_BOOLEAN, true);
#endif
    SDL_Process* process = SDL_CreateProcessWithProperties(props);
    SDL_DestroyProperties(props);
    if (process == nullptr) {
        *why = std::string("cannot start autana record: ") + SDL_GetError();
        return nullptr;
    }
    // Made only once its child runs: the destructor waits for a child to end.
    std::unique_ptr<BoardLink> link(new BoardLink());
    link->process_ = process;
    link->log_ = log;
    link->end_within_ = end_within;
    link->exit_code_ = std::make_unique<ExitCode>(
        SDL_GetNumberProperty(SDL_GetProcessProperties(process), SDL_PROP_PROCESS_PID_NUMBER, 0));
    link->reader_ = std::jthread([raw = link.get()](const std::stop_token& stop) {
        try {
            raw->read_pipes(stop);
        } catch (const std::exception& failure) {
            // No memory for the bytes: the stream stops here.
            log::error(std::format("the board link stopped reading: {}", failure.what()));
        }
    });
    return link;
}

void
BoardLink::read_pipes(const std::stop_token& stop) {
    SDL_IOStream* out = SDL_GetProcessOutput(process_);
    SDL_IOStream* err = static_cast<SDL_IOStream*>(
        SDL_GetPointerProperty(SDL_GetProcessProperties(process_), SDL_PROP_PROCESS_STDERR_POINTER, nullptr));
    std::ofstream log(log_, std::ios::binary);
    std::array<uint8_t, READ_BYTES> buffer{};
    while ((out != nullptr || err != nullptr) && !stop.stop_requested()) {
        size_t count = 0;
        if (!read_some(out, buffer, &count)) {
            out = nullptr; // the child closed stdout
        } else if (count > 0) {
            received_ += count;
            const std::scoped_lock lock(mutex_);
            pending_.insert(pending_.end(), buffer.begin(), buffer.begin() + static_cast<std::ptrdiff_t>(count));
        }
        const bool wrote = count > 0;
        if (!read_some(err, buffer, &count)) {
            err = nullptr;
        } else if (count > 0) {
            log.write(reinterpret_cast<const char*>(buffer.data()), static_cast<std::streamsize>(count));
            log.flush();
            take_lines(std::string_view(reinterpret_cast<const char*>(buffer.data()), count));
        }
        if (!wrote && count == 0) {
            std::this_thread::sleep_for(IDLE_WAIT);
        }
    }
    if (out == nullptr && err == nullptr) {
        drained_ = true;
    }
}

void
BoardLink::take_lines(std::string_view bytes) {
    partial_ += bytes;
    size_t start = 0;
    for (size_t end = partial_.find('\n'); end != std::string::npos; end = partial_.find('\n', start)) {
        std::string line = partial_.substr(start, end - start);
        start = end + 1;
        if (line.ends_with('\r')) {
            line.pop_back();
        }
        if (!line.empty()) {
            const std::scoped_lock lock(mutex_);
            began_ = began_ || line.starts_with(RECORDING_BEGAN);
            latest_ = std::move(line);
        }
    }
    partial_.erase(0, start);
}

BoardLink::~BoardLink() {
    stop();
    const auto deadline = std::chrono::steady_clock::now() + end_within_;
    int code = 0;
    bool ended = exit_.has_value();
    while (!ended && std::chrono::steady_clock::now() < deadline) {
        ended = SDL_WaitProcess(process_, false, &code);
        std::this_thread::sleep_for(END_POLL);
    }
    if (!ended) {
        (void)SDL_KillProcess(process_, true);
    }
    reader_.request_stop();
    reader_ = {};
    SDL_DestroyProcess(process_);
}

size_t
BoardLink::take(std::vector<uint8_t>& out) {
    const std::scoped_lock lock(mutex_);
    const size_t count = pending_.size();
    out.insert(out.end(), pending_.begin(), pending_.end());
    pending_.clear();
    return count;
}

// No flush: a pipe write is not buffered, and flushing a Windows pipe waits
// until the child has read it, while the child may be waiting on its stdout.
bool
BoardLink::send(std::string_view line) {
    SDL_IOStream* in = stopped_ ? nullptr : SDL_GetProcessInput(process_);
    if (in == nullptr) {
        return false;
    }
    const std::string text = std::string(line) + "\n";
    return SDL_WriteIO(in, text.data(), text.size()) == text.size();
}

void
BoardLink::stop() {
    if (stopped_) {
        return;
    }
    stopped_ = true;
    if (SDL_IOStream* in = SDL_GetProcessInput(process_)) {
        SDL_CloseIO(in);
    }
}

BoardLink::Status
BoardLink::status() {
    int code = 0;
    if (!exit_ && SDL_WaitProcess(process_, false, &code)) {
        exit_ = exit_code_->of(code);
    }
    const bool running = !exit_ || !drained_;
    const std::scoped_lock lock(mutex_);
    return {.running = running,
            .stopped = stopped_,
            .exit = exit_,
            .last_line = running ? "" : latest_,
            .latest = latest_,
            .began = began_,
            .received = received_.load(),
            .elapsed = std::chrono::steady_clock::now() - started_,
            .log = log_};
}

} // namespace loupe::window
