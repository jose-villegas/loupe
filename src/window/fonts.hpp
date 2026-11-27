// The panels' typefaces by role: Text for words, Mono where values must line
// up (numbers, hashes, columns). A panel asks for a role, never an ImFont.
#pragma once

#include <cstdint>

namespace loupe::window {

enum class FontRole : std::uint8_t { Text, Mono };

// Adds both roles' faces to ImGui's atlas, Text first as the default: after
// each ImGui::CreateContext. Without LOUPE_LATIN_MODERN, ImGui's own font
// draws both.
void load_fonts();

// Draws in `role`'s face, at the current size, while it lives.
class FontScope {
  public:
    explicit FontScope(FontRole role);
    ~FontScope();
    FontScope(const FontScope&) = delete;
    FontScope& operator=(const FontScope&) = delete;
    FontScope(FontScope&&) = delete;
    FontScope& operator=(FontScope&&) = delete;
};

} // namespace loupe::window
