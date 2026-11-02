#include "window/run_window.hpp"

#include <cstdio>
#include <format>
#include <memory>
#include <string>
#include <vector>

#include "imgui.h"
#include "window/frame_texture.hpp"
#include "window/gpu.hpp"
#include "window/panels.hpp"

namespace loupe {

namespace {

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

} // namespace

bool
run_window(Frames& frames, const Options& options) {
    if (frames.count() <= Frames::FIRST_SEEKABLE) {
        std::fprintf(stderr, "loupe: the window needs a run of more than %d frames\n", Frames::FIRST_SEEKABLE);
        return false;
    }
    std::string why;
    const std::string title = std::format("loupe: {}", scene_id(options.scene_toml));
    const std::unique_ptr<window::Gpu> gpu = window::Gpu::open(title.c_str(), &why);
    if (gpu == nullptr) {
        std::fprintf(stderr, "loupe: %s\n", why.c_str());
        return false;
    }
    frames.keep_checkpoints(options.checkpoint_mb.value_or(DEFAULT_CHECKPOINT_MB) * MIB);
    // Opens on the first frame a seek reaches, so every control works from the start.
    bool ok = true;
    while (frames.index() <= Frames::FIRST_SEEKABLE) {
        ok &= frames.step(frames.dt_ms());
    }
    window::FrameTexture texture(*gpu, frames.frame());
    window::Timeline timeline;
    const std::vector<DebugView> views = frames.has_views() ? debug_views() : std::vector<DebugView>{};
    for (int tick = 0; (!options.ticks || tick < *options.ticks) && gpu->begin_frame(); tick++) {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::Begin("loupe", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        int view = frames.view();
        const bool picked = !views.empty() && window::draw_view_picker(views, &view);
        window::draw_timeline(frames, timeline);
        if (apply(frames, timeline, picked ? &view : nullptr, &ok)) {
            texture.upload(frames.frame());
        }
        window::draw_frame(texture);
        ImGui::End();
        gpu->end_frame();
    }
    return ok;
}

} // namespace loupe
