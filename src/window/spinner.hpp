// A wait's spinner: the braille dots spinner (U+280B ⠋ ⠙ ⠹ ⠸ ⠼ ⠴ ⠦ ⠧ ⠇ ⠏),
// a glyph at the head of the label's own text, so it shares the label's
// size and baseline. Every frame has one advance (fonts.cpp's braille), so
// the words after it stay put.
#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace loupe::window {

// The spinner's frames as braille cells: bit i raised is dot i + 1, as in
// U+2800 + the bits.
constexpr std::array<uint8_t, 10> SPINNER_CELLS = {0x0B, 0x19, 0x39, 0x38, 0x3C, 0x34, 0x26, 0x27, 0x07, 0x0F};
// How long each frame shows.
constexpr int64_t SPINNER_FRAME_MS = 80;

// The frame showing `ms` milliseconds into any clock, as its UTF-8 glyph.
std::string spinner_frame(int64_t ms);
// `text` after the frame showing now and a space.
std::string spinning(std::string_view text);

} // namespace loupe::window
