// A scene's camera path from the command line: one frame of it, or the whole
// path drawn by two sides, A and B, laid out as an A | B | difference video.
// A is the scene given; B is the same scene's pack built at a git revision or
// in another checkout, or A's again under other renderer flags.
#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "options.hpp"

namespace loupe {

// The frames both sides draw and the time between them.
struct PathTiming {
    int frames = 0;
    uint32_t dt_ms = 0;
};

// What a run of the scene says of its path: the path's length, the run's own
// frame time, and how many of its frames one video holds.
struct PathFacts {
    uint32_t path_ms = 0;
    uint32_t own_dt_ms = 0;
    int64_t fit = 0;
};

// The path's timing: `given`'s --dt, else the run's own frame time, or the
// shortest whole millisecond at which the path fits one video when it does
// not at that; `given`'s --frames, else from the start of the path to its end.
PathTiming path_timing(const PathFacts& path, const Options& given);

// One run as -o and --video ask: opened, stepped through every frame, closed;
// false with `why` set when it failed.
bool render(const Options& options, std::string* why);

// --frame and --compare as main runs them, saying on stderr what failed:
// 0 when everything was written, 1 when a run or a tool failed, 2 when the
// command cannot run (no such revision, no such scene in it, a frame past the
// path's end).
int run_path(const Options& options);

} // namespace loupe
