// The one place Loupe includes autana's headers (tests/include_gate.cmake).
#pragma once

// C11's spelling of the assertion C++ calls static_assert, for autana's headers only.
#define _Static_assert static_assert
extern "C" {
#include "asset/asset_store.h"
#include "render_host.h"

// The scene-file provider, launcher/tools/render/scene_viewer.c.
extern const frame_provider_t scene_provider;
}
#undef _Static_assert
