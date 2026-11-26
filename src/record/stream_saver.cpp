#include "record/stream_saver.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <span>
#include <string>
#include <system_error>

#include "engine/autana.hpp"
#include "paths.hpp"

namespace fs = std::filesystem;

namespace loupe {

namespace {

constexpr size_t HEAD = sizeof(frame_record_head_t);
// A FRAME is routed by its flags: its head and its body up to them.
constexpr size_t FRAME_ROUTED = HEAD + offsetof(frame_record_frame_t, flags) + 1;

struct Head {
    uint16_t type;
    uint32_t length;
};

Head
head_of(const std::vector<uint8_t>& bytes) {
    frame_record_head_t head{};
    std::memcpy(&head, bytes.data(), HEAD);
    return {.type = head.type, .length = head.length};
}

// How many of a record's first bytes route() needs, once its head is known.
size_t
routed_by(const Head& head) {
    return head.type == FRAME_RECORD_FRAME ? std::min(FRAME_ROUTED, HEAD + head.length) : HEAD;
}

} // namespace

StreamSaver::State
StreamSaver::state() const {
    return state_;
}

int
StreamSaver::frames() const {
    return frames_;
}

const fs::path&
StreamSaver::file() const {
    return file_;
}

void
StreamSaver::feed(std::span<const uint8_t> bytes) {
    while (!bytes.empty()) {
        if (remaining_ > 0) {
            const size_t count = std::min(remaining_, bytes.size());
            if (keep_) {
                write(bytes.first(count));
            }
            remaining_ -= count;
            bytes = bytes.subspan(count);
            continue;
        }
        const size_t need = start_.size() < HEAD ? HEAD : routed_by(head_of(start_));
        const size_t count = std::min(need - start_.size(), bytes.size());
        start_.insert(start_.end(), bytes.begin(), bytes.begin() + static_cast<std::ptrdiff_t>(count));
        bytes = bytes.subspan(count);
        if (start_.size() >= HEAD && start_.size() == routed_by(head_of(start_))) {
            route();
        }
    }
}

void
StreamSaver::route() {
    const Head head = head_of(start_);
    if (head.type == FRAME_RECORD_FRAME) {
        opened_ = true;
        if (state_ == State::Saving && stopping_) {
            out_.close();
            state_ = State::Off;
            stopping_ = false;
        }
        const bool keyframe = start_.size() == FRAME_ROUTED && (start_.back() & FRAME_RECORD_KEYFRAME) != 0;
        if (state_ == State::Waiting && keyframe) {
            out_.write(reinterpret_cast<const char*>(opening_.data()), static_cast<std::streamsize>(opening_.size()));
            state_ = State::Saving;
        }
        frames_ += state_ == State::Saving ? 1 : 0;
    }
    keep_ = !opened_ || state_ == State::Saving;
    if (keep_) {
        write(start_);
    }
    remaining_ = HEAD + head.length - start_.size();
    start_.clear();
}

void
StreamSaver::write(std::span<const uint8_t> bytes) {
    if (!opened_) {
        opening_.insert(opening_.end(), bytes.begin(), bytes.end());
    } else if (state_ == State::Saving) {
        out_.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    }
}

bool
StreamSaver::start(const fs::path& file, std::string* why) {
    if (state_ != State::Off) {
        *why = "already saving to " + utf8(file_);
        return false;
    }
    std::error_code error;
    fs::create_directories(file.parent_path(), error);
    out_.open(file, std::ios::binary | std::ios::trunc);
    if (!out_) {
        *why = "cannot write " + utf8(file);
        return false;
    }
    file_ = file;
    frames_ = 0;
    state_ = State::Waiting;
    return true;
}

void
StreamSaver::stop() {
    if (state_ == State::Waiting) {
        // No keyframe came: no frame to keep, and no file.
        out_.close();
        std::error_code error;
        fs::remove(file_, error);
        state_ = State::Off;
    } else if (state_ == State::Saving) {
        stopping_ = true;
    }
}

} // namespace loupe
