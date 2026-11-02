#include "window/panels.hpp"

#include <algorithm>
#include <cfloat>

#include "imgui.h"

namespace loupe::window {

void
draw_timeline(const Frames& frames, Timeline& timeline) {
    const int shown = frames.index() - 1;
    const int last = std::max(frames.count() - 1, Frames::FIRST_SEEKABLE);
    if (ImGui::Button(timeline.playing ? "Pause" : "Play")) {
        timeline.playing = !timeline.playing;
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(shown <= Frames::FIRST_SEEKABLE);
    if (ImGui::ArrowButton("back", ImGuiDir_Left)) {
        timeline.seek_to = shown - 1;
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::BeginDisabled(shown >= last);
    if (ImGui::ArrowButton("forward", ImGuiDir_Right)) {
        timeline.seek_to = shown + 1;
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    int picked = shown;
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::SliderInt("##frame", &picked, Frames::FIRST_SEEKABLE, last, "frame %d")) {
        timeline.seek_to = picked;
    }
}

bool
draw_view_picker(const std::vector<DebugView>& views, int* view) {
    const auto current = std::ranges::find(views, *view, &DebugView::id);
    bool changed = false;
    if (ImGui::BeginCombo("view", current == views.end() ? "" : current->name.c_str())) {
        for (const DebugView& row : views) {
            const bool selected = row.id == *view;
            if (ImGui::Selectable(row.name.c_str(), selected) && !selected) {
                *view = row.id;
                changed = true;
            }
        }
        ImGui::EndCombo();
    }
    return changed;
}

void
draw_frame(const FrameTexture& texture) {
    const ImVec2 room = ImGui::GetContentRegionAvail();
    const int zoom =
        std::max(1, std::min(static_cast<int>(room.x) / texture.width(), static_cast<int>(room.y) / texture.height()));
    const ImVec2 size(static_cast<float>(texture.width() * zoom), static_cast<float>(texture.height() * zoom));
    const ImVec2 at = ImGui::GetCursorPos();
    ImGui::SetCursorPos(
        ImVec2(at.x + std::max(0.0F, (room.x - size.x) / 2), at.y + std::max(0.0F, (room.y - size.y) / 2)));
    // Whole zoom, nearest sampling: each engine pixel stays a sharp square.
    ImDrawList* list = ImGui::GetWindowDrawList();
    list->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerNearest, nullptr);
    ImGui::Image(texture.ref(), size);
    list->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerLinear, nullptr);
}

} // namespace loupe::window
