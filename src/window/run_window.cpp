#include "window/run_window.hpp"

#include <filesystem>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>

#include "engine/frames.hpp"
#include "imgui.h"
#include "imgui_internal.h"
// After imgui.h, whose types its header uses without including it.
#include "ImGuizmo.h"
#include "library.hpp"
#include "log/log.hpp"
#include "paths.hpp"
#include "process.hpp"
#include "scratch.hpp"
#include "tools.hpp"
#include "update/update.hpp"
#include "version.hpp"
#include "window/console.hpp"
#include "window/editor.hpp"
#include "window/files.hpp"
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
// The scratch frame autana's close writes (Window::scratch_), and the suffix
// of the engine's stdout beside it, which the log reads as ENGINE's lines.

constexpr const char FRAME_BMP[] = "frame.bmp";
constexpr const char ENGINE_OUT_SUFFIX[] = ".console";
constexpr const char ENGINE[] = "autana";
// Where an unscripted window keeps its log, in the settings folder.
constexpr const char LOG_FILE[] = "loupe.log";
// The Console's size when it first opens over the start panel, undocked.
constexpr ImVec2 CONSOLE_SIZE(720, 280);
// Each side panel's share of the window: outliner, inspector, frame debugger.
constexpr float OUTLINER_SHARE = 0.18F;
constexpr float INSPECTOR_SHARE = 0.26F;
constexpr float DEBUGGER_SHARE = 0.16F;
// The Console's share of the viewport's height, when it opens under it.
constexpr float CONSOLE_SHARE = 0.35F;

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

// The open scene's run and what the window keeps for it.
struct Shown {
    std::unique_ptr<Frames> frames;
    window::FrameTexture texture;
    window::Timeline timeline;
    window::Observer observer;
    std::vector<DebugView> views;
    // The editor over the scene file; none for a pack, or without the pack tool, `editor_why` saying why.
    std::optional<window::Editor> editor;
    std::string editor_why;
    // The viewport's dock node, which the Console splits when it opens; none before lay_out().
    ImGuiID viewport_node = 0;
    bool console_placed = false;

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
        timeline.playing = false; // the run's last frame
        return false;
    }
    if (!frames.step(frames.dt_ms())) {
        *ok = false;
        timeline.playing = false;
    }
    return true;
}

// Docks the panels into `dock`, `size` large: the outliner left, the
// inspector right, the frame debugger below, the viewport in the middle,
// whose node it gives back.
ImGuiID
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
    return middle;
}

// Docks the Console below the viewport in `viewport_node`, CONSOLE_SHARE of
// it; nothing when that node is gone or split, the panels rearranged since,
// and the Console floats.
void
place_console(ImGuiID viewport_node) {
    const ImGuiDockNode* node = ImGui::DockBuilderGetNode(viewport_node);
    if (node == nullptr || !node->IsLeafNode()) {
        return;
    }

    ImGuiID viewport = viewport_node;
    const ImGuiID console = ImGui::DockBuilderSplitNode(viewport, ImGuiDir_Down, CONSOLE_SHARE, nullptr, &viewport);
    ImGui::DockBuilderDockWindow(window::CONSOLE, console);
    ImGui::DockBuilderFinish(viewport_node);
}

