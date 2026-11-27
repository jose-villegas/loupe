#include "log/log.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <format>
#include <system_error>
#include <utility>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
// After windows.h, as the C runtime's own headers are.
#include <fcntl.h>
#include <io.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

namespace fs = std::filesystem;

namespace loupe::log {

namespace {

#if defined(_WIN32)
int
dup_fd(int fd) {
    return _dup(fd);
}

int
dup2_fd(int from, int to) {
    return _dup2(from, to);
}

int
close_fd(int fd) {
    return _close(fd);
}

long long
read_fd(int fd, char* bytes, size_t size) {
    return _read(fd, bytes, static_cast<unsigned>(size));
}

void
write_fd(int fd, std::string_view bytes) {
    (void)_write(fd, bytes.data(), static_cast<unsigned>(bytes.size()));
}

// A pipe no program Loupe starts inherits, so its end closes when Loupe's does.
bool
open_pipe(std::array<int, 2>& ends) {
    constexpr unsigned PIPE_BYTES = 4096;
    return _pipe(ends.data(), PIPE_BYTES, _O_BINARY | _O_NOINHERIT) == 0;
}

// stderr as dup2() left it is inherited; Loupe's programs get their own.
void
keep_from_programs(int fd) {
    SetHandleInformation(reinterpret_cast<HANDLE>(_get_osfhandle(fd)), HANDLE_FLAG_INHERIT, 0);
}
#else
int
dup_fd(int fd) {
    return dup(fd);
}

int
dup2_fd(int from, int to) {
    return dup2(from, to);
}

int
close_fd(int fd) {
    return close(fd);
}

long long
read_fd(int fd, char* bytes, size_t size) {
    return read(fd, bytes, size);
}

void
write_fd(int fd, std::string_view bytes) {
    while (!bytes.empty()) {
        const ssize_t wrote = write(fd, bytes.data(), bytes.size());
        if (wrote <= 0) {
            return;
        }
        bytes.remove_prefix(static_cast<size_t>(wrote));
    }
}

bool
open_pipe(std::array<int, 2>& ends) {
    return pipe(ends.data()) == 0 && fcntl(ends[0], F_SETFD, FD_CLOEXEC) == 0;
}

void
keep_from_programs(int /*fd*/) {}
#endif

bool
starts_with_word(std::string_view text, std::string_view word) {
    return text.size() >= word.size() && std::ranges::equal(text.substr(0, word.size()), word, [](char a, char b) {
               return std::tolower(static_cast<unsigned char>(a)) == b;
           });
}

bool
holds(std::string_view text, std::string_view words) {
    std::string lower(text);
    std::ranges::transform(lower, lower.begin(), [](char c) { return std::tolower(static_cast<unsigned char>(c)); });
    return lower.find(words) != std::string::npos;
}

char
letter(Level level) {
    return level == Level::Error ? 'E' : level == Level::Warning ? 'W' : 'I';
}

// Splits `bytes` after `partial` into whole lines for `take`, keeping the rest in `partial`.
template <typename Take>
void
split_lines(std::string& partial, std::string_view bytes, Take take) {
    partial += bytes;
    size_t start = 0;
    for (size_t end = partial.find('\n'); end != std::string::npos; end = partial.find('\n', start)) {
        std::string_view line(partial.data() + start, end - start);
        if (line.ends_with('\r')) {
            line.remove_suffix(1);
        }
        take(line);
        start = end + 1;
    }
    partial.erase(0, start);
}

} // namespace

Ring::Ring(size_t capacity) : capacity_(std::max<size_t>(capacity, 1)) { lines_.reserve(capacity_); }

void
Ring::push(Line line) {
    if (lines_.size() < capacity_) {
        lines_.push_back(std::move(line));
        return;
    }
    lines_[oldest_] = std::move(line);
    oldest_ = (oldest_ + 1) % capacity_;
}

void
Ring::clear() {
    lines_.clear();
    oldest_ = 0;
}

size_t
Ring::size() const {
    return lines_.size();
}

const Line&
Ring::operator[](size_t i) const {
    return lines_[(oldest_ + i) % lines_.size()];
}

Level
level_of(std::string_view text, Level otherwise) {
    const size_t first = text.find_first_not_of(" \t");
    const std::string_view words = first == std::string_view::npos ? std::string_view() : text.substr(first);
    const std::string_view head = words.substr(0, words.find(':'));
    const bool raised = words.find(':') != std::string_view::npos && head.find(' ') == std::string_view::npos
                        && (head.ends_with("Error") || head.ends_with("Exception"));
    if (raised || starts_with_word(words, "error") || starts_with_word(words, "fatal")
        || starts_with_word(words, "traceback") || holds(words, ": error")) {
        return Level::Error;
    }
    if (starts_with_word(words, "warning") || holds(words, ": warning")) {
        return Level::Warning;
    }
    return otherwise;
}

std::string
file_line(const Line& line) {
    return std::format("{:%FT%TZ} {} [{}] {}", std::chrono::floor<std::chrono::milliseconds>(line.time),
                       letter(line.level), line.source, line.text);
}

Sink::Sink(size_t capacity, int echo) : ring_(capacity), echo_(echo) {}

void
Sink::write(Level level, std::string_view source, std::string_view text) {
    const auto now = std::chrono::system_clock::now();
    std::string rest(text);
    if (!rest.ends_with('\n')) {
        rest += '\n';
    }
    const std::scoped_lock hold(lock_);
    std::string partial;
    split_lines(partial, rest, [&](std::string_view part) {
        put(Line{.time = now, .level = level, .source = std::string(source), .text = std::string(part), .seq = 0});
    });
}

void
Sink::put(const Line& line) {
    Line kept = line;
    kept.seq = ++last_;
    if (file_.is_open()) {
        file_ << file_line(kept) << '\n';
        file_.flush();
    }
    if (echo_ >= 0 && (kept.source == LOUPE || kept.source == STDERR)) {
        write_fd(echo_, kept.source == LOUPE ? std::format("loupe: {}\n", kept.text) : kept.text + '\n');
    }
    ring_.push(std::move(kept));
}

uint64_t
Sink::read_since(uint64_t seq, const std::function<void(const Line&)>& take) const {
    const std::scoped_lock hold(lock_);
    for (size_t i = 0; i < ring_.size(); i++) {
        if (ring_[i].seq > seq) {
            take(ring_[i]);
            seq = ring_[i].seq;
        }
    }
    return seq;
}

bool
Sink::open_file(const fs::path& file) {
    const std::scoped_lock hold(lock_);
    file_.close();
    std::error_code error;
    fs::create_directories(file.parent_path(), error);
    fs::path previous = file.parent_path() / file.stem();
    previous += "-prev";
    previous += file.extension();
    fs::rename(file, previous, error);
    file_.open(file, std::ios::binary | std::ios::trunc);
    for (size_t i = 0; file_.is_open() && i < ring_.size(); i++) {
        file_ << file_line(ring_[i]) << '\n';
    }
    file_.flush();
    return file_.is_open() && file_.good();
}

void
Sink::close_file() {
    const std::scoped_lock hold(lock_);
    file_.close();
}

void
Sink::echo_to(int fd) {
    const std::scoped_lock hold(lock_);
    echo_ = fd;
}

int
Sink::echo() const {
    const std::scoped_lock hold(lock_);
    return echo_;
}

Sink&
sink() {
    static Sink loupe(KEPT_LINES, fileno(stderr));
    return loupe;
}

void
info(std::string_view text) {
    sink().write(Level::Info, LOUPE, text);
}

void
warn(std::string_view text) {
    sink().write(Level::Warning, LOUPE, text);
}

void
error(std::string_view text) {
    sink().write(Level::Error, LOUPE, text);
}

Tail::Tail(fs::path file, std::string source, Sink& to) : file_(std::move(file)), source_(std::move(source)), to_(to) {}

void
Tail::poll(bool last) {
    std::error_code error;
    const uintmax_t size = fs::file_size(file_, error);
    if (error) {
        return;
    }
    if (size < read_) {
        read_ = 0;
        partial_.clear();
    }
    std::string bytes;
    if (size > read_) {
        std::ifstream in(file_, std::ios::binary);
        in.seekg(static_cast<std::streamoff>(read_));
        bytes.resize(static_cast<size_t>(size - read_));
        in.read(bytes.data(), static_cast<std::streamsize>(bytes.size()));
        bytes.resize(static_cast<size_t>(in.gcount()));
        read_ += bytes.size();
    }
    split_lines(partial_, bytes,
                [this](std::string_view line) { to_.write(level_of(line, Level::Info), source_, line); });
    if (last && !partial_.empty()) {
        to_.write(level_of(partial_, Level::Info), source_, partial_);
        partial_.clear();
    }
}

StderrCapture::StderrCapture(Sink& to) : to_(to), fd_(fileno(stderr)) {
    std::array<int, 2> ends = {-1, -1};
    if (fd_ < 0 || !open_pipe(ends)) {
        return;
    }
    std::fflush(stderr);
    saved_ = dup_fd(fd_);
    if (saved_ < 0 || dup2_fd(ends[1], fd_) < 0) {
        close_fd(ends[0]);
        close_fd(ends[1]);
        if (saved_ >= 0) {
            close_fd(saved_);
        }
        saved_ = -1;
        return;
    }
    close_fd(ends[1]);
    keep_from_programs(fd_);
    read_ = ends[0];
    echo_ = to_.echo();
    to_.echo_to(saved_);
    reader_ = std::thread([this] {
        constexpr size_t CHUNK = 4096;
        std::array<char, CHUNK> bytes{};
        std::string partial;
        for (long long got = 0; (got = read_fd(read_, bytes.data(), bytes.size())) > 0;) {
            split_lines(partial, std::string_view(bytes.data(), static_cast<size_t>(got)),
                        [this](std::string_view line) { to_.write(level_of(line, Level::Warning), STDERR, line); });
        }
        if (!partial.empty()) {
            to_.write(level_of(partial, Level::Warning), STDERR, partial);
        }
    });
}

// stderr put back closes the pipe's last writing end, so the reader reads to its end and returns.
StderrCapture::~StderrCapture() {
    if (saved_ < 0) {
        return;
    }
    std::fflush(stderr);
    dup2_fd(saved_, fd_);
    reader_.join();
    to_.echo_to(echo_);
    close_fd(read_);
    close_fd(saved_);
}

void
attach_terminal() {
#if defined(_WIN32)
    const auto redirected = [](DWORD which) {
        const HANDLE handle = GetStdHandle(which);
        return handle != nullptr && handle != INVALID_HANDLE_VALUE;
    };
    const bool out = redirected(STD_OUTPUT_HANDLE);
    const bool err = redirected(STD_ERROR_HANDLE);
    if (out && err) {
        return;
    }
    const char* to = AttachConsole(ATTACH_PARENT_PROCESS) ? "CONOUT$" : "NUL";
    if (!out) {
        (void)std::freopen(to, "w", stdout);
    }
    if (!err) {
        (void)std::freopen(to, "w", stderr);
        std::setvbuf(stderr, nullptr, _IONBF, 0);
    }
    sink().echo_to(fileno(stderr));
#endif
}

} // namespace loupe::log
