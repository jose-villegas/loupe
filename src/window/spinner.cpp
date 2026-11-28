#include "window/spinner.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>

#include "imgui.h"

namespace loupe::window {

namespace {

// Shares of the cell's height, the capitals': a dot's radius, the slot's
// width (two columns as far apart as the rows, a margin each side), and the
// gap before the label.
constexpr float RADIUS_SHARE = 0.12F;
constexpr float SLOT_SHARE = 0.8F;
constexpr float GAP_SHARE = 0.5F;
// The smallest dot radius, in pixels: a smaller one fades out at small scales.
constexpr float MIN_RADIUS = 1.0F;
// The gaps between braille's four rows.
constexpr float ROW_GAPS = 3.0F;
// Braille's dots 1 to 8 by column and row: 1-3 down the left, 4-6 down the
// right, then 7 and 8 under them.
constexpr std::array<DotPlace, 8> DOTS = {{{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {0, 3}, {1, 3}}};
constexpr int DOT_COUNT = static_cast<int>(DOTS.size());
// The capital whose top and bottom the cell spans.
constexpr ImWchar CAPITAL = 'H';

int64_t
now_ms() {
    const auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

} // namespace

uint8_t
spinner_cell(int64_t ms) {
    return SPINNER_CELLS.at(static_cast<size_t>(ms / SPINNER_FRAME_MS) % SPINNER_CELLS.size());
}

SpinnerLayout
spinner_layout(float cap_top, float baseline) {
    const float height = baseline - cap_top;
    SpinnerLayout layout;
    layout.radius = std::max(height * RADIUS_SHARE, MIN_RADIUS);
    const float pitch = (height - (2 * layout.radius)) / ROW_GAPS;
    for (size_t i = 0; i < layout.rows.size(); ++i) {
        layout.rows.at(i) = cap_top + layout.radius + (static_cast<float>(i) * pitch);
    }
    // Whole pixels: the label's glyphs stay on the pixel grid.
    layout.slot = std::round(height * SLOT_SHARE);
    layout.columns = {(layout.slot - pitch) / 2, (layout.slot + pitch) / 2};
    layout.label_x = layout.slot + std::round(height * GAP_SHARE);
    return layout;
}

SpinnerLayout
spinner_layout_here() {
    const ImFontGlyph* capital = ImGui::GetFontBaked()->FindGlyph(CAPITAL);
    return spinner_layout(capital->Y0, capital->Y1);
}

DotPlace
dot_place(int dot) {
    return DOTS.at(static_cast<size_t>(dot));
}

void
spinner_text(const char* text, bool muted) {
    const SpinnerLayout layout = spinner_layout_here();
    const ImVec2 at = ImGui::GetCursorScreenPos();
    const ImU32 colour = ImGui::GetColorU32(muted ? ImGuiCol_TextDisabled : ImGuiCol_Text);
    ImDrawList* draw = ImGui::GetWindowDrawList();
    const uint8_t cell = spinner_cell(now_ms());
    for (int dot = 0; dot < DOT_COUNT; ++dot) {
        if ((cell & (1U << dot)) != 0) {
            const DotPlace place = dot_place(dot);
            draw->AddCircleFilled(ImVec2(at.x + layout.columns.at(place.column), at.y + layout.rows.at(place.row)),
                                  layout.radius, colour);
        }
    }
    ImGui::BeginGroup();
    ImGui::SetCursorScreenPos(ImVec2(at.x + layout.label_x, at.y));
    if (muted) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
    }
    ImGui::TextWrapped("%s", text);
    if (muted) {
        ImGui::PopStyleColor();
    }
    ImGui::EndGroup();
}

float
spinner_text_width(const char* text) {
    return spinner_layout_here().label_x + ImGui::CalcTextSize(text).x;
}

} // namespace loupe::window
