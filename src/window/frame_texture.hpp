// The engine's frame as one texture ImGui draws: a user texture the Vulkan
// backend creates, uploads and frees like its own font atlas.
#pragma once

#include "engine/frames.hpp"
#include "imgui.h"
#include "window/gpu.hpp"

namespace loupe::window {

class FrameTexture {
  public:
    // At `first`'s size, which is the run's: autana fixes it when the run opens.
    FrameTexture(Gpu& gpu, const Frame& first);

    // Takes `frame` for the next ImGui frame: BGR rows bottom-up become RGBA rows
    // top-down, so it draws unflipped.
    void upload(const Frame& frame);
    ImTextureRef ref() const;
    int width() const;
    int height() const;

  private:
    ImTextureData& texture_;
};

} // namespace loupe::window
