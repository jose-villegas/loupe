// The window's panels. They only record what the viewer asked for; the tick
// (run_window.cpp) applies it to Frames.
#pragma once

#include <cstdint>
#include <filesystem>
#include <future>
#include <optional>
#include <string>
#include <vector>

#include "board/boards.hpp"
#include "engine/frames.hpp"
#include "imgui.h"
#include "library.hpp"
#include "update/update.hpp"
#include "window/frame_texture.hpp"
#include "window/observer.hpp"
#include "window/recording.hpp"

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

// The start panel's boards: the pool's last listing, why it failed, and
// what was asked this tick: list them again, or record one.
struct Attach {
    BoardList list;
    bool listed = false;
    bool refresh = false;
    std::future<BoardList> listing; // valid while `autana devices` runs
    std::optional<std::string> record;
};

// A live recording's controls: what was asked this tick. `paused` is the
// panel's own state between ticks: FREEZE sent, RESUME not yet.
struct RecordControls {
    bool paused = false;
    bool stop = false;
    std::optional<std::string> send; // FREEZE, RESUME or STEP
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
// The boards to record from: List boards, then each with its port and Record when free.
void draw_attach(Attach& attach);
// Stop, Pause or Unpause, Next frame, and the stream's frame and dropped
// counts; a refused stream or an ended recording says why.
void draw_record_controls(const Frames::StreamState& stream, const Recording::Status& status, RecordControls& controls);
// Play or pause, a step back and forward, and a scrubber over the run's frames.
void draw_timeline(const Frames& frames, Timeline& timeline);
// How wide draw_view_picker() draws, label included, when the line has room.
float view_picker_width(const std::vector<DebugView>& views);
// The debug-view picker; true when `view` changed.
bool draw_view_picker(const std::vector<DebugView>& views, int* view);

// What the frame debugger shows: the run, its views, and for a stream its
// counts and, while it records, the recording's state.
struct Debugger {
    const Frames& frames;
    const std::vector<DebugView>& views;
    std::optional<Frames::StreamState> stream;
    std::optional<Recording::Status> recording;
};

// Scenes, the view picker and the observer; a stream's line, with a
// recording's controls; then the timeline. Each row wraps at the panel's
// edge. True when Scenes was pressed; a picked view goes into `*view`.
bool draw_debugger(const Debugger& shown, RecordControls& controls, Observer& observer, Timeline& timeline,
                   std::optional<int>* view);

// Where a recording stands until its first frame shows, from what autana has
// written so far: started, its own lines, the board recording, stream bytes
// read; or why it ended or was refused before any frame.
enum class Waiting : uint8_t { Starting, Autana, Recording, Reading, Shown, Ended, Refused };

struct WaitLine {
    Waiting phase = Waiting::Starting;
    std::string text; // empty once Shown
};

// `board` names the board, as "A308 (COM5)".
WaitLine waiting_for_frames(const Recording::Status& status, const Frames::StreamState& stream,
                            const std::string& board);
// `line` wrapped to the room left in the window, centred top to bottom.
void draw_waiting(const WaitLine& line);

// Where draw_frame put the frame, in screen pixels.
struct FrameRect {
    ImVec2 min, max;
};

// The frame at the largest whole zoom that fits, sampled nearest.
FrameRect draw_frame(const FrameTexture& texture);

} // namespace loupe::window
