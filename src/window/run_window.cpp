#include "window/run_window.hpp"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <format>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>

#include "board/boards.hpp"
#include "engine/frames.hpp"
#include "imgui.h"
#include "imgui_internal.h"
// After imgui.h, whose types its header uses without including it.
#include "ImGuizmo.h"
#include "library.hpp"
#include "pack.hpp"
#include "paths.hpp"
#include "process.hpp"
#include "scratch.hpp"
#include "tools.hpp"
#include "update/update.hpp"
#include "version.hpp"
#include "window/board_link.hpp"
#include "window/editor.hpp"
#include "window/files.hpp"
#include "window/flow.hpp"
#include "window/frame_texture.hpp"
#include "window/gpu.hpp"
#include "window/observer.hpp"
#include "window/panels.hpp"

namespace loupe {

namespace {

constexpr const char TITLE[] = "loupe";
// The scene's panels, docked by lay_out().
constexpr const char VIEWPORT[] = "Viewport";
constexpr const char OUTLINER[] = "Outliner";
constexpr const char INSPECTOR[] = "Inspector";
constexpr const char DEBUGGER[] = "Frame debugger";
// Each side panel's share of the window: outliner, inspector, frame debugger.
constexpr float OUTLINER_SHARE = 0.18F;
constexpr float INSPECTOR_SHARE = 0.26F;
constexpr float DEBUGGER_SHARE = 0.16F;
// The most of the dock's height the frame debugger grows to, fitting its rows.
constexpr float DEBUGGER_MAX_SHARE = 0.5F;
// A height within this of another is the same: rounding, not a resize.
constexpr float FIT_SLACK = 1.0F;
// How many of a live stream's frames may wait before a tick seeks to the
// newest rather than stepping. A step draws one frame on from the last; a
// seek decodes from the keyframe before it, about six steps' time, so it
// pays only past a backlog that stepping, one a tick, is slow to drain. A
// board's frames arrive in chunks of up to 8, which stepping keeps up with.
constexpr int LIVE_SEEK_BEHIND = 16;
// The outliner's line for a recording, which draws no scene file.
constexpr const char RECORDING_NO_EDITOR[] = "a recording has no scene file to edit";

// `options`' run on `scene`, open on the first frame a seek reaches so every
// control works from the start; null, with `why` set, when it cannot be shown.
std::unique_ptr<Frames>
open_scene(const Options& options, const std::filesystem::path& scene, std::string* why) {
    Options run = options;
    run.scene = scene;
    std::unique_ptr<Frames> frames = open_whole_path(run, why);
    if (frames == nullptr) {
        return nullptr;
    }
    if (frames->count() <= Frames::FIRST_SEEKABLE) {
        *why = std::format("the window needs a run of more than {} frames", Frames::FIRST_SEEKABLE);
        return nullptr;
    }
    frames->keep_checkpoints(options.checkpoint_mb.value_or(DEFAULT_CHECKPOINT_MB) * MIB);
    bool ok = true;
    while (frames->index() <= Frames::FIRST_SEEKABLE) {
        ok &= frames->step(frames->dt_ms());
    }
    if (!ok) {
        *why = "the scene's first frames failed";
        return nullptr;
    }
    return frames;
}

// A recording's stream, open on its first frame when it has one; null, with
// `why` set, when it cannot be shown. A live stream (no scene) opens empty.
std::unique_ptr<Frames>
open_recording(const Options& options, const std::filesystem::path& packs, std::string id, std::string* why) {
    std::unique_ptr<Frames> frames = Frames::open_stream(options, packs, std::move(id), why);
    if (frames != nullptr && frames->count() > 0 && !frames->seek(0)) {
        *why = "the stream's first frame failed";
        return nullptr;
    }
    return frames;
}

// What View readies for `board`, off the frame loop: the reservation when a
// person holds it (`held`), logged to `log`, then autana `commit`'s firmware
// packs, which an install keeps in `shipped` (firmware_packs(), pack.hpp).
struct ReadyJob {
    std::string board;
    std::optional<Board> held;
    std::string commit;
    std::filesystem::path shipped, log;
};

// No packs, with `why` set, when either step of `job` failed.
window::Reserved
ready_recording(const ReadyJob& job) {
    window::Reserved reserved{.board = job.board, .token = "", .why = "", .packs = {}};
    if (job.held) {
        reserved.token = reservation_token(*job.held, job.log, &reserved.why);
        if (reserved.token.empty()) {
            return reserved;
        }
    }
    reserved.packs = firmware_packs(job.commit, job.shipped, &reserved.why).value_or(std::filesystem::path());
    return reserved;
}

// Where a recording from board `id` is kept: <config>/recordings/<board>-<local time>.framerec,
// <board> being board_file_stem(id). A second recording within the same second
// takes -2, -3, ... rather than overwrite.
std::filesystem::path
recording_file(const std::filesystem::path& config, const std::string& id) {
    const std::string board = board_file_stem(id);
    const auto now = std::chrono::zoned_time(
        std::chrono::current_zone(), std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now()));
    const std::string stem = std::format("{}-{:%Y%m%d-%H%M%S}", board, now);
    std::filesystem::path file = config / "recordings" / std::format("{}{}", stem, RECORDING_SUFFIX);
    for (int n = 2; std::filesystem::exists(file); n++) {
        file = config / "recordings" / std::format("{}-{}{}", stem, n, RECORDING_SUFFIX);
    }
    return file;
}

// The open scene's run and what the window keeps for it.
struct Shown {
    std::unique_ptr<Frames> frames;
    // A board's link feeding a live stream; none for a scene or a saved recording.
    std::unique_ptr<window::BoardLink> link;
    // The live board's id, and what Record saves of its stream.
    std::string board;
    StreamSaver saver;
    // What the viewport names while waiting (the board, our packs' autana,
    // whether a frame was drawn), and where the wait stood last tick, logged
    // as it moves on.
    window::WaitFor wait;
    std::optional<window::Waiting> waited;
    window::BoardControls controls;
    window::FrameTexture texture;
    window::Timeline timeline;
    window::Observer observer;
    std::vector<DebugView> views;
    // The editor over the scene file; none for a pack, or without the pack tool, `editor_why` saying why.
    std::optional<window::Editor> editor;
    std::string editor_why;
    bool laid_out = false;
    // The frame debugger's docked height as last fitted to its rows; none
    // once the person sized it, so it is theirs from then on.
    std::optional<float> debugger_fitted = 0.0F;

