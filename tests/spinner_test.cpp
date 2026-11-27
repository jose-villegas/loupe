// The wait spinner: the braille dots spinner's frames, every dot inside the
// slot, and the label starting at one x whichever frame shows, at every
// panel scale.
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <format>
#include <set>

#include "checks.hpp"
#include "window/spinner.hpp"

namespace {

constexpr const char TEST[] = "spinner_test";
constexpr loupe::test::Checks check{TEST};
// U+280B ⠋ ⠙ ⠹ ⠸ ⠼ ⠴ ⠦ ⠧ ⠇ ⠏, the frames as code points.
constexpr std::array<uint32_t, 10> BRAILLE = {0x280B, 0x2819, 0x2839, 0x2838, 0x283C,
                                              0x2834, 0x2826, 0x2827, 0x2807, 0x280F};
// Text heights at the panels' scales, 1 to 3 at the default face, with a fraction.
constexpr std::array<float, 4> SIZES = {13.0F, 19.5F, 26.0F, 39.0F};
constexpr int DOTS = 8;

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

bool
label_stays_put(float size) {
    // The layout every frame draws with; the label x comes from it alone.
    const loupe::window::SpinnerLayout first = loupe::window::spinner_layout(size);
    bool ok = true;
    for (size_t i = 0; i < SPINNER_CELLS.size(); ++i) {
        const loupe::window::SpinnerLayout layout = loupe::window::spinner_layout(size);
        float right = 0;
        for (int dot = 0; dot < DOTS; ++dot) {
            if ((SPINNER_CELLS.at(i) & (1U << dot)) != 0) {
                right = std::max(right, layout.columns.at(loupe::window::dot_place(dot).column) + layout.radius);
            }
        }
        ok &=
            check(layout.label_x == first.label_x && right <= layout.slot && layout.slot < layout.label_x,
                  std::format("size {}: frame {}'s dots stay in the slot, the label at x {}", size, i, first.label_x));
    }
    ok &= check(first.label_x == static_cast<float>(static_cast<int>(first.label_x)),
                std::format("size {}: the label starts on a whole pixel", size));
    return ok;
}

} // namespace

int
main() {
    bool ok = frames_are_the_braille_spinner();
    for (const float size : SIZES) {
        ok &= label_stays_put(size);
    }
    std::fprintf(stderr, "%s: %s\n", TEST, ok ? "passed" : "FAILED");
    return ok ? 0 : 1;
}
