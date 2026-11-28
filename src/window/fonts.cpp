#include "window/fonts.hpp"

#include "imgui.h"

namespace loupe::window {

namespace {

#if LOUPE_LATIN_MODERN
// Latin Modern Sans and Mono, assets/fonts, as CMake built them in.
constexpr unsigned char SANS_OTF[] = {
#include "window/lmsans10_regular.inc"
};
constexpr unsigned char MONO_OTF[] = {
#include "window/lmmono10_regular.inc"
};
// Their size in pixels at a scale of 1: Latin Modern's x-height is small, so
// this is above ImGui's own 13 to keep the text as tall.
constexpr float PIXELS = 17.0F;
#else
// ImGui's own font's size in pixels.
constexpr float PIXELS = 13.0F;
#endif
// DejaVu Sans's braille patterns, U+2800 to U+28FF, for the wait spinner: no
// face above has them. One advance for all, so text after one stays put.
constexpr unsigned char BRAILLE_TTF[] = {
#include "window/dejavusans_braille.inc"
};
constexpr ImWchar BRAILLE_RANGE[] = {0x2800, 0x28FF, 0};

// Adds the braille patterns to the face added last, at its size.
void
merge_braille() {
    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    config.MergeMode = true;
    config.GlyphRanges = BRAILLE_RANGE;
    // ImGui only reads the bytes; it takes them as writable.
    ImGui::GetIO().Fonts->AddFontFromMemoryTTF(const_cast<unsigned char*>(BRAILLE_TTF), sizeof BRAILLE_TTF, PIXELS,
                                               &config);
}

#if LOUPE_LATIN_MODERN
void
add(const unsigned char* otf, int size, bool whole_pixels) {
    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    // A monospaced face's columns on whole pixels, as a terminal's cells.
    config.PixelSnapH = whole_pixels;
    ImGui::GetIO().Fonts->AddFontFromMemoryTTF(const_cast<unsigned char*>(otf), size, PIXELS, &config);
    merge_braille();
}
#endif

} // namespace

// The atlas holds the faces in FontRole's order; a role without its own face draws in the first.
void
load_fonts() {
#if LOUPE_LATIN_MODERN
    add(SANS_OTF, sizeof SANS_OTF, false);
    add(MONO_OTF, sizeof MONO_OTF, true);
#else
    ImGui::GetIO().Fonts->AddFontDefault();
    merge_braille();
#endif
}

FontScope::FontScope(FontRole role) {
    const ImVector<ImFont*>& fonts = ImGui::GetIO().Fonts->Fonts;
    const int i = static_cast<int>(role);
    ImGui::PushFont(fonts[i < fonts.Size ? i : 0], 0.0F);
}

FontScope::~FontScope() { ImGui::PopFont(); }

} // namespace loupe::window
