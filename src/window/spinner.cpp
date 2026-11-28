#include "window/spinner.hpp"

#include <chrono>

namespace loupe::window {

namespace {

// UTF-8's three-byte form for U+2800 to U+28FF: E2, then A0 plus the cell's
// top two bits, then 80 plus its low six.
constexpr uint8_t LEAD = 0xE2;
constexpr uint8_t SECOND = 0xA0;
constexpr uint8_t CONTINUATION = 0x80;
constexpr unsigned LOW_BITS = 6;
constexpr uint8_t LOW_MASK = 0x3F;

} // namespace

std::string
spinner_frame(int64_t ms) {
    const uint8_t cell = SPINNER_CELLS.at(static_cast<size_t>(ms / SPINNER_FRAME_MS) % SPINNER_CELLS.size());
    return {static_cast<char>(LEAD), static_cast<char>(SECOND | (cell >> LOW_BITS)),
            static_cast<char>(CONTINUATION | (cell & LOW_MASK))};
}

std::string
spinning(std::string_view text) {
    const auto now = std::chrono::steady_clock::now().time_since_epoch();
    return spinner_frame(std::chrono::duration_cast<std::chrono::milliseconds>(now).count()) + " " + std::string(text);
}

} // namespace loupe::window
