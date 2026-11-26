// Dear ImGui's configuration in a storyboard build: the test engine's hooks,
// its coroutines on std::thread.
#pragma once

#define IMGUI_TEST_ENGINE_ENABLE_COROUTINE_STDTHREAD_IMPL 1
#include "imgui_te_imconfig.h"
