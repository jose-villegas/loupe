// The window: a scene's frame, a timeline and a debug-view picker, or the
// start panel while no scene is open.
#pragma once

#include "options.hpp"

namespace loupe {

// Runs the window on `options`' scene, or on the start panel without one,
// until it is closed or for --ticks ticks. False when no window opened, the
// given scene did not, or a step, seek or run failed.
bool run_window(const Options& options);

} // namespace loupe
