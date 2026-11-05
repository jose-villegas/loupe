#include "window/run_window.hpp"

#include <cstdio>
#include <filesystem>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "engine/frames.hpp"
#include "imgui.h"
#include "library.hpp"
#include "paths.hpp"
#include "process.hpp"
#include "scratch.hpp"
#include "update/update.hpp"
#include "version.hpp"
#include "window/files.hpp"
#include "window/frame_texture.hpp"
#include "window/gpu.hpp"
#include "window/observer.hpp"
#include "window/panels.hpp"

namespace loupe {

namespace {

constexpr const char TITLE[] = "loupe";

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
        *ok &= frames.observe(observer.on ? std::optional(observer.pose) : std::nullopt);
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

// The open scene's tick: Scenes, the view picker, the timeline and the frame;
// true when Scenes was pressed, to go back to the start panel.
bool
draw_shown(Shown& shown, bool* ok) {
    const bool back = ImGui::Button("Scenes");
    ImGui::SameLine();
    int view = shown.frames->view();
    Asked asked;
    if (!shown.views.empty() && window::draw_view_picker(shown.views, &view)) {
        asked.view = view;
    }
    ImGui::SameLine();
    const bool was_on = shown.observer.on;
    window::draw_observer_toggle(*shown.frames, shown.observer);
    ImGui::BeginDisabled(shown.observer.on);
    window::draw_timeline(*shown.frames, shown.timeline);
    ImGui::EndDisabled();
    window::draw_frame(shown.texture);
    const bool moved = shown.observer.on && window::drive_observer(shown.observer, ImGui::GetIO().DeltaTime);
    asked.observe = moved || shown.observer.on != was_on;
    // Uploaded after the frame is drawn, for the observer's input over it; still this tick's picture.
    if (apply(*shown.frames, shown.timeline, shown.observer, asked, ok)) {
        shown.texture.upload(shown.frames->frame());
    }
    return back;
}

// The window's state between ticks: the open scene, or the start panel's.
class Window {
  public:
    Window(window::Gpu& gpu, const Options& options);
    // Opens `options`' scene in place of the open one; false, said on the
    // start panel, when it did not open.
    bool open(const Options& options);
    // One tick's panels, then the scene they asked for.
    void tick();
    // Closes the open scene; false when a run failed along the way.
    bool finish();

  private:
    // Closes the open scene, if any, back to the start panel.
    void close();

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
    std::string refusal_;
    std::optional<Shown> shown_;
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
    ImGui::Begin(TITLE, nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);
    if (window::draw_menu(about_) && !update::set_follow(copy_, about_.follow)) {
        std::fprintf(stderr, "loupe: cannot save the update setting in %s\n", utf8(copy_.config).c_str());
    }
    window::Start start;
    bool back = false;
    if (shown_) {
        back = draw_shown(*shown_, &ok_);
    } else {
        window::draw_start(library_, refusal_, start);
    }
    ImGui::End();
    gpu_.end_frame();
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
    std::unique_ptr<Frames> frames = scratch_.path().empty() ? nullptr : open_scene(run, options.scene, &why);
    if (frames == nullptr) {
        refusal_ = std::format("{}: {}", utf8(options.scene), why);
        std::fprintf(stderr, "loupe: %s\n", refusal_.c_str());
        return false;
    }
    refusal_.clear();
    (void)remember(folders_, options.scene);
    gpu_.set_title(std::format("{}: {}", TITLE, frames->scene_id()));
    shown_.emplace(gpu_, std::move(frames));
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
