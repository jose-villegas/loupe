// The window's panels. They only record what the viewer asked for; the tick
// (run_window.cpp) applies it to Frames.
#pragma once

#include <chrono>
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
#include "record/stream_saver.hpp"
#include "update/update.hpp"
#include "window/board_link.hpp"
#include "window/frame_texture.hpp"
#include "window/observer.hpp"

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

// The listing's boards as the panel shows them: those plugged in here, which
// View can attach to, then the rest the pool knows, under a collapsed heading.
struct BoardRows {
    std::vector<const Board*> here, elsewhere;
};

BoardRows board_rows(const BoardList& list);

// What View readies before autana record starts: the board, the token of a
// person's reservation to record inside (empty when no person holds it), and
// the folder of packs its stream is drawn with; `why` set when either failed.
struct Reserved {
    std::string board, token, why;
    std::filesystem::path packs;
};

// The start panel's boards: the pool's last listing, why it failed, and
// what was asked this tick: list them again, or view one live.
struct Attach {
    BoardList list;
    bool listed = false;
    bool refresh = false;
    std::future<BoardList> listing;                                       // valid while `autana devices` runs
    std::chrono::steady_clock::time_point listing_since, reserving_since; // when each began
    std::optional<std::string> view;
    std::future<Reserved> reserving; // valid while the reservation is taken and the packs are built
    std::string packs_autana;        // the autana commit the board's packs are built from
};

// A live board's controls: what was asked this tick. `paused` is the
// panel's own state between ticks: FREEZE sent, RESUME not yet. `record`
// turns saving on or off.
struct BoardControls {
    bool paused = false;
    bool record = false;
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
// Record or Stop recording, Freeze or Unfreeze, Step, and the stream's frame
// and dropped counts; a refused stream or an ended link says why.
void draw_board_controls(const Frames::StreamState& stream, const BoardLink::Status& status, StreamSaver::State saving,
                         BoardControls& controls);
// Play or pause, a step back and forward, and a scrubber over the run's frames.
void draw_timeline(const Frames& frames, Timeline& timeline);
// How wide draw_view_picker() draws, label included, when the line has room.
float view_picker_width(const std::vector<DebugView>& views);
// The debug-view picker; true when `view` changed.
bool draw_view_picker(const std::vector<DebugView>& views, int* view);

// What the frame debugger shows: the run, its views, and for a stream its
// counts and, while it is live, the board's link and the save.
struct Debugger {
    const Frames& frames;
    const std::vector<DebugView>& views;
    std::optional<Frames::StreamState> stream;
    std::optional<BoardLink::Status> link;
    StreamSaver::State saving = StreamSaver::State::Off;
    std::string what; // what the screen is (live_line()); empty for a scene
};

// What a live board's screen is, in one line: "Live: BOARD", and the file
// its frames are saved to while Record is on (`saving`, else empty); once
// the link has ended (`running` false), that it ended.
std::string live_line(const std::string& board, const std::filesystem::path& saving, bool running);

// The height the current window needs to show what it has drawn so far with
// no scrolling: its last row's bottom and the window's padding. Call it
// after the rows, before End().
float content_height();

// Scenes, the view picker and the observer; a live board's controls, or a
// saved stream's counts and its timeline. Each row wraps at the panel's
// edge. True when Scenes was pressed; a picked view goes into `*view`.
bool draw_debugger(const Debugger& shown, BoardControls& controls, Observer& observer, Timeline& timeline,
                   std::optional<int>* view);

// Where a live view stands until its first frame shows, from what autana has
// written so far: started, its own lines, the board streaming, stream bytes
// read; or why it ended or was refused before any frame.
enum class Waiting : uint8_t { Starting, Autana, Linked, Reading, Shown, Ended, Refused };

struct WaitLine {
    Waiting phase = Waiting::Starting;
    std::string text; // empty once Shown
};

// What the wait knows beside autana and the stream: the board, as
// "A308 (COM5)"; the autana commit whose packs this Loupe carries, named when
// a stream is refused; whether a frame has been drawn yet.
struct WaitFor {
    std::string board, packs_autana;
    bool drew = false;
};

// Shown once a frame was drawn. A refusal before that is said in the frame's
// place, as the stream may count a frame it then refuses to draw.
WaitLine waiting_for_frames(const BoardLink::Status& status, const Frames::StreamState& stream, const WaitFor& wait);
// `line` wrapped to the room left in the window, centred top to bottom.
void draw_waiting(const WaitLine& line);

// Where draw_frame put the frame, in screen pixels.
struct FrameRect {
    ImVec2 min, max;
};

// The frame at the largest whole zoom that fits, sampled nearest.
FrameRect draw_frame(const FrameTexture& texture);

} // namespace loupe::window
