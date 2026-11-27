// The window's panels. They only record what the viewer asked for; the tick
// (run_window.cpp) applies it to Frames.
#pragma once

#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "engine/frames.hpp"
#include "imgui.h"
#include "library.hpp"
#include "update/update.hpp"
#include "window/frame_texture.hpp"

namespace loupe::window {

struct Timeline {
    bool playing = false;
    // A frame to seek to, from the scrubber or the step buttons.
    std::optional<int> seek_to;
};

struct Start {
    // A sample or recent scene to open.
    std::optional<std::filesystem::path> picked;
    // Open... was pressed: show the open-file dialog.
    bool browse = false;
};

// What About shows and sets: the build's version_line(), "Updated to ..."
// after a swap, and what the copy follows, which a local build cannot change.
struct About {
    std::string version;
    std::string updated;
    bool can_update = false;
    update::Follow follow = update::Follow::Off;
};

// The menu bar's Edit menu, shown when a scene file is open: what it may do
// (`can_*`), and what was picked this tick.
struct EditMenu {
    bool shown = false, can_undo = false, can_redo = false, can_save = false;
    bool undo = false, redo = false, save = false;
};

// The menu bar: Edit, then About. True when the viewer picked another `about.follow`.
bool draw_menu(About& about, EditMenu& edit);
// The window without a scene: the samples, the recent scenes and Open...;
// `refusal` says why the last scene picked did not open.
void draw_start(const Library& library, const std::string& refusal, Start& start);
// Play or pause, a step back and forward, and a scrubber over the run's frames.
void draw_timeline(const Frames& frames, Timeline& timeline);
// The debug-view picker; true when `view` changed.
bool draw_view_picker(const std::vector<DebugView>& views, int* view);
// `text` in the disabled colour, wrapped to the panel.
void hint(const char* text);

// Field rows, as an inspector lays them out: a label, then its values. The
// labels share one column, so every row's values start at the same x.
// The column's width for these rows: the widest of `labels`, and a gap.
float field_column(std::span<const char* const> labels);
// One row: `label` in the Text face, then `values` in Mono, dragged or typed,
// each left-aligned in a frame tighter than a button's, the frames
// sharing the rest of the panel's width. Laid out and named as DragFloatN
// would with the label "##label". True when a value changed.
bool field_row(const char* label, float column, std::span<float> values, float speed, const char* format);

// Where draw_frame put the frame, in screen pixels.
struct FrameRect {
    ImVec2 min, max;
};

// The frame at the largest whole zoom that fits, sampled nearest.
FrameRect draw_frame(const FrameTexture& texture);

} // namespace loupe::window
