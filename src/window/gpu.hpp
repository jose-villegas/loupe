// The window: an SDL3 window drawn through Vulkan, with Dear ImGui on it.
#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

struct ImTextureData;
struct SDL_Window;

namespace loupe::window {

// A picture of the window: RGBA rows top-down, width * CHANNELS bytes each.
struct Picture {
    static constexpr int CHANNELS = 4;
    int width = 0, height = 0;
    std::vector<uint8_t> rgba;
};

class Gpu {
  public:
    // A window titled `title`, its panels drawn at `content_scale`, or at the
    // display's when none; null with `why` set when SDL or Vulkan cannot open one.
    static std::unique_ptr<Gpu> open(const char* title, std::string* why,
                                     std::optional<float> content_scale = std::nullopt);
    ~Gpu();
    Gpu(const Gpu&) = delete;
    Gpu& operator=(const Gpu&) = delete;
    Gpu(Gpu&&) = delete;
    Gpu& operator=(Gpu&&) = delete;

    // Takes the window's events and starts an ImGui frame; false once the window is closed.
    bool begin_frame();
    // Draws the ImGui frame and presents it.
    void end_frame();
    // end_frame(), and the picture it presented; none, with `why` set, when the
    // frame was not drawn or the swapchain cannot be read.
    std::optional<Picture> end_frame_read_back(std::string* why);
    // A width x height RGBA texture ImGui can draw, uploaded by ImGui's Vulkan
    // backend and freed with the Gpu; ImTextureDataQueueUpload() sends changes.
    ImTextureData& add_texture(int width, int height);
    // Frees a texture add_texture() made, once ImGui's Vulkan backend has let it go.
    void drop_texture(ImTextureData& texture);
    void set_title(const std::string& title);
    SDL_Window* sdl_window() const;

  private:
    struct State;
    explicit Gpu(std::unique_ptr<State> state);
    std::unique_ptr<State> state_;
};

} // namespace loupe::window
