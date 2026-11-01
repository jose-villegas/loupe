// The window over an open run: the frame, a timeline and a debug-view picker.
#pragma once

#include "engine/frames.hpp"
#include "options.hpp"

namespace loupe {

// Runs the window until it is closed, or for --ticks ticks; false when no window
// opened or a step or seek failed.
bool run_window(Frames& frames, const Options& options);

} // namespace loupe