    Shown(window::Gpu& gpu, std::unique_ptr<Frames> opened)
        : frames(std::move(opened)), texture(gpu, frames->frame()),
          views(frames->has_views() ? debug_views() : std::vector<DebugView>{}) {}
};

// What the panels asked of the shown frame this tick: another view, or the
// observer turned on, off or moved.
struct Asked {
    std::optional<int> view;
    bool observe = false;
};

// Applies one of what the panels asked for this tick, a picked view first, then
// the observer, which pauses the timeline while on; true when the frame on screen
// changed. A failure clears `ok`, and a failed step stops play. A live
// stream steps one frame a tick, and seeks to its newest once more than
// LIVE_SEEK_BEHIND frames wait.
bool
apply(Frames& frames, window::Timeline& timeline, window::Observer& observer, const Asked& asked, bool* ok) {
    if (asked.view) {
        // Draws the shown frame again under the view, playing or not, and leaves the observer.
        observer.on = false;
        *ok &= frames.set_view(*asked.view);
        return true;
    }
    if (observer.on) {
        timeline.playing = false;
        timeline.seek_to.reset();
    }
    if (asked.observe) {
        *ok &= frames.observe(observer.on ? std::optional(observer.fly.pose) : std::nullopt);
        return true;
    }
    if (timeline.seek_to) {
        *ok &= frames.seek(*timeline.seek_to);
        timeline.seek_to.reset();
        return true;
    }
    if (!timeline.playing) {
        return false;
    }
    if (frames.index() >= frames.count()) {
        // The run's last frame; a live stream's play waits for the next.
        timeline.playing = frames.live();
        return false;
    }
    const bool behind = frames.live() && frames.count() - frames.index() > LIVE_SEEK_BEHIND;
    if (!(behind ? frames.seek(frames.count() - 1) : frames.step(frames.dt_ms()))) {
        *ok = false;
        timeline.playing = false;
    }
    return true;
}

// Docks the panels into `dock`, `size` large: the outliner left, the
// inspector right, the frame debugger below, the viewport in the middle.
void
lay_out(ImGuiID dock, ImVec2 size) {
    ImGui::DockBuilderRemoveNode(dock);
    ImGui::DockBuilderAddNode(dock, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dock, size);
    ImGuiID middle = dock;
    const ImGuiID left = ImGui::DockBuilderSplitNode(middle, ImGuiDir_Left, OUTLINER_SHARE, nullptr, &middle);
    const ImGuiID right = ImGui::DockBuilderSplitNode(middle, ImGuiDir_Right, INSPECTOR_SHARE, nullptr, &middle);
    const ImGuiID below = ImGui::DockBuilderSplitNode(middle, ImGuiDir_Down, DEBUGGER_SHARE, nullptr, &middle);
    ImGui::DockBuilderDockWindow(OUTLINER, left);
    ImGui::DockBuilderDockWindow(INSPECTOR, right);
    ImGui::DockBuilderDockWindow(DEBUGGER, below);
    ImGui::DockBuilderDockWindow(VIEWPORT, middle);
    ImGui::DockBuilderFinish(dock);
}

// Grows the frame debugger's dock until its rows show with no scrolling,
// `needed` tall, as rows come (a refusal, an end); call inside its window.
// Never past DEBUGGER_MAX_SHARE of the dock, and never once the person has
// sized it: a height other than the one last fitted is theirs. Read from the
// size the dock keeps (SizeRef), which a splitter drag sets and a smaller or
// minimised window only clamps around.
void
fit_debugger(std::optional<float>& fitted, float needed) {
    ImGuiDockNode* node = ImGui::DockBuilderGetNode(ImGui::GetWindowDockID());
    if (!fitted || node == nullptr || node->ParentNode == nullptr) {
        return;
    }
    if (*fitted > 0.0F && std::abs(node->SizeRef.y - *fitted) > FIT_SLACK) {
        fitted.reset();
        return;
    }
    // The node holds the window and its tab bar above it.
    const float wanted =
        std::min(needed + (node->Size.y - ImGui::GetWindowHeight()), node->ParentNode->Size.y * DEBUGGER_MAX_SHARE);
    if (wanted > node->Size.y + FIT_SLACK) {
        ImGui::DockBuilderSetNodeSize(node->ID, {node->Size.x, wanted});
        node->WantLockSizeOnce = true;
        fitted = wanted;
    } else {
        fitted = node->SizeRef.y;
    }
}

// Record turns saving on, into a new file under <config>/recordings, or off.
void
toggle_record(Shown& shown, const Folders& folders) {
    if (shown.saver.state() != StreamSaver::State::Off) {
        shown.saver.stop();
        return;
    }
    std::string why;
    if (!shown.saver.start(recording_file(folders.config, shown.board), &why)) {
        std::fprintf(stderr, "loupe: %s\n", why.c_str());
    }
}

// What a live board's controls asked, sent to the board, then the bytes it
// sent since the last tick fed to the stream and to the save. A save that
// ends joins the recent list.
void
drive_link(Shown& shown, const Folders& folders) {
    if (!shown.link) {
        return;
    }
    if (shown.controls.send) {
        (void)shown.link->send(*shown.controls.send);
        shown.controls.send.reset();
    }
    if (shown.controls.record) {
        toggle_record(shown, folders);
        shown.controls.record = false;
    }
    const StreamSaver::State was = shown.saver.state();
    std::vector<uint8_t> bytes;
    if (shown.link->take(bytes) > 0) {
        (void)shown.frames->feed(bytes);
        shown.saver.feed(bytes);
    }
    if (was == StreamSaver::State::Saving && shown.saver.state() == StreamSaver::State::Off) {
        (void)remember(folders, shown.saver.file());
    }
}

// What a live board's screen is (live_line()); empty for a scene or a saved recording.
std::string
screen_what(const Shown& shown, const std::optional<window::BoardLink::Status>& status) {
    if (!shown.link) {
        return "";
    }
    const bool saving = shown.saver.state() != StreamSaver::State::Off;
    return window::live_line(shown.wait.board, saving ? shown.saver.file() : std::filesystem::path(),
                             status && status->running);
}

// What the viewport says in place of a recording's frame until the first one
// shows; each step is logged with its time, so the wait can be measured.
// Waiting::Shown for a scene or a saved recording.
window::Waiting
draw_wait(Shown& shown, const std::optional<window::BoardLink::Status>& status) {
    const std::optional<Frames::StreamState> stream = shown.frames->stream_state();
    if (!status || !stream) {
        return window::Waiting::Shown;
    }
    const window::WaitLine line = window::waiting_for_frames(*status, *stream, shown.wait);
    if (line.phase != shown.waited) {
        std::fprintf(stderr, "loupe: recording %s: wait step %d at %.3f s\n", shown.wait.board.c_str(),
                     static_cast<int>(line.phase), std::chrono::duration<double>(status->elapsed).count());
        shown.waited = line.phase;
    }
    if (line.phase != window::Waiting::Shown) {
        window::draw_waiting(line);
    }
    return line.phase;
}

// The viewport: the handle picker, the frame, and over it the observer's
// steering or the editor's handles. True when the observer moved.
bool
draw_viewport(Shown& shown, const std::optional<window::BoardLink::Status>& status) {
    if (draw_wait(shown, status) != window::Waiting::Shown) {
        return false;
    }
    const bool editing = shown.editor && !shown.observer.on;
    if (editing) {
        window::draw_handle_bar(*shown.editor);
        ImGui::SameLine();
        (void)window::draw_free_view_toggle(*shown.editor, *shown.frames);
        window::draw_path_scrub(*shown.editor, *shown.frames, &shown.timeline.seek_to);
    }
    const window::FrameRect rect = window::draw_frame(shown.texture);
    const float dt = ImGui::GetIO().DeltaTime;
    const bool moved = shown.observer.on && window::drive_observer(shown.observer, dt);
    if (editing) {
        (void)window::drive_free_view(*shown.editor, dt);
        window::draw_handles(*shown.editor, *shown.frames, rect);
    }
    return moved;
}

// The editor's tick: its keys and the Edit menu applied, then its placements
// and camera paths synced to the frames; true when that drew a frame.
bool
edit_tick(window::Editor& editor, Shown& shown, const window::EditMenu& menu, bool* ok) {
    // WASD flies the observer or the free view; W, E and R pick the handle otherwise.
    window::EditAsk ask = window::edit_keys(editor, !shown.observer.on && !editor.path.free.on);
    ask.undo |= menu.undo;
    ask.redo |= menu.redo;
    ask.save |= menu.save;
    window::apply(editor, ask);
    if (!shown.observer.on) {
        const int64_t drawn = shown.frames->drawn();
        *ok &= window::sync(editor, [&shown](const std::string& object, const transformf_t& local) {
            return shown.frames->place(object, local);
        });
        *ok &= window::sync_paths(
            editor,
            [&shown](const std::string& camera, Frames::PoseAt pose) {
                return shown.frames->fly(camera, std::move(pose));
            },
            [&shown](const std::string& camera) { return shown.frames->land(camera); });
        return shown.frames->drawn() != drawn;
    }
    return false;
}

// The open scene's tick: its docked panels, then what they and the Edit menu
// asked; true when Scenes was pressed, to go back to the start panel.
bool
draw_shown(Shown& shown, const Folders& folders, const window::EditMenu& menu, bool* ok) {
    const ImGuiID dock = ImGui::GetID("panels");
    if (!shown.laid_out) {
        lay_out(dock, ImGui::GetContentRegionAvail());
        shown.laid_out = true;
    }
    ImGui::DockSpace(dock);
    Asked asked;
    const bool was_on = shown.observer.on;
    bool back = false;
    const std::optional<window::BoardLink::Status> status =
        shown.link ? std::optional(shown.link->status()) : std::nullopt;
    if (ImGui::Begin(DEBUGGER)) {
        back = window::draw_debugger({.frames = *shown.frames,
                                      .views = shown.views,
                                      .stream = shown.frames->stream_state(),
                                      .link = status,
                                      .saving = shown.saver.state(),
                                      .what = screen_what(shown, status)},
                                     shown.controls, shown.observer, shown.timeline, &asked.view);
        fit_debugger(shown.debugger_fitted, window::content_height());
    }
    ImGui::End();
    bool moved = false;
    if (ImGui::Begin(VIEWPORT)) {
        moved = draw_viewport(shown, status);
    }
    ImGui::End();
    // A tree row cannot wrap: an object named wider than the panel scrolls sideways.
    if (ImGui::Begin(OUTLINER, nullptr, ImGuiWindowFlags_HorizontalScrollbar)) {
        if (shown.editor) {
            window::draw_outliner(*shown.editor);
        } else {
            ImGui::TextWrapped("%s", shown.editor_why.c_str());
        }
    }
    ImGui::End();
    if (ImGui::Begin(INSPECTOR) && shown.editor) {
        window::draw_inspector(*shown.editor);
    }
    ImGui::End();
    asked.observe = moved || shown.observer.on != was_on;
    drive_link(shown, folders);
    // Uploaded after the frame is drawn, for the observer's input over it; still this tick's picture.
    bool changed = apply(*shown.frames, shown.timeline, shown.observer, asked, ok);
    if (shown.editor) {
        changed |= edit_tick(*shown.editor, shown, menu, ok);
    }
    if (changed) {
        shown.texture.upload(shown.frames->frame());
        // From the first frame shown on, a later refusal leaves it in view.
        shown.wait.drew = true;
    }
    return back;
}

// Ends the tick's frame; with `picture`, also reads back what it drew, left
// empty, and said, when it could not be.
void
end_frame(window::Gpu& gpu, std::optional<window::Picture>* picture) {
    if (picture == nullptr) {
        gpu.end_frame();
        return;
    }
    std::string why;
    *picture = gpu.end_frame_read_back(&why);
    if (!*picture) {
        std::fprintf(stderr, "loupe: cannot read the window back: %s\n", why.c_str());
    }
}

// The window's state between ticks: the open scene, or the start panel's.
class Window {
  public:
    // A `scripted` window (--ticks, a recipe) neither updates nor remembers what it opened.
    Window(window::Gpu& gpu, const Options& options, bool scripted);
    // Opens `options`' scene or recording in place of the open one; false,
    // said on the start panel, when it did not open.
    bool open(const Options& options);
    // Shows `reserved.board` live in place of the open scene, inside the
    // lock its token names when not empty; false, said on the start panel,
    // when it did not start.
    bool attach(const window::Reserved& reserved);
    // One tick's panels, then the scene they asked for; with `picture`, also
    // what the tick drew, which stays empty when it could not be read back.
    void tick(std::optional<window::Picture>* picture = nullptr);
    // Closes the open scene; false when a run failed along the way.
    bool finish();

