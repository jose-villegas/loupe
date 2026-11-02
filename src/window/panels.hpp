// The window's panels. They only record what the viewer asked for; the tick
// (run_window.cpp) applies it to Frames.
#pragma once

#include <optional>
#include <vector>

#include "engine/frames.hpp"
#include "window/frame_texture.hpp"

namespace loupe::window {

struct Timeline {
    bool playing = false;
    // A frame to seek to, from the scrubber or the step buttons.
    std::optional<int> seek_to;
};

// Play or pause, a step back and forward, and a scrubber over the run's frames.
void draw_timeline(const Frames& frames, Timeline& timeline);
// The debug-view picker; true when `view` changed.
bool draw_view_picker(const std::vector<DebugView>& views, int* view);
// The frame at the largest whole zoom that fits, sampled nearest.
void draw_frame(const FrameTexture& texture);

} // namespace loupe::window
