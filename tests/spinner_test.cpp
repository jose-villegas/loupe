// The wait spinner: its frames are the braille dots spinner's glyphs; the
// panels' faces, both roles, draw every frame with a glyph of their own, not
// the fallback, at one advance; so the label after a frame starts at one x
// whichever frame shows, at every size and DPI scale.
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
// U+280B ⠋ ⠙ ⠹ ⠸ ⠼ ⠴ ⠦ ⠧ ⠇ ⠏, the frames, in UTF-8.
constexpr std::array<const char*, 10> BRAILLE = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
constexpr std::array<ImWchar, 10> CODE_POINTS = {0x280B, 0x2819, 0x2839, 0x2838, 0x283C,
                                                 0x2834, 0x2826, 0x2827, 0x2807, 0x280F};
// Font sizes before scaling, and the DPI scales a monitor gives.
constexpr std::array<float, 3> SIZES = {13.0F, 16.0F, 20.0F};
constexpr std::array<float, 4> DPI_SCALES = {1.0F, 1.25F, 1.5F, 2.0F};
constexpr std::array<loupe::window::FontRole, 2> ROLES = {loupe::window::FontRole::Text, loupe::window::FontRole::Mono};
// A label, and the frame time of frame i.
constexpr const char LABEL[] = "readying the live view (3 s)";

constexpr int64_t
frame_ms(size_t i) {
    return static_cast<int64_t>(i) * loupe::window::SPINNER_FRAME_MS;
}

// A display big enough for a frame; its size does not matter.
constexpr float DISPLAY = 512.0F;
constexpr float FRAME_S = 1.0F / 60;

bool
frames_are_the_braille_spinner() {
    bool ok = true;
    std::set<std::string> seen;
    for (size_t i = 0; i < BRAILLE.size(); ++i) {
        const std::string frame = loupe::window::spinner_frame(frame_ms(i));
        ok &= check(frame == BRAILLE.at(i), std::format("frame {} is {}", i, BRAILLE.at(i)));
        seen.insert(frame);
    }
    ok &= check(seen.size() == BRAILLE.size(), "every frame differs");
    ok &= check(loupe::window::spinner_frame(frame_ms(BRAILLE.size())) == BRAILLE[0],
                "after the last frame the first comes again");
    return ok;
}

// In the current font at its current size: every frame is a glyph of its
// own, all at one advance, and the label after it ends at one x.
bool
label_stays_put(const std::string& at) {
    ImFontBaked* baked = ImGui::GetFontBaked();
    bool ok = true;
    std::set<float> advances;
    std::set<float> label_ends;
    for (size_t i = 0; i < CODE_POINTS.size(); ++i) {
        ok &= check(baked->FindGlyphNoFallback(CODE_POINTS.at(i)) != nullptr,
                    std::format("{}: frame {} has a glyph", at, i));
        advances.insert(ImGui::CalcTextSize(BRAILLE.at(i)).x);
        label_ends.insert(ImGui::CalcTextSize((std::string(BRAILLE.at(i)) + " " + LABEL).c_str()).x);
    }
    ok &= check(advances.size() == 1 && *advances.begin() > 0,
                std::format("{}: every frame advances {:.2f}", at, *advances.begin()));
    ok &= check(label_ends.size() == 1, std::format("{}: the label after any frame ends at one x", at));
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
        for (const loupe::window::FontRole role : ROLES) {
            const loupe::window::FontScope face(role);
            for (const float size : SIZES) {
                ImGui::PushFont(nullptr, size);
                ok &=
                    label_stays_put(std::format("role {} size {} at DPI scale {}", static_cast<int>(role), size, dpi));
                ImGui::PopFont();
            }
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
