// The one place Loupe includes autana's headers (tests/include_gate.cmake).
#pragma once

// C11's spelling of the assertion C++ calls static_assert, for autana's headers only.
#define _Static_assert static_assert
extern "C" {
#include "asset/asset_store.h"
#include "render_host.h"
#include "scene_viewer.h"
}
#undef _Static_assert
