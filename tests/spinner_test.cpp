// The wait spinner: the braille dots spinner's frames; the cell spanning the
// label's capitals, its top dots' tops at "H"'s top and its bottom dots'
// bottoms on "H"'s bottom, in the panels' own faces at every size and DPI
// scale; every dot inside the slot; the label at one x whichever frame shows.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <format>
#include <set>
#include <string>

#include "checks.hpp"
#include "imgui.h"
#include "window/fonts.hpp"
#include "window/spinner.hpp"

namespace {

constexpr const char TEST[] = "spinner_test";
constexpr loupe::test::Checks check{TEST};
// U+280B ⠋ ⠙ ⠹ ⠸ ⠼ ⠴ ⠦ ⠧ ⠇ ⠏, the frames as code points.
constexpr std::array<uint32_t, 10> BRAILLE = {0x280B, 0x2819, 0x2839, 0x2838, 0x283C,
                                              0x2834, 0x2826, 0x2827, 0x2807, 0x280F};
// Font sizes before scaling, and the DPI scales a monitor gives.
constexpr std::array<float, 3> SIZES = {13.0F, 16.0F, 20.0F};
constexpr std::array<float, 4> DPI_SCALES = {1.0F, 1.25F, 1.5F, 2.0F};
constexpr int DOTS = 8;
// How far apart two positions may be and count as the same, in pixels.
constexpr float SAME = 0.001F;
// A display big enough for a frame; its size does not matter.
constexpr float DISPLAY = 512.0F;
constexpr float FRAME_S = 1.0F / 60;

using loupe::window::SPINNER_CELLS;
using loupe::window::SPINNER_FRAME_MS;

bool
frames_are_the_braille_spinner() {
    bool ok = true;
    std::set<uint8_t> seen;
    for (size_t i = 0; i < BRAILLE.size(); ++i) {
        const uint8_t cell = loupe::window::spinner_cell(static_cast<int64_t>(i) * SPINNER_FRAME_MS);
        ok &= check(0x2800U + cell == BRAILLE.at(i), std::format("frame {} is U+{:04X}", i, BRAILLE.at(i)));
        seen.insert(cell);
    }
    ok &= check(seen.size() == BRAILLE.size(), "every frame differs");
    ok &=
        check(loupe::window::spinner_cell(static_cast<int64_t>(BRAILLE.size()) * SPINNER_FRAME_MS) == SPINNER_CELLS[0],
              "after the last frame the first comes again");
    return ok;
}

// In the current font at its current size: the cell's top and bottom are
// "H"'s, each frame's dots stay in the slot, and the label starts at one
// whole-pixel x.
bool
cell_spans_the_capitals(const std::string& at) {
    const ImFontGlyph* capital = ImGui::GetFontBaked()->FindGlyph('H');
    const loupe::window::SpinnerLayout layout = loupe::window::spinner_layout_here();
    bool ok = check(std::abs(layout.rows.front() - layout.radius - capital->Y0) < SAME
                        && std::abs(layout.rows.back() + layout.radius - capital->Y1) < SAME,
                    std::format("{}: the cell spans H, {:.2f} to {:.2f}", at, capital->Y0, capital->Y1));
    for (size_t i = 0; i < SPINNER_CELLS.size(); ++i) {
        float left = layout.slot;
        float right = 0;
        for (int dot = 0; dot < DOTS; ++dot) {
            if ((SPINNER_CELLS.at(i) & (1U << dot)) != 0) {
                const float x = layout.columns.at(loupe::window::dot_place(dot).column);
                left = std::min(left, x - layout.radius);
                right = std::max(right, x + layout.radius);
            }
        }
        ok &= check(left >= 0 && right <= layout.slot && layout.slot < layout.label_x
                        && loupe::window::spinner_layout_here().label_x == layout.label_x,
                    std::format("{}: frame {}'s dots stay in the slot, the label at x {}", at, i, layout.label_x));
    }
    ok &= check(layout.label_x == std::round(layout.label_x), std::format("{}: the label starts on a whole pixel", at));
    return ok;
}

bool
at_every_size() {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
    io.IniFilename = nullptr;
    io.DisplaySize = ImVec2(DISPLAY, DISPLAY);
    io.DeltaTime = FRAME_S;
    loupe::window::load_fonts();
    bool ok = true;
    for (const float dpi : DPI_SCALES) {
        ImGui::GetStyle().FontScaleDpi = dpi;
        ImGui::NewFrame();
        ImGui::Begin("spinner");
        for (const float size : SIZES) {
            ImGui::PushFont(nullptr, size);
            ok &= cell_spans_the_capitals(std::format("size {} at DPI scale {}", size, dpi));
            ImGui::PopFont();
        }
        ImGui::End();
        ImGui::EndFrame();
    }
    ImGui::DestroyContext();
    return ok;
}

} // namespace

int
main() {
    bool ok = frames_are_the_braille_spinner();
    ok &= at_every_size();
    std::fprintf(stderr, "%s: %s\n", TEST, ok ? "passed" : "FAILED");
    return ok ? 0 : 1;
}