  private:
    // Closes the open scene, if any, back to the start panel.
    void close();
    // Scenes pressed: a running recording is stopped first, and the window
    // shows it draining until it has ended, then closes.
    void leave();
    // Starts a board listing when asked, off this thread; takes it when done.
    void list_boards();
    // Shows the board View was pressed for, live. A person's reservation is
    // first taken off this thread, so the link runs inside it.
    void view_asked();

    window::Gpu& gpu_;
    // A window run's stand-in for -o: autana's close writes the last frame, to
    // stdout without -o, and with it the engine's stdout beside the image
    // (out.bmp.console). The window wants neither, so both go here, until
    // autana can close a run without writing an image.
    Scratch scratch_;
    std::optional<size_t> checkpoint_mb_;
    Folders folders_;
    update::Copy copy_;
    // Only a window someone uses updates and remembers what it opened.
    bool scripted_ = false;
    bool updates_ = false;
    window::About about_;
    // The update check, stopped and joined before the staged build is applied.
    std::jthread checker_;
    Library library_;
    window::Attach attach_;
    std::string refusal_;
    std::optional<Shown> shown_;
    bool leaving_ = false; // leave() waits for a recording to end
    bool ok_ = true;
};

Window::Window(window::Gpu& gpu, const Options& options, bool scripted)
    : gpu_(gpu), checkpoint_mb_(options.checkpoint_mb), folders_(window::folders()),
      copy_{.id = build_id(folders_.exe_dir), .exe = exe_path(), .config = folders_.config}, scripted_(scripted),
      updates_(!scripted_ && copy_.id.built_for != Channel::None && !copy_.config.empty()),
      library_(library(folders_)) {
    about_.version = version_line(copy_.id);
    about_.can_update = updates_;
    if (updates_) {
        about_.updated = update::settle(copy_).value_or("");
        about_.follow = update::follow(copy_);
        checker_ = update::check_in_background(copy_, about_.follow);
    }
}

void
Window::tick(std::optional<window::Picture>* picture) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::Begin(TITLE, nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar
                     | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGuizmo::BeginFrame();
    window::EditMenu menu;
    if (shown_ && shown_->editor) {
        const window::Editor& editor = *shown_->editor;
        menu = {.shown = true,
                .can_undo = editor.edits.can_undo(),
                .can_redo = editor.edits.can_redo(),
                .can_save = !window::dragging(editor)};
    }
    if (window::draw_menu(about_, menu) && !update::set_follow(copy_, about_.follow)) {
        std::fprintf(stderr, "loupe: cannot save the update setting in %s\n", utf8(copy_.config).c_str());
    }
    window::Start start;
    bool back = false;
    if (shown_) {
        back = draw_shown(*shown_, folders_, menu, &ok_);
    } else {
        window::draw_start(library_, refusal_, start);
        window::draw_attach(attach_);
    }
    ImGui::End();
    end_frame(gpu_, picture);
    if (back) {
        leave();
    }
    if (leaving_ && (!shown_ || !shown_->link || !shown_->link->status().running)) {
        leaving_ = false;
        close();
    }
    list_boards();
    view_asked();
    if (start.browse) {
        window::browse(gpu_.sdl_window());
    }
    // A scene picked on the start panel opens with the provider's defaults.
    if (const std::optional<std::filesystem::path> scene = start.picked ? start.picked : window::browsed()) {
        Options picked;
        picked.scene = *scene;
        picked.checkpoint_mb = checkpoint_mb_;
        (void)open(picked);
    }
}

bool
Window::finish() {
    close();
    checker_.request_stop();
    if (checker_.joinable()) {
        checker_.join();
    }
    std::string why;
    const update::Follow follow = update::follow(copy_);
    if (updates_ && follow != update::Follow::Off) {
        update::log(copy_, update::apply_staged(copy_, follow, &why) ? "swapped the staged build in"
                           : why.empty()                             ? "nothing staged"
                                                                     : why);
    }
    return ok_;
}

void
Window::leave() {
    if (shown_ && shown_->link && shown_->link->status().running) {
        shown_->link->stop();
        leaving_ = true;
        return;
    }
    close();
}

void
Window::list_boards() {
    if (attach_.refresh && !attach_.listing.valid()) {
        attach_.listing = std::async(std::launch::async, loupe::list_boards, scratch_.path() / "devices.log");
        attach_.listing_since = std::chrono::steady_clock::now();
        attach_.refresh = false;
    }
    if (attach_.listing.valid() && attach_.listing.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        attach_.list = attach_.listing.get();
        attach_.listed = true;
    }
}

void
Window::view_asked() {
    if (attach_.view) {
        const auto listed = std::ranges::find(attach_.list.boards, *attach_.view, &Board::id);
        std::optional<Board> held;
        if (listed != attach_.list.boards.end() && listed->holder == Holder::Person) {
            held = *listed;
        }
        attach_.packs_autana = copy_.id.autana_commit;
        attach_.reserving_since = std::chrono::steady_clock::now();
        std::fprintf(stderr, "loupe: readying %s: %sthe packs of autana %s\n", attach_.view->c_str(),
                     held ? "its reservation (autana lock hand), then " : "", attach_.packs_autana.c_str());
        attach_.reserving = std::async(std::launch::async, ready_recording,
                                       ReadyJob{.board = *attach_.view,
                                                .held = held,
                                                .commit = copy_.id.autana_commit,
                                                .shipped = firmware_dir(folders_),
                                                .log = scratch_.path() / "hand.log"});
        attach_.view.reset();
    }
    if (attach_.reserving.valid() && attach_.reserving.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        const window::Reserved reserved = attach_.reserving.get();
        std::fprintf(stderr, "loupe: readied %s in %.3f s\n", reserved.board.c_str(),
                     std::chrono::duration<double>(std::chrono::steady_clock::now() - attach_.reserving_since).count());
        if (shown_) {
            // A scene opened while the live view was readied: it stays.
            return;
        }
        if (reserved.packs.empty()) {
            refusal_ = std::format("{}: {}", reserved.board, reserved.why);
            std::fprintf(stderr, "loupe: %s\n", refusal_.c_str());
            return;
        }
        (void)attach(reserved);
    }
}

void
Window::close() {
    if (shown_) {
        // A save under way ends with the frames that came; one that saved none leaves no file.
        if (shown_->saver.state() != StreamSaver::State::Off) {
            shown_->saver.stop();
            if (shown_->saver.frames() > 0) {
                (void)remember(folders_, shown_->saver.file());
            }
        }
        ok_ &= shown_->frames->close();
        shown_.reset();
        gpu_.set_title(TITLE);
        library_ = library(folders_);
    }
}

bool
Window::open(const Options& options) {
    close();
    Options run = options;
    run.out_bmp = utf8(scratch_.path() / "frame.bmp");
    std::string why = "cannot make a scratch folder for its frames";
    const bool recording = utf8(options.scene.filename()).ends_with(RECORDING_SUFFIX);
    std::unique_ptr<Frames> frames;
    if (!scratch_.path().empty() && !recording) {
        frames = open_scene(run, options.scene, &why);
    } else if (!scratch_.path().empty()) {
        // Built with Loupe for its pin, so a replay waits for none.
        if (const auto packs = firmware_packs(copy_.id.autana_commit, firmware_dir(folders_), &why)) {
            frames = open_recording(run, *packs, utf8(options.scene.stem()), &why);
        }
    }
    if (frames == nullptr) {
        refusal_ = std::format("{}: {}", utf8(options.scene), why);
        std::fprintf(stderr, "loupe: %s\n", refusal_.c_str());
        return false;
    }
    refusal_.clear();
    if (!scripted_) {
        (void)remember(folders_, options.scene);
    }
    gpu_.set_title(std::format("{}: {}", TITLE, frames->scene_id()));
    shown_.emplace(gpu_, std::move(frames));
    if (recording) {
        shown_->editor_why = RECORDING_NO_EDITOR;
    } else {
        shown_->editor = window::open_editor(options.scene, &shown_->editor_why);
    }
    return true;
}

bool
Window::attach(const window::Reserved& reserved) {
    const std::string& board = reserved.board;
    close();
    std::string why = "no settings folder to keep the link's log in";
    std::unique_ptr<Frames> frames;
    std::unique_ptr<window::BoardLink> link;
    if (!folders_.config.empty() && !scratch_.path().empty()) {
        const std::filesystem::path log = folders_.config / "logs" / std::format("{}.log", board_file_stem(board));
        Options live;
        live.out_bmp = utf8(scratch_.path() / "frame.bmp");
        frames = open_recording(live, reserved.packs, board, &why);
        std::vector<std::string> args = {"-b", board, "record"};
        if (!reserved.token.empty()) {
            args.insert(args.begin(), {"--lock-token", reserved.token});
        }
        link = frames != nullptr ? window::BoardLink::start(autana_command(args), log, &why) : nullptr;
    }
    if (link == nullptr) {
        refusal_ = std::format("{}: {}", board, why);
        std::fprintf(stderr, "loupe: %s\n", refusal_.c_str());
        return false;
    }
    refusal_.clear();
    gpu_.set_title(std::format("{}: {} (live)", TITLE, board));
    shown_.emplace(gpu_, std::move(frames));
    shown_->link = std::move(link);
    shown_->board = board;
    const auto listed = std::ranges::find(attach_.list.boards, board, &Board::id);
    shown_->wait.board = listed == attach_.list.boards.end() || listed->port.empty()
                             ? board
                             : std::format("{} ({})", board, listed->port);
    shown_->wait.packs_autana = copy_.id.autana_commit;
    shown_->editor_why = RECORDING_NO_EDITOR;
    // Play draws each frame as it arrives.
    shown_->timeline.playing = true;
    return true;
}

} // namespace

