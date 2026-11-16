#include "window/recording.hpp"

#include <array>
#include <chrono>
#include <fstream>
#include <utility>

#include <SDL3/SDL.h>
#include <nlohmann/json.hpp>

#include "paths.hpp"
#include "process.hpp"

namespace fs = std::filesystem;

namespace loupe::window {

namespace {

// One read of the child's stdout at most.
constexpr size_t READ_BYTES = size_t{64} * 1024;
// How long the reader waits before reading a pipe that had nothing again.
constexpr auto IDLE_WAIT = std::chrono::milliseconds(2);
// The pool's word for a board no one holds.
constexpr std::string_view FREE_STATE = "unlocked";

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

std::vector<std::string>
autana_command(const std::vector<std::string>& args) {
#if defined(_WIN32)
    std::vector<std::string> command = {utf8(system_program("cmd")), "/d", "/c", "autana"};
#else
    std::vector<std::string> command = {"autana"};
#endif
    command.insert(command.end(), args.begin(), args.end());
    return command;
}

std::vector<Board>
parse_boards(const std::string& json, std::string* why) {
    const nlohmann::json parsed = nlohmann::json::parse(json, nullptr, false);
    if (parsed.is_discarded() || !parsed.contains("boards") || !parsed["boards"].is_array()) {
        *why = "autana devices --json gave no boards: " + json;
        return {};
    }
    std::vector<Board> boards;
    try {
        for (const nlohmann::json& item : parsed["boards"]) {
            const nlohmann::json& port = item.at("port");
            boards.push_back({.id = item.at("board").get<std::string>(),
                              .port = port.is_null() ? std::string() : port.get<std::string>(),
                              .free = item.at("state").get<std::string>() == FREE_STATE});
        }
    } catch (const nlohmann::json::exception& error) {
        *why = std::string("autana devices --json gave a board without its fields: ") + error.what();
        return {};
    }
    return boards;
}

std::vector<Board>
list_boards(std::string* why) {
    const std::vector<std::string> command = autana_command({"devices", "--json"});
    const std::vector<char*> argv = argv_of(command);
    SDL_Process* process = SDL_CreateProcess(argv.data(), true);
    if (process == nullptr) {
        *why = std::string("cannot run autana devices: ") + SDL_GetError();
        return {};
    }
    size_t size = 0;
    int exit_code = 0;
    char* output = static_cast<char*>(SDL_ReadProcess(process, &size, &exit_code));
    SDL_DestroyProcess(process);
    const std::string text = output != nullptr ? std::string(output, size) : std::string();
    SDL_free(output);
    if (exit_code != 0) {
        *why = "autana devices failed: " + text;
        return {};
    }
    return parse_boards(text, why);
}

std::unique_ptr<Recording>
Recording::start(const std::vector<std::string>& command, const fs::path& save_to, std::string* why) {
    std::error_code error;
    fs::create_directories(save_to.parent_path(), error);
    std::unique_ptr<Recording> recording(new Recording());
    recording->saved_ = save_to;
    recording->log_ = fs::path(save_to).concat(".log");
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
    recording->reader_ = std::jthread([raw = recording.get()] { raw->read_stdout(); });
    return recording;
}

void
Recording::read_stdout() {
    SDL_IOStream* out = SDL_GetProcessOutput(process_);
    std::ofstream save(saved_, std::ios::binary);
    std::array<uint8_t, READ_BYTES> buffer{};
    while (out != nullptr) {
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
    if (reader_.joinable()) {
        reader_.join();
    }
    if (process_ != nullptr) {
        int code = 0;
        (void)SDL_WaitProcess(process_, true, &code);
        SDL_DestroyProcess(process_);
    }
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
    if (stopped_ || process_ == nullptr) {
        return;
    }
    stopped_ = true;
    if (SDL_IOStream* in = SDL_GetProcessInput(process_)) {
        SDL_CloseIO(in);
    }
}

Recording::Status
Recording::status() {
    if (!exit_) {
        int code = 0;
        if (SDL_WaitProcess(process_, false, &code)) {
            exit_ = code;
        }
    }
    return {.running = !exit_, .stopped = stopped_, .exit = exit_, .last_line = last_line(log_)};
}

const fs::path&
Recording::saved() const {
    return saved_;
}

} // namespace loupe::window
