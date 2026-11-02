#include "window/run_window.hpp"

#include <cstdio>
#include <filesystem>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "engine/frames.hpp"
#include "imgui.h"
#include "library.hpp"
#include "paths.hpp"
#include "scratch.hpp"
#include "window/files.hpp"
#include "window/frame_texture.hpp"
#include "window/gpu.hpp"
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
    std::unique_ptr<Frames> frames = Frames::open(run, why);
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
    std::vector<DebugView> views;

    Shown(window::Gpu& gpu, std::unique_ptr<Frames> opened)
        : frames(std::move(opened)), texture(gpu, frames->frame()),
          views(frames->has_views() ? debug_views() : std::vector<DebugView>{}) {}
};

// Applies one of what the panels asked for this tick, a picked view first; true
// when the frame on screen changed. A failure clears `ok`, and a failed step stops play.
bool
apply(Frames& frames, window::Timeline& timeline, const int* picked_view, bool* ok) {
    if (picked_view != nullptr) {
        // Draws the shown frame again under the view, playing or not.
        *ok &= frames.set_view(*picked_view);
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
    const bool picked = !shown.views.empty() && window::draw_view_picker(shown.views, &view);
    window::draw_timeline(*shown.frames, shown.timeline);
    if (apply(*shown.frames, shown.timeline, picked ? &view : nullptr, ok)) {
        shown.texture.upload(shown.frames->frame());
    }
    window::draw_frame(shown.texture);
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
    Library library_;
    std::string refusal_;
    std::optional<Shown> shown_;
    bool ok_ = true;
};

Window::Window(window::Gpu& gpu, const Options& options)
    : gpu_(gpu), checkpoint_mb_(options.checkpoint_mb), folders_(window::folders()), library_(library(folders_)) {}

void
Window::tick() {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::Begin(TITLE, nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);
    window::draw_menu();
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
    std::string why;
    std::unique_ptr<Frames> frames = open_scene(run, options.scene, &why);
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