bool
run_window(const Options& options) {
    std::string why;
    const std::unique_ptr<window::Gpu> gpu = window::Gpu::open(TITLE, &why);
    if (gpu == nullptr) {
        std::fprintf(stderr, "loupe: %s\n", why.c_str());
        return false;
    }
    Window window(*gpu, options, options.ticks.has_value());
    const bool opened = options.scene.empty() || window.open(options);
    for (int tick = 0; (!options.ticks || tick < *options.ticks) && gpu->begin_frame(); tick++) {
        window.tick();
    }
    return window.finish() && opened;
}

bool
run_scripted(float content_scale, Script& script) {
    std::string why;
    const std::unique_ptr<window::Gpu> gpu = window::Gpu::open(TITLE, &why, content_scale);
    if (gpu == nullptr) {
        std::fprintf(stderr, "loupe: %s\n", why.c_str());
        return false;
    }
    Window window(*gpu, Options{}, true);
    script.start();
    for (;;) {
        Script::Ask ask = script.before_tick();
        if (ask.done) {
            break;
        }
        Script::Outcome outcome;
        if (ask.open) {
            outcome.opened = window.open(*ask.open);
        }
        if (!gpu->begin_frame()) {
            break;
        }
        window.tick(ask.picture ? &outcome.picture : nullptr);
        script.after_tick(ask, std::move(outcome));
    }
    script.stop();
    return window.finish();
}

} // namespace loupe
