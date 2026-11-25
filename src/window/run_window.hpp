// The window: a scene's frame, a timeline and a debug-view picker, or the
// start panel while no scene is open.
#pragma once

#include "options.hpp"

namespace loupe {

// Runs the window on `options`' scene, or on the start panel without one,
// until it is closed or for --ticks ticks. False when no window opened, the
// given scene did not, or a step, seek or run failed.
bool run_window(const Options& options);
// Writes the doc shot `options.shot` (window/doc_shots.hpp) to `options.out_bmp`,
// a PNG, or with "all" every shot into that folder as <name>.png; false for an
// unknown shot, or when one could not be driven or written.
bool run_shot(const Options& options);

} // namespace loupe
