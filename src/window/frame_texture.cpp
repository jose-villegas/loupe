#include "window/frame_texture.hpp"

#include <cstddef>
#include <cstdint>

#include "imgui_internal.h"

namespace loupe::window {

namespace {

constexpr int RGBA_BYTES = 4;
constexpr int BGR_BYTES = 3;
constexpr uint8_t ALPHA_OPAQUE = 0xFF;

} // namespace

FrameTexture::FrameTexture(Gpu& gpu) : gpu_(gpu) {}

FrameTexture::~FrameTexture() { release(); }

void
FrameTexture::release() {
    if (texture_ == nullptr) {
        return;
    }
    gpu_.destroy_texture(texture_.get());
    ImGui::UnregisterUserTexture(texture_.get());
    // The backend's list of textures, rebuilt each frame, would keep it until the next.
    ImGui::GetPlatformIO().Textures.find_erase(texture_.get());
    texture_.reset();
}

void
FrameTexture::upload(const Frame& frame) {
    if (texture_ != nullptr && (texture_->Width != frame.width || texture_->Height != frame.height)) {
        release();
    }
    if (texture_ == nullptr) {
        texture_ = std::make_unique<ImTextureData>();
        texture_->Create(ImTextureFormat_RGBA32, frame.width, frame.height);
        ImGui::RegisterUserTexture(texture_.get());
    }
    auto* pixels = static_cast<uint8_t*>(texture_->GetPixels());
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
    ImTextureDataQueueUpload(texture_.get(), 0, 0, frame.width, frame.height);
}

bool
FrameTexture::empty() const {
    return texture_ == nullptr;
}

ImTextureRef
FrameTexture::ref() const {
    return texture_->GetTexRef();
}

int
FrameTexture::width() const {
    return texture_->Width;
}

int
FrameTexture::height() const {
    return texture_->Height;
}

} // namespace loupe::window
