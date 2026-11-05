// autana's maths package (launcher/packages/math), for the poses Frames and the
// window hand each other. Beside src/engine/autana.hpp, the one other place
// Loupe includes autana's headers (tests/include_gate.cmake): the package
// stands alone, below the engine, so including it pulls in nothing else.
#pragma once

// C11's spelling of the assertion C++ calls static_assert, for autana's headers only.
#define _Static_assert static_assert // NOLINT(bugprone-reserved-identifier): C11 reserves it for this
extern "C" {
#include "math/linear/transformf.h"
}
#undef _Static_assert
