// What a tick of the window left on screen, as a test reads it.
#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace loupe {

// Which screen (start, scene or live), the start panel's refusal, listed
// boards and, for each a command holds, its serial and who holds it; for a live board, where its wait stands (Waiting's names), its
// stream's frames, how long since the last one came, and Record's state,
// frames and file.
struct Observed {
    std::string screen, refusal;
    std::vector<std::string> boards, busy;
    std::string phase;
    int frames = 0;
    double frame_age_s = 0.0;
    std::string saving;
    int saved_frames = 0;
    std::filesystem::path recording;
    uint64_t recording_bytes = 0;
};

} // namespace loupe
