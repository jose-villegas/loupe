// A wait's spinner: the braille dots spinner (U+280B ⠋ ⠙ ⠹ ⠸ ⠼ ⠴ ⠦ ⠧ ⠇ ⠏),
// drawn as dots in a slot of one width, so the label after it starts at
// the same x whichever frame shows. Drawn, not a glyph: the panels' faces
// have none in U+2800-28FF, and circles stay sharp at every scale.
#pragma once

#include <array>
#include <cstdint>

namespace loupe::window {

// The spinner's frames as braille cells: bit i raised is dot i + 1, as in
// U+2800 + the bits.
constexpr std::array<uint8_t, 10> SPINNER_CELLS = {0x0B, 0x19, 0x39, 0x38, 0x3C, 0x34, 0x26, 0x27, 0x07, 0x0F};
// How long each frame shows.
constexpr int64_t SPINNER_FRAME_MS = 80;

// The cell showing `ms` milliseconds into any clock.
uint8_t spinner_cell(int64_t ms);

// Where a spinner's dots go and where its label starts, from the line's top
// left; the same for every cell. The cell spans the label's capitals: its
// top dots' tops at `cap_top` and its bottom dots' bottoms on `baseline`,
// both down from the line's top, in a slot as wide as braille's shape takes.
struct SpinnerLayout {
    float slot = 0, label_x = 0, radius = 0;
    std::array<float, 2> columns{};
    std::array<float, 4> rows{};
};

SpinnerLayout spinner_layout(float cap_top, float baseline);

// The layout for the current font at its current size, from its "H".
SpinnerLayout spinner_layout_here();

// Dot `dot`'s (0 to 7, braille's dots 1 to 8) column and row in the cell.
struct DotPlace {
    int column = 0, row = 0;
};

DotPlace dot_place(int dot);

// The spinner, then `text` from the fixed label x, wrapped within the room
// left, dimmed when `muted`, as one item.
void spinner_text(const char* text, bool muted);
// How wide spinner_text() draws `text` on one line.
float spinner_text_width(const char* text);

} // namespace loupe::window
