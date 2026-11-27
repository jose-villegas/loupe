#include "window/spinner.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>

#include "imgui.h"

namespace loupe::window {

namespace {

// The cell's share of the text height: its width, the gap after it, a dot's
// radius, and its two columns' and four rows' centres, as a braille glyph
// sits in a monospace face.
constexpr float SLOT_SHARE = 0.6F;
constexpr float GAP_SHARE = 0.4F;
constexpr float RADIUS_SHARE = 0.09F;
// The smallest dot radius, in pixels: a smaller one fades out at small scales.
constexpr float MIN_RADIUS = 1.0F;
constexpr std::array<float, 2> COLUMN_SHARES = {0.32F, 0.68F};
constexpr std::array<float, 4> ROW_SHARES = {0.2F, 0.4F, 0.6F, 0.8F};
// Braille's dots 1 to 8 by column and row: 1-3 down the left, 4-6 down the
// right, then 7 and 8 under them.
constexpr std::array<DotPlace, 8> DOTS = {{{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {0, 3}, {1, 3}}};
constexpr int DOT_COUNT = static_cast<int>(DOTS.size());

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
spinner_layout(float size) {
    SpinnerLayout layout;
    layout.slot = std::round(size * SLOT_SHARE);
    // Whole pixels: the label's glyphs stay on the pixel grid.
    layout.label_x = layout.slot + std::round(size * GAP_SHARE);
    layout.radius = std::max(size * RADIUS_SHARE, MIN_RADIUS);
    for (size_t i = 0; i < layout.columns.size(); ++i) {
        layout.columns.at(i) = layout.slot * COLUMN_SHARES.at(i);
    }
    for (size_t i = 0; i < layout.rows.size(); ++i) {
        layout.rows.at(i) = size * ROW_SHARES.at(i);
    }
    return layout;
}

DotPlace
dot_place(int dot) {
    return DOTS.at(static_cast<size_t>(dot));
}

void
spinner_text(const char* text, bool muted) {
    const SpinnerLayout layout = spinner_layout(ImGui::GetTextLineHeight());
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
    return spinner_layout(ImGui::GetTextLineHeight()).label_x + ImGui::CalcTextSize(text).x;
}

} // namespace loupe::window
