#include "window/recording.hpp"

#include <array>
#include <cstdio>
#include <exception>
#include <fstream>
#include <utility>

#include <SDL3/SDL.h>

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

// The last non-empty line of `file`; empty when it has none.
std::string
last_line(const fs::path& file) {
    const std::vector<std::string> lines = read_lines(file);
    for (auto it = lines.rbegin(); it != lines.rend(); ++it) {
        if (!it->empty()) {
            return *it;
        }
    }
    return {};
}

} // namespace

std::unique_ptr<Recording>
Recording::start(const std::vector<std::string>& command, const fs::path& save_to, std::string* why,
                 std::chrono::milliseconds end_within) {
    std::error_code error;
    fs::create_directories(save_to.parent_path(), error);
    std::unique_ptr<Recording> recording(new Recording());
    recording->saved_ = save_to;
    recording->log_ = fs::path(save_to).concat(".log");
    recording->end_within_ = end_within;
    SDL_IOStream* log = SDL_IOFromFile(utf8(recording->log_).c_str(), "w");
    if (log == nullptr) {
        *why = "cannot write " + utf8(recording->log_) + ": " + SDL_GetError();
        return nullptr;
    }
    std::vector<char*> argv = argv_of(command);
    const SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_PROCESS_CREATE_ARGS_POINTER, static_cast<void*>(argv.data()));
    SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDIN_NUMBER, SDL_PROCESS_STDIO_APP);
    SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDOUT_NUMBER, SDL_PROCESS_STDIO_APP);
    SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDERR_NUMBER, SDL_PROCESS_STDIO_REDIRECT);
    SDL_SetPointerProperty(props, SDL_PROP_PROCESS_CREATE_STDERR_POINTER, log);
    recording->process_ = SDL_CreateProcessWithProperties(props);
    SDL_DestroyProperties(props);
    // The child holds its own handle to the log now.
    SDL_CloseIO(log);
    if (recording->process_ == nullptr) {
        *why = std::string("cannot start autana record: ") + SDL_GetError();
        return nullptr;
    }
    recording->reader_ = std::jthread([raw = recording.get()](const std::stop_token& stop) {
        try {
            raw->read_stdout(stop);
        } catch (const std::exception& failure) {
            // No memory for the bytes: the stream stops here, and the saved file holds what came.
            std::fprintf(stderr, "loupe: recording stopped reading: %s\n", failure.what());
        }
    });
    return recording;
}

void
Recording::read_stdout(const std::stop_token& stop) {
    SDL_IOStream* out = SDL_GetProcessOutput(process_);
    std::ofstream save(saved_, std::ios::binary);
    std::array<uint8_t, READ_BYTES> buffer{};
    while (out != nullptr && !stop.stop_requested()) {
        const size_t count = SDL_ReadIO(out, buffer.data(), buffer.size());
        if (count == 0) {
            if (SDL_GetIOStatus(out) != SDL_IO_STATUS_NOT_READY) {
                break; // the child closed stdout: it has ended
            }
            std::this_thread::sleep_for(IDLE_WAIT);
            continue;
        }
        save.write(reinterpret_cast<const char*>(buffer.data()), static_cast<std::streamsize>(count));
        save.flush();
        const std::scoped_lock lock(mutex_);
        pending_.insert(pending_.end(), buffer.begin(), buffer.begin() + static_cast<std::ptrdiff_t>(count));
    }
}

Recording::~Recording() {
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
Recording::take(std::vector<uint8_t>& out) {
    const std::scoped_lock lock(mutex_);
    const size_t count = pending_.size();
    out.insert(out.end(), pending_.begin(), pending_.end());
    pending_.clear();
    return count;
}

// No flush: a pipe write is not buffered, and flushing a Windows pipe waits
// until the child has read it, while the child may be waiting on its stdout.
bool
Recording::send(std::string_view line) {
    SDL_IOStream* in = stopped_ ? nullptr : SDL_GetProcessInput(process_);
    if (in == nullptr) {
        return false;
    }
    const std::string text = std::string(line) + "\n";
    return SDL_WriteIO(in, text.data(), text.size()) == text.size();
}

void
Recording::stop() {
    if (stopped_) {
        return;
    }
    stopped_ = true;
    if (SDL_IOStream* in = SDL_GetProcessInput(process_)) {
        SDL_CloseIO(in);
    }
}

// The log is read once, when the child has ended: the panel shows its last line then.
Recording::Status
Recording::status() {
    int code = 0;
    if (!exit_ && SDL_WaitProcess(process_, false, &code)) {
        exit_ = code;
        last_line_ = last_line(log_);
    }
    return {.running = !exit_, .stopped = stopped_, .exit = exit_, .last_line = last_line_};
}

const fs::path&
Recording::saved() const {
    return saved_;
}

} // namespace loupe::window
