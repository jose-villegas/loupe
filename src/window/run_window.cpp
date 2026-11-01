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

// Applies what the panels asked for this tick; true when the frame on screen changed.
// A failed step or seek clears `ok`.
bool
apply(Frames& frames, window::Timeline& timeline, const int* picked_view, bool* ok) {
    bool changed = false;
    if (picked_view != nullptr) {
        // Draws the shown frame again under the view, playing or not.
        frames.set_view(*picked_view);
        changed = true;
    }
    if (timeline.seek_to) {
        *ok &= frames.seek(*timeline.seek_to);
        timeline.seek_to.reset();
        return true;
    }
    if (timeline.playing) {
        if (frames.index() < frames.count()) {
            *ok &= frames.step(frames.dt_ms());
            return true;
        }
        timeline.playing = false;
    }
    return changed;
}

} // namespace

bool
run_window(Frames& frames, const Options& options) {
    std::string why;
    const std::string title = std::format("loupe: {}", scene_id(options.scene_toml));
    const std::unique_ptr<window::Gpu> gpu = window::Gpu::open(title.c_str(), &why);
    if (gpu == nullptr) {
        std::fprintf(stderr, "loupe: %s\n", why.c_str());
        return false;
    }
    frames.keep_checkpoints(options.checkpoint_mb * MIB);
    bool ok = frames.step(frames.dt_ms());
    window::FrameTexture texture(*gpu);
    texture.upload(frames.frame());
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
