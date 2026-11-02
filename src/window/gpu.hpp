// The window: an SDL3 window drawn through Vulkan, with Dear ImGui on it.
#pragma once

#include <memory>
#include <string>

struct ImTextureData;

namespace loupe::window {

class Gpu {
  public:
    // A window titled `title`; null with `why` set when SDL or Vulkan cannot open one.
    static std::unique_ptr<Gpu> open(const char* title, std::string* why);
    ~Gpu();
    Gpu(const Gpu&) = delete;
    Gpu& operator=(const Gpu&) = delete;
    Gpu(Gpu&&) = delete;
    Gpu& operator=(Gpu&&) = delete;

    // Takes the window's events and starts an ImGui frame; false once the window is closed.
    bool begin_frame();
    // Draws the ImGui frame and presents it.
    void end_frame();
    // A width x height RGBA texture ImGui can draw, uploaded by ImGui's Vulkan
    // backend and freed with the Gpu; ImTextureDataQueueUpload() sends changes.
    ImTextureData& add_texture(int width, int height);

  private:
    struct State;
    explicit Gpu(std::unique_ptr<State> state);
    std::unique_ptr<State> state_;
};

} // namespace loupe::window
