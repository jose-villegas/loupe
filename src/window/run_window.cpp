#include "window/run_window.hpp"

#include <cstdio>
#include <filesystem>
#include <format>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include "engine/frames.hpp"
#include "imgui.h"
#include "library.hpp"
#include "process.hpp"
#include "window/files.hpp"
#include "window/frame_texture.hpp"
#include "window/gpu.hpp"
#include "window/panels.hpp"

namespace loupe {

namespace {

constexpr const char TITLE[] = "loupe";

// A window run's stand-in for -o. autana's close writes the last frame, to stdout
// without -o, and with it captures the engine's stdout beside the image
// (out.bmp.console). The window wants neither, so both go to a scratch file this
// removes, until autana can close a run without writing an image.
class ScratchImage {
  public:
    ScratchImage()
        : path_(std::filesystem::temp_directory_path() / std::format("loupe-{}.bmp", std::random_device{}())) {}

    ~ScratchImage() {
        std::error_code ignored;
        std::filesystem::remove(path_, ignored);
        std::filesystem::remove(path_.string() + ".console", ignored);
    }

    ScratchImage(const ScratchImage&) = delete;
    ScratchImage& operator=(const ScratchImage&) = delete;
    ScratchImage(ScratchImage&&) = delete;
    ScratchImage& operator=(ScratchImage&&) = delete;

    std::string
    path() const {
        return path_.string();
    }

  private:
    std::filesystem::path path_;
};

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
    // Opens `scene` with `options`' run in place of what the window showed, or
    // says on the start panel why it did not open.
    void switch_to(const std::filesystem::path& scene, const Options& options);
    // One tick's panels, then the scene they asked for; false once another
    // loupe took the window over.
    bool tick();
    // Closes the open scene; false when a run failed along the way.
    bool finish();

  private:
    // Closes the open scene, if any, back to the start panel.
    void close();
    // Starts loupe again on `scene`: autana keeps a run's asset packs mounted
    // after it closes, so only a process's first run is a fresh one.
    bool relaunch(const std::filesystem::path& scene);

    window::Gpu& gpu_;
    ScratchImage scratch_;
    // A scene picked on the start panel opens with the provider's defaults, as
    // its command-line flags were for another scene.
    Options picked_;
    Folders folders_;
    Library library_;
    std::string refusal_;
    std::optional<Shown> shown_;
    bool opened_ = false; // a scene was opened in this process, or tried
    bool ok_ = true;
};

Window::Window(window::Gpu& gpu, const Options& options)
    : gpu_(gpu), folders_(window::folders()), library_(library(folders_)) {
    picked_.checkpoint_mb = options.checkpoint_mb;
}

bool
Window::tick() {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::Begin(TITLE, nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
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
    const std::optional<std::filesystem::path> scene = start.picked ? start.picked : window::browsed();
    if (scene && opened_) {
        return !relaunch(*scene);
    }
    if (scene) {
        switch_to(*scene, picked_);
    }
    return true;
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

void
Window::switch_to(const std::filesystem::path& scene, const Options& options) {
    close();
    opened_ = true;
    Options run = options;
    run.out_bmp = scratch_.path();
    std::string why;
    std::unique_ptr<Frames> frames = open_scene(run, scene, &why);
    if (frames == nullptr) {
        refusal_ = std::format("{}: {}", utf8(scene), why);
        std::fprintf(stderr, "loupe: %s\n", refusal_.c_str());
        ok_ = false;
        return;
    }
    refusal_.clear();
    (void)remember(folders_, scene);
    gpu_.set_title(std::format("{}: {}", TITLE, frames->scene_id()));
    shown_.emplace(gpu_, std::move(frames));
}

bool
Window::relaunch(const std::filesystem::path& scene) {
    if (!spawn(window_args(exe_path(), scene, picked_.checkpoint_mb))) {
        refusal_ = std::format("{}: loupe could not start again on it", utf8(scene));
        return false;
    }
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
    if (!options.scene.empty()) {
        window.switch_to(options.scene, options);
    }
    for (int tick = 0; (!options.ticks || tick < *options.ticks) && gpu->begin_frame() && window.tick(); tick++) {}
    return window.finish();
}

} // namespace loupe
