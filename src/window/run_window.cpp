#include "window/run_window.hpp"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <format>
#include <future>
#include <memory>
#include <optional>
#include <string>
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
#include "window/editor.hpp"
#include "window/files.hpp"
#include "window/frame_texture.hpp"
#include "window/gpu.hpp"
#include "window/observer.hpp"
#include "window/panels.hpp"
#include "window/recording.hpp"

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

// Where a recording from `board` is kept: <config>/recordings/<board>-<local time>.framerec,
// the board's serial without its colons, which a Windows file name cannot hold.
// A second recording within the same second takes -2, -3, ... rather than overwrite.
std::filesystem::path
recording_file(const std::filesystem::path& config, std::string board) {
    std::erase(board, ':');
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
    // A board's recording feeding a live stream; none for a scene or a saved recording.
    std::unique_ptr<window::Recording> recording;
    window::RecordControls controls;
    window::FrameTexture texture;
    window::Timeline timeline;
    window::Observer observer;
    std::vector<DebugView> views;
    // The editor over the scene file; none for a pack, or without the pack tool, `editor_why` saying why.
    std::optional<window::Editor> editor;
    std::string editor_why;
    bool laid_out = false;

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
// changed. A failure clears `ok`, and a failed step stops play.
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
    if (!frames.step(frames.dt_ms())) {
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

// A stream's line: a recording's controls, or a saved stream's counts.
void
draw_stream(Shown& shown, const Frames::StreamState& stream) {
    if (shown.recording) {
        window::draw_record_controls(stream, shown.recording->status(), shown.controls);
    } else {
        ImGui::Text("%d frames, %d dropped", stream.frames, stream.dropped);
    }
}

// What a recording's controls asked, sent to the board, then the bytes it
// sent since the last tick fed to the stream.
void
drive_recording(Shown& shown) {
    if (!shown.recording) {
        return;
    }
    if (shown.controls.send) {
        (void)shown.recording->send(*shown.controls.send);
        shown.controls.send.reset();
    }
    if (shown.controls.stop) {
        shown.recording->stop();
        shown.controls.stop = false;
    }
    std::vector<uint8_t> bytes;
    if (shown.recording->take(bytes) > 0) {
        (void)shown.frames->feed(bytes);
    }
}

// The frame debugger: Scenes, a stream's line, the view picker, the observer and the timeline.
// True when Scenes was pressed; a picked view goes into `asked`.
bool
draw_debugger(Shown& shown, Asked& asked) {
    const bool back = ImGui::Button("Scenes");
    if (const std::optional<Frames::StreamState> stream = shown.frames->stream_state()) {
        ImGui::SameLine();
        draw_stream(shown, *stream);
    }
    ImGui::SameLine();
    int view = shown.frames->view();
    if (!shown.views.empty() && window::draw_view_picker(shown.views, &view)) {
        asked.view = view;
    }
    ImGui::SameLine();
    window::draw_observer_toggle(*shown.frames, shown.observer);
    ImGui::BeginDisabled(shown.observer.on);
    window::draw_timeline(*shown.frames, shown.timeline);
    ImGui::EndDisabled();
    return back;
}

// The viewport: the handle picker, the frame, and over it the observer's
// steering or the editor's handles. True when the observer moved.
bool
draw_viewport(Shown& shown) {
    const bool editing = shown.editor && !shown.observer.on;
    if (editing) {
        window::draw_handle_bar(*shown.editor);
    }
    const window::FrameRect rect = window::draw_frame(shown.texture);
    const bool moved = shown.observer.on && window::drive_observer(shown.observer, ImGui::GetIO().DeltaTime);
    if (editing) {
        window::draw_handles(*shown.editor, *shown.frames, rect.min, rect.max);
    }
    return moved;
}

// The open scene's tick: its docked panels, then what they and the Edit menu
// asked; true when Scenes was pressed, to go back to the start panel.
bool
draw_shown(Shown& shown, const window::EditMenu& menu, bool* ok) {
    const ImGuiID dock = ImGui::GetID("panels");
    if (!shown.laid_out) {
        lay_out(dock, ImGui::GetContentRegionAvail());
        shown.laid_out = true;
    }
    ImGui::DockSpace(dock);
    Asked asked;
    const bool was_on = shown.observer.on;
    bool back = false;
    if (ImGui::Begin(DEBUGGER)) {
        back = draw_debugger(shown, asked);
    }
    ImGui::End();
    bool moved = false;
    if (ImGui::Begin(VIEWPORT)) {
        moved = draw_viewport(shown);
    }
    ImGui::End();
    if (ImGui::Begin(OUTLINER)) {
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
    drive_recording(shown);
    // Uploaded after the frame is drawn, for the observer's input over it; still this tick's picture.
    bool changed = apply(*shown.frames, shown.timeline, shown.observer, asked, ok);
    if (shown.editor) {
        window::EditAsk ask = window::edit_keys(*shown.editor, !shown.observer.on);
        ask.undo |= menu.undo;
        ask.redo |= menu.redo;
        ask.save |= menu.save;
        window::apply(*shown.editor, ask);
        if (!shown.observer.on) {
            const int64_t drawn = shown.frames->drawn();
            *ok &= window::sync(*shown.editor, [&shown](const std::string& object, const transformf_t& local) {
                return shown.frames->place(object, local);
            });
            changed |= shown.frames->drawn() != drawn;
        }
    }
    if (changed) {
        shown.texture.upload(shown.frames->frame());
    }
    return back;
}

// The window's state between ticks: the open scene, or the start panel's.
class Window {
  public:
    Window(window::Gpu& gpu, const Options& options);
    // Opens `options`' scene or recording in place of the open one; false,
    // said on the start panel, when it did not open.
    bool open(const Options& options);
    // Records `board` into a live stream in place of the open scene; false,
    // said on the start panel, when it did not start.
    bool attach(const std::string& board);
    // One tick's panels, then the scene they asked for.
    void tick();
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

    window::Gpu& gpu_;
    // A window run's stand-in for -o: autana's close writes the last frame, to
    // stdout without -o, and with it the engine's stdout beside the image
    // (out.bmp.console). The window wants neither, so both go here, until
    // autana can close a run without writing an image.
    Scratch scratch_;
    std::optional<size_t> checkpoint_mb_;
    Folders folders_;
    update::Copy copy_;
    // Only a window someone uses updates; a scripted run (--ticks) never does.
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

Window::Window(window::Gpu& gpu, const Options& options)
    : gpu_(gpu), checkpoint_mb_(options.checkpoint_mb), folders_(window::folders()),
      copy_{.id = build_id(folders_.exe_dir), .exe = exe_path(), .config = folders_.config},
      updates_(!options.ticks && copy_.id.built_for != Channel::None && !copy_.config.empty()),
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
Window::tick() {
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
                .can_save = !editor.drag.has_value()};
    }
    if (window::draw_menu(about_, menu) && !update::set_follow(copy_, about_.follow)) {
        std::fprintf(stderr, "loupe: cannot save the update setting in %s\n", utf8(copy_.config).c_str());
    }
    window::Start start;
    bool back = false;
    if (shown_) {
        back = draw_shown(*shown_, menu, &ok_);
    } else {
        window::draw_start(library_, refusal_, start);
        window::draw_attach(attach_);
    }
    ImGui::End();
    gpu_.end_frame();
    if (back) {
        leave();
    }
    if (leaving_ && (!shown_ || !shown_->recording->status().running)) {
        leaving_ = false;
        close();
    }
    list_boards();
    if (attach_.record) {
        (void)attach(*attach_.record);
        attach_.record.reset();
    }
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
    if (shown_ && shown_->recording && shown_->recording->status().running) {
        shown_->recording->stop();
        leaving_ = true;
        return;
    }
    close();
}

void
Window::list_boards() {
    if (attach_.refresh && !attach_.listing.valid()) {
        attach_.listing = std::async(std::launch::async, loupe::list_boards, scratch_.path() / "devices.log");
    }
    if (attach_.listing.valid() && attach_.listing.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        attach_.list = attach_.listing.get();
        attach_.listed = true;
    }
}

void
Window::close() {
    if (shown_) {
        // A recording joins the recent list once it holds bytes; one refused at once does not.
        std::error_code missing;
        if (shown_->recording && std::filesystem::file_size(shown_->recording->saved(), missing) > 0) {
            (void)remember(folders_, shown_->recording->saved());
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
    if (!scratch_.path().empty()) {
        frames = recording ? open_recording(run, samples_dir(folders_), utf8(options.scene.stem()), &why)
                           : open_scene(run, options.scene, &why);
    }
    if (frames == nullptr) {
        refusal_ = std::format("{}: {}", utf8(options.scene), why);
        std::fprintf(stderr, "loupe: %s\n", refusal_.c_str());
        return false;
    }
    refusal_.clear();
    (void)remember(folders_, options.scene);
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
Window::attach(const std::string& board) {
    close();
    std::string why = "no settings folder to keep the recording in";
    std::unique_ptr<Frames> frames;
    std::unique_ptr<window::Recording> recording;
    if (!folders_.config.empty() && !scratch_.path().empty()) {
        const std::filesystem::path file = recording_file(folders_.config, board);
        Options live;
        live.out_bmp = utf8(scratch_.path() / "frame.bmp");
        frames = open_recording(live, samples_dir(folders_), board, &why);
        recording =
            frames != nullptr ? window::Recording::start(autana_command({"-b", board, "record"}), file, &why) : nullptr;
    }
    if (recording == nullptr) {
        refusal_ = std::format("{}: {}", board, why);
        std::fprintf(stderr, "loupe: %s\n", refusal_.c_str());
        return false;
    }
    refusal_.clear();
    gpu_.set_title(std::format("{}: {} (recording)", TITLE, board));
    shown_.emplace(gpu_, std::move(frames));
    shown_->recording = std::move(recording);
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
    Window window(*gpu, options);
    const bool opened = options.scene.empty() || window.open(options);
    for (int tick = 0; (!options.ticks || tick < *options.ticks) && gpu->begin_frame(); tick++) {
        window.tick();
    }
    return window.finish() && opened;
}

} // namespace loupe
