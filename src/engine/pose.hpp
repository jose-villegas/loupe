// autana's packages (launcher/packages) that the poses Frames and the window
// hand each other need: math for the poses and the scene's box, kinematics for
// the flight that moves them; their C headers wrapped for C++ once. Packages sit
// below autana's engine, so the include gate (tests/include_gate.cmake) lets any
// file reach them; the engine's headers stay src/engine/autana.hpp's.
#pragma once

// C11's spelling of the assertion C++ calls static_assert, for autana's headers only.
#define _Static_assert static_assert // NOLINT(bugprone-reserved-identifier): C11 reserves it for this
extern "C" {
#include "kinematics/fly/fly_motion.h"
#include "math/geometry/box3f.h"
#include "math/linear/transformf.h"
}
#undef _Static_assert
