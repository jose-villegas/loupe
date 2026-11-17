#include "window/flow.hpp"

#include "imgui.h"
#include "imgui_internal.h"

namespace loupe::window {

void
flow(float width) {
    // Past the last item the cursor starts the next line, so it and the room
    // left there span the panel's content, whatever its scrollbar. SameLine
    // puts the next item where the last one ended (CursorPosPrevLine), which
    // for a wrapped or framed item is not its rectangle's right edge.
    const float right = ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x;
    const float beside = ImGui::GetCurrentWindow()->DC.CursorPosPrevLine.x + ImGui::GetStyle().ItemSpacing.x;
    if (beside + width <= right) {
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

void
hint(const char* text) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
    ImGui::TextWrapped("%s", text);
    ImGui::PopStyleColor();
}

} // namespace loupe::window
