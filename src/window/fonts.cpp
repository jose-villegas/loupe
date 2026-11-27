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

ImFont*
add(const unsigned char* otf, int size) {
    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    // ImGui only reads the bytes; it takes them as writable.
    return ImGui::GetIO().Fonts->AddFontFromMemoryTTF(const_cast<unsigned char*>(otf), size, PIXELS, &config);
}
#endif

// The Mono role's face; null without one of its own, and the default face draws it.
ImFont* mono = nullptr;

} // namespace

void
load_fonts() {
#if LOUPE_LATIN_MODERN
    add(SANS_OTF, sizeof SANS_OTF);
    mono = add(MONO_OTF, sizeof MONO_OTF);
#else
    mono = nullptr;
#endif
}

FontScope::FontScope(FontRole role) {
    ImFont* const text = ImGui::GetIO().Fonts->Fonts[0];
    ImGui::PushFont(role == FontRole::Mono && mono != nullptr ? mono : text, 0.0F);
}

FontScope::~FontScope() { ImGui::PopFont(); }

} // namespace loupe::window
