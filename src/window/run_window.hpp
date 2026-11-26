// The window: a scene's frame, a timeline and a debug-view picker, or the
// start panel while no scene is open.
#pragma once

#include <optional>

#include "options.hpp"
#include "window/gpu.hpp"

namespace loupe {

// Runs the window on `options`' scene, or on the start panel without one,
// until it is closed or for --ticks ticks. False when no window opened, the
// given scene did not, or a step, seek or run failed.
bool run_window(const Options& options);

// What drives a scripted window (loupe-storyboard): it starts once the window
// and ImGui are up, says before each tick what that tick is for, gets the
// tick's outcome, and stops before the window goes down.
class Script {
  public:
    // Before a tick: a scene to open first, whether to read the tick's picture back, or the end of the run.
    struct Ask {
        std::optional<Options> open;
        bool picture = false;
        bool done = false;
    };

    // The tick asked for: whether its scene opened, and the picture when one was asked for and read.
    struct Outcome {
        bool opened = true;
        std::optional<window::Picture> picture;
    };

    Script() = default;
    Script(const Script&) = delete;
    Script& operator=(const Script&) = delete;
    Script(Script&&) = delete;
    Script& operator=(Script&&) = delete;
    virtual ~Script() = default;
    virtual void start() = 0;
    virtual Ask before_tick() = 0;
    virtual void after_tick(const Ask& asked, Outcome outcome) = 0;
    virtual void stop() = 0;
};

// Runs the window, its panels at `content_scale`, as `script` asks until it is
// done or the window closes; false when no window opened or a run failed.
bool run_scripted(float content_scale, Script& script);

} // namespace loupe