// The frame debugger: Scenes, the view picker, the observer and the timeline.
// True when Scenes was pressed; a picked view goes into `asked`.
bool
draw_debugger(Shown& shown, Asked& asked) {
    const bool back = ImGui::Button("Scenes");
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

// The open scene's tick: its docked panels, then what they and the Edit menu
// asked; true when Scenes was pressed, to go back to the start panel.
bool
draw_shown(Shown& shown, const window::EditMenu& menu, bool* ok) {
    const ImGuiID dock = ImGui::GetID("panels");
    if (shown.viewport_node == 0) {
        shown.viewport_node = lay_out(dock, ImGui::GetContentRegionAvail());
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
    // Uploaded after the frame is drawn, for the observer's input over it; still this tick's picture.
    bool changed = apply(*shown.frames, shown.timeline, shown.observer, asked, ok);
    if (shown.editor) {
        // WASD flies the observer or the free view; W, E and R pick the handle otherwise.
        window::EditAsk ask = window::edit_keys(*shown.editor, !shown.observer.on && !shown.editor->path.free.on);
        ask.undo |= menu.undo;
        ask.redo |= menu.redo;
        ask.save |= menu.save;
        window::apply(*shown.editor, ask);
        if (!shown.observer.on) {
            const int64_t drawn = shown.frames->drawn();
            *ok &= window::sync(*shown.editor, [&shown](const std::string& object, const transformf_t& local) {
                return shown.frames->place(object, local);
            });
            *ok &= window::sync_paths(
                *shown.editor,
                [&shown](const std::string& camera, Frames::PoseAt pose) {
                    return shown.frames->fly(camera, std::move(pose));
                },
                [&shown](const std::string& camera) { return shown.frames->land(camera); });
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
    // A `scripted` window (--ticks, a recipe) neither updates nor remembers what it opened.
    Window(window::Gpu& gpu, const Options& options, bool scripted);
    // Opens `options`' scene in place of the open one; false, said on the
    // start panel, when it did not open.
    bool open(const Options& options);
    // One tick's panels, then the scene they asked for; with `picture`, also
    // what the tick drew, which stays empty when it could not be read back.
    void tick(std::optional<window::Picture>* picture = nullptr);
    // Closes the open scene; false when a run failed along the way.
    bool finish();

  private:
    // Closes the open scene, if any, back to the start panel.
    void close();
    // The Console, when open: under the viewport when it opens over a scene, over the start panel otherwise.
    void draw_console();

    window::Gpu& gpu_;
    // A window run's stand-in for -o: autana's close writes the last frame, to
    // stdout without -o, and with it the engine's stdout beside the image
    // (out.bmp.console). Both go here: the image unwanted, until autana can
    // close a run without writing one, the engine's stdout read into the log.
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
    std::string refusal_;
    std::optional<Shown> shown_;
    // The open scene's engine output, read into the log each tick.
    std::optional<log::Tail> engine_out_;
    window::Console console_;
    bool ok_ = true;
};

Window::Window(window::Gpu& gpu, const Options& options, bool scripted)
    : gpu_(gpu), checkpoint_mb_(options.checkpoint_mb), folders_(window::folders()),
      copy_{.id = build_id(folders_.exe_dir), .exe = exe_path(), .config = folders_.config}, scripted_(scripted),
      updates_(!scripted_ && copy_.id.built_for != Channel::None && !copy_.config.empty()),
      library_(library(folders_)) {
    if (!scripted_ && !copy_.config.empty() && !log::sink().open_file(copy_.config / LOG_FILE)) {
        log::warn(std::format("cannot write the log in {}", utf8(copy_.config / LOG_FILE)));
    }
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
    if (window::draw_menu(about_, menu, &console_.open) && !update::set_follow(copy_, about_.follow)) {
        log::error(std::format("cannot save the update setting in {}", utf8(copy_.config)));
    }
    window::Start start;
    bool back = false;
    if (shown_) {
        back = draw_shown(*shown_, menu, &ok_);
    } else {
        window::draw_start(library_, refusal_, start);
    }
    ImGui::End();
    draw_console();
    if (picture != nullptr) {
        std::string why;
        *picture = gpu_.end_frame_read_back(&why);
        if (!*picture) {
            log::error(std::format("cannot read the window back: {}", why));
        }
    } else {
        gpu_.end_frame();
    }
    if (back) {
        close();
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

void
Window::draw_console() {
    if (engine_out_) {
        engine_out_->poll();
    }
    window::pull(console_);
    if (!console_.open) {
        if (shown_) {
            shown_->console_placed = false;
        }
        return;
    }
    // Each time it opens over a scene, the Console docks under the viewport; moved after, it stays.
    if (shown_ && shown_->viewport_node != 0 && !shown_->console_placed) {
        place_console(shown_->viewport_node);
        shown_->console_placed = true;
    }

    ImGui::SetNextWindowSize(CONSOLE_SIZE, ImGuiCond_FirstUseEver);
    if (ImGui::Begin(window::CONSOLE, &console_.open)) {
        window::draw_console(console_);
    }
    ImGui::End();
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
Window::close() {
    if (shown_) {
        ok_ &= shown_->frames->close();
        if (engine_out_) {
            engine_out_->poll(true);
            engine_out_.reset();
        }
        log::info(std::format("closed {}", shown_->frames->scene_id()));
        shown_.reset();
        gpu_.set_title(TITLE);
        library_ = library(folders_);
    }
}

bool
Window::open(const Options& options) {
    close();
    Options run = options;
    run.out_bmp = utf8(scratch_.path() / FRAME_BMP);
    std::string why = "cannot make a scratch folder for its frames";
    std::unique_ptr<Frames> frames = scratch_.path().empty() ? nullptr : open_scene(run, options.scene, &why);
    if (frames == nullptr) {
        refusal_ = std::format("{}: {}", utf8(options.scene), why);
        log::error(refusal_);
        return false;
    }
    refusal_.clear();
    log::info(std::format("opened {} from {}", frames->scene_id(), utf8(options.scene)));
    engine_out_.emplace(from_utf8(run.out_bmp + ENGINE_OUT_SUFFIX), ENGINE);

    if (!scripted_) {
        (void)remember(folders_, options.scene);
    }
    gpu_.set_title(std::format("{}: {}", TITLE, frames->scene_id()));
    shown_.emplace(gpu_, std::move(frames));
    shown_->editor = window::open_editor(options.scene, &shown_->editor_why);
    return true;
}

} // namespace

bool
run_window(const Options& options) {
    std::string why;
    const log::StderrCapture captured;
    const std::unique_ptr<window::Gpu> gpu = window::Gpu::open(TITLE, &why);
    if (gpu == nullptr) {
        log::error(why);
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
    const log::StderrCapture captured;
    const std::unique_ptr<window::Gpu> gpu = window::Gpu::open(TITLE, &why, content_scale);
    if (gpu == nullptr) {
        log::error(why);
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
