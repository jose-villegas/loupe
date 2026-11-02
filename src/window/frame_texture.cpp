#include "window/frame_texture.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>

#include "imgui_internal.h"

namespace loupe::window {

namespace {

constexpr int RGBA_BYTES = 4;
constexpr int BGR_BYTES = 3;
constexpr uint8_t ALPHA_OPAQUE = 0xFF;

} // namespace

FrameTexture::FrameTexture(Gpu& gpu, const Frame& first)
    : gpu_(gpu), texture_(gpu.add_texture(first.width, first.height)) {
    upload(first);
}

FrameTexture::~FrameTexture() { gpu_.drop_texture(texture_); }

void
FrameTexture::upload(const Frame& frame) {
    assert(frame.width == texture_.Width && frame.height == texture_.Height);
    auto* pixels = static_cast<uint8_t*>(texture_.GetPixels());
    for (int y = 0; y < frame.height; y++) {
        const uint8_t* bgr = frame.bgr.data() + (static_cast<size_t>(frame.height - 1 - y) * frame.stride);
        uint8_t* rgba = pixels + (static_cast<size_t>(y) * frame.width * RGBA_BYTES);
        for (int x = 0; x < frame.width; x++, bgr += BGR_BYTES, rgba += RGBA_BYTES) {
            rgba[0] = bgr[2];
            rgba[1] = bgr[1];
            rgba[2] = bgr[0];
            rgba[3] = ALPHA_OPAQUE;
        }
    }
    ImTextureDataQueueUpload(&texture_, 0, 0, frame.width, frame.height);
}

ImTextureRef
FrameTexture::ref() const {
    return texture_.GetTexRef();
}

int
FrameTexture::width() const {
    return texture_.Width;
}

int
FrameTexture::height() const {
    return texture_.Height;
}

} // namespace loupe::window
