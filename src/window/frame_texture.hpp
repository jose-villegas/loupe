// The engine's frame as one texture ImGui draws: a user texture the Vulkan
// backend creates and uploads like its own font atlas.
#pragma once

#include <memory>

#include "engine/frames.hpp"
#include "imgui.h"
#include "window/gpu.hpp"

namespace loupe::window {

class FrameTexture {
  public:
    explicit FrameTexture(Gpu& gpu);
    ~FrameTexture();
    FrameTexture(const FrameTexture&) = delete;
    FrameTexture& operator=(const FrameTexture&) = delete;
    FrameTexture(FrameTexture&&) = delete;
    FrameTexture& operator=(FrameTexture&&) = delete;

    // Takes `frame` for the next ImGui frame: BGR rows bottom-up become RGBA rows
    // top-down, so it draws unflipped. A new size makes a new texture.
    void upload(const Frame& frame);
    // Nothing to draw before the first upload.
    bool empty() const;
    ImTextureRef ref() const;
    int width() const;
    int height() const;

  private:
    // Frees the texture, on the GPU and in ImGui's lists.
    void release();
    Gpu& gpu_;
    std::unique_ptr<ImTextureData> texture_;
};

} // namespace loupe::window
