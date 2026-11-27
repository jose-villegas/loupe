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

void
add(const unsigned char* otf, int size) {
    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    // ImGui only reads the bytes; it takes them as writable.
    ImGui::GetIO().Fonts->AddFontFromMemoryTTF(const_cast<unsigned char*>(otf), size, PIXELS, &config);
}
#endif

} // namespace

// The atlas holds the faces in FontRole's order; a role without its own face draws in the first.
void
load_fonts() {
#if LOUPE_LATIN_MODERN
    add(SANS_OTF, sizeof SANS_OTF);
    add(MONO_OTF, sizeof MONO_OTF);
#endif
}

FontScope::FontScope(FontRole role) {
    const ImVector<ImFont*>& fonts = ImGui::GetIO().Fonts->Fonts;
    const int i = static_cast<int>(role);
    ImGui::PushFont(fonts[i < fonts.Size ? i : 0], 0.0F);
}

FontScope::~FontScope() { ImGui::PopFont(); }

} // namespace loupe::window
