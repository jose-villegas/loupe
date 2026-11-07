// autana's maths package (launcher/packages/math), for the poses Frames and the
// window hand each other: its C headers wrapped for C++ once. The package sits
// below autana's engine, so the include gate (tests/include_gate.cmake) lets
// any file reach it; the engine's headers stay src/engine/autana.hpp's.
#pragma once

// C11's spelling of the assertion C++ calls static_assert, for autana's headers only.
#define _Static_assert static_assert // NOLINT(bugprone-reserved-identifier): C11 reserves it for this
extern "C" {
#include "math/linear/transformf.h"
}
#undef _Static_assert
