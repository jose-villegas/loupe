// Saves part of a board's live stream as a .framerec that opens on its own:
// the stream's opening records (HEADER, its PACKs), then whole frames from the
// first keyframe after start() until stop(). A keyframe holds everything a
// redraw needs (frame_record_format.h in autana), so the file needs nothing
// the stream sent before it.
#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <span>
#include <string>
#include <vector>

namespace loupe {

class StreamSaver {
  public:
    enum class State : uint8_t { Off, Waiting, Saving };

    // Every byte of the live stream, in order, from its first.
    void feed(std::span<const uint8_t> bytes);
    // Saves to `file` from the next keyframe on; false, with `why` set, when
    // it cannot be written or a save is already under way.
    bool start(const std::filesystem::path& file, std::string* why);
    // Ends the file before the next frame, or at once while still waiting.
    void stop();

    State state() const;
    // The frames written to the file so far.
    int frames() const;
    const std::filesystem::path& file() const;

  private:
    // Where the record now starting goes, once its head (and a FRAME's flags) are read.
    void route();
    void write(std::span<const uint8_t> bytes);

    State state_ = State::Off;
    bool stopping_ = false;
    // The records before the first FRAME, once whole.
    std::vector<uint8_t> opening_;
    bool opened_ = false;
    // The start of the record being read, until route() decides where it goes;
    // then how many of its bytes are still to come, and whether they are kept.
    std::vector<uint8_t> start_;
    size_t remaining_ = 0;
    bool keep_ = false;
    std::filesystem::path file_;
    std::ofstream out_;
    int frames_ = 0;
};

} // namespace loupe
