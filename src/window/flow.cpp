#include "window/flow.hpp"

#include "imgui.h"

namespace loupe::window {

void
flow(float width) {
    // Past the last item the cursor starts the next line, so it and the room
    // left there span the panel's content, whatever its scrollbar.
    const float right = ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x;
    if (ImGui::GetItemRectMax().x + ImGui::GetStyle().ItemSpacing.x + width <= right) {
        ImGui::SameLine();
    }
}

float
button_width(const char* label) {
    return ImGui::CalcTextSize(label, nullptr, true).x + (ImGui::GetStyle().FramePadding.x * 2);
}

float
check_width(const char* label) {
    return ImGui::GetFrameHeight() + ImGui::GetStyle().ItemInnerSpacing.x + ImGui::CalcTextSize(label, nullptr, true).x;
}

float
text_width(const char* text) {
    return ImGui::CalcTextSize(text).x;
}

} // namespace loupe::window
