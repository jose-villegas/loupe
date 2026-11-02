// After imgui's examples/example_sdl3_vulkan: one instance, device and queue,
// the swapchain kept by imgui's ImGui_ImplVulkanH_* helpers.
#include "window/gpu.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <volk.h>

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <vector>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"
#include "imgui_internal.h"

namespace loupe::window {

namespace {

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;
// Double-buffered: FIFO presents at the display's refresh.
constexpr uint32_t MIN_IMAGE_COUNT = 2;
// Sampled images: imgui's font atlas, and one per added texture (the engine's frame).
constexpr uint32_t DESCRIPTOR_POOL_SIZE = IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE + 1;
// How long a minimized window sleeps between looks at its events.
constexpr int MINIMIZED_WAIT_MS = 10;
// The app logo, assets/loupe.png, as CMake built it in.
constexpr unsigned char ICON_PNG[] = {
#include "window/icon_png.inc"
};

// Gives `window` the app logo; a window without it still works.
void
set_icon(SDL_Window* window) {
    SDL_Surface* icon = SDL_LoadPNG_IO(SDL_IOFromConstMem(ICON_PNG, sizeof ICON_PNG), true);
    if (icon == nullptr || !SDL_SetWindowIcon(window, icon)) {
        std::fprintf(stderr, "loupe: no window icon: %s\n", SDL_GetError());
    }
    SDL_DestroySurface(icon);
}

// Past setup, a Vulkan error leaves no window to go on with.
void
check(VkResult result) {
    if (result < 0) {
        std::fprintf(stderr, "loupe: Vulkan error %d\n", static_cast<int>(result));
        std::abort();
    }
}

} // namespace

struct Gpu::State {
    SDL_Window* window = nullptr;
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physical = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t queue_family = 0;
    VkQueue queue = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    ImGui_ImplVulkanH_Window swapchain;
    bool rebuild = false;
    // What of ImGui is up, for ~Gpu to take down.
    bool imgui_context = false;
    bool imgui_sdl = false;
    bool imgui_vulkan = false;
    // Freed after ImGui's Vulkan backend has freed their images.
    std::vector<std::unique_ptr<ImTextureData>> textures;

    bool open_instance(std::string* why);
    bool open_device(std::string* why);
    bool open_swapchain(std::string* why);
    bool open_imgui(std::string* why);

    // The semaphores of the frame being drawn.
    const ImGui_ImplVulkanH_FrameSemaphores&
    semaphores() const {
        return swapchain.FrameSemaphores[static_cast<int>(swapchain.SemaphoreIndex)];
    }

    void render(ImDrawData* draw_data);
    void present();
};

bool
Gpu::State::open_instance(std::string* why) {
    const SDL_FunctionPointer loader = SDL_Vulkan_GetVkGetInstanceProcAddr();
    uint32_t count = 0;
    const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&count);
    if (loader == nullptr || extensions == nullptr) {
        *why = std::format("no Vulkan loader: {}", SDL_GetError());
        return false;
    }
    volkInitializeCustom(reinterpret_cast<PFN_vkGetInstanceProcAddr>(loader));
    VkApplicationInfo app = {};
    app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app.pApplicationName = "loupe";
    app.apiVersion = VK_API_VERSION_1_1;
    VkInstanceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo = &app;
    info.enabledExtensionCount = count;
    info.ppEnabledExtensionNames = extensions;
    const VkResult result = vkCreateInstance(&info, nullptr, &instance);
    if (result != VK_SUCCESS) {
        *why = std::format("no Vulkan instance (VkResult {})", static_cast<int>(result));
        return false;
    }
    volkLoadInstance(instance);
    return true;
}

bool
Gpu::State::open_device(std::string* why) {
    physical = ImGui_ImplVulkanH_SelectPhysicalDevice(instance);
    if (physical == VK_NULL_HANDLE) {
        *why = "no Vulkan device";
        return false;
    }
    queue_family = ImGui_ImplVulkanH_SelectQueueFamilyIndex(physical);
    const float priority = 1.0F;
    VkDeviceQueueCreateInfo queue_info = {};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.queueFamilyIndex = queue_family;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = &priority;
    const std::array<const char*, 1> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &queue_info;
    info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    info.ppEnabledExtensionNames = extensions.data();
    const VkResult result = vkCreateDevice(physical, &info, nullptr, &device);
    if (result != VK_SUCCESS) {
        *why = std::format("no Vulkan device with a swapchain (VkResult {})", static_cast<int>(result));
        return false;
    }
    volkLoadDevice(device);
    vkGetDeviceQueue(device, queue_family, 0, &queue);
    return true;
}

bool
Gpu::State::open_swapchain(std::string* why) {
    if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface)) {
        *why = std::format("no Vulkan surface: {}", SDL_GetError());
        return false;
    }
    VkBool32 presents = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(physical, queue_family, surface, &presents);
    if (presents != VK_TRUE) {
        *why = "the Vulkan device cannot present to the window";
        return false;
    }
    const std::array<VkFormat, 2> formats = {VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM};
    const VkPresentModeKHR fifo = VK_PRESENT_MODE_FIFO_KHR;
    swapchain.Surface = surface;
    swapchain.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
        physical, surface, formats.data(), static_cast<int>(formats.size()), VK_COLORSPACE_SRGB_NONLINEAR_KHR);
    swapchain.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(physical, surface, &fifo, 1);
    int width = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(window, &width, &height);
    ImGui_ImplVulkanH_CreateOrResizeWindow(instance, physical, device, &swapchain, queue_family, nullptr, width, height,
                                           MIN_IMAGE_COUNT, 0);
    // Before ImGui's Vulkan backend is up, its helpers say nothing of a failure.
    if (swapchain.Swapchain == VK_NULL_HANDLE || swapchain.RenderPass == VK_NULL_HANDLE) {
        *why = "no Vulkan swapchain for the window";
        return false;
    }
    return true;
}

bool
Gpu::State::open_imgui(std::string* why) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    imgui_context = true;
    // No imgui.ini beside wherever Loupe was started.
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    const float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    ImGui::GetStyle().ScaleAllSizes(scale);
    ImGui::GetStyle().FontScaleDpi = scale;
    imgui_sdl = ImGui_ImplSDL3_InitForVulkan(window);
    if (!imgui_sdl) {
        *why = "ImGui's SDL3 backend did not start";
        return false;
    }
    ImGui_ImplVulkan_InitInfo info = {};
    info.ApiVersion = VK_API_VERSION_1_1;
    info.Instance = instance;
    info.PhysicalDevice = physical;
    info.Device = device;
    info.QueueFamily = queue_family;
    info.Queue = queue;
    info.DescriptorPoolSize = DESCRIPTOR_POOL_SIZE;
    info.MinImageCount = MIN_IMAGE_COUNT;
    info.ImageCount = swapchain.ImageCount;
    info.PipelineInfoMain.RenderPass = swapchain.RenderPass;
    info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    info.CheckVkResultFn = check;
    imgui_vulkan = ImGui_ImplVulkan_Init(&info);
    if (!imgui_vulkan) {
        *why = "ImGui's Vulkan backend did not start";
    }
    return imgui_vulkan;
}

void
Gpu::State::render(ImDrawData* draw_data) {
    const VkSemaphore acquired = semaphores().ImageAcquiredSemaphore;
    const VkSemaphore rendered = semaphores().RenderCompleteSemaphore;
    const VkResult result =
        vkAcquireNextImageKHR(device, swapchain.Swapchain, UINT64_MAX, acquired, VK_NULL_HANDLE, &swapchain.FrameIndex);
    rebuild |= result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR;
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return;
    }
    check(result);
    const ImGui_ImplVulkanH_Frame& frame = swapchain.Frames[static_cast<int>(swapchain.FrameIndex)];
    check(vkWaitForFences(device, 1, &frame.Fence, VK_TRUE, UINT64_MAX));
    check(vkResetFences(device, 1, &frame.Fence));
    check(vkResetCommandPool(device, frame.CommandPool, 0));
    VkCommandBufferBeginInfo begin = {};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    check(vkBeginCommandBuffer(frame.CommandBuffer, &begin));
    VkRenderPassBeginInfo pass = {};
    pass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    pass.renderPass = swapchain.RenderPass;
    pass.framebuffer = frame.Framebuffer;
    pass.renderArea.extent.width = static_cast<uint32_t>(swapchain.Width);
    pass.renderArea.extent.height = static_cast<uint32_t>(swapchain.Height);
    pass.clearValueCount = 1;
    pass.pClearValues = &swapchain.ClearValue;
    vkCmdBeginRenderPass(frame.CommandBuffer, &pass, VK_SUBPASS_CONTENTS_INLINE);
    ImGui_ImplVulkan_RenderDrawData(draw_data, frame.CommandBuffer);
    vkCmdEndRenderPass(frame.CommandBuffer);
    check(vkEndCommandBuffer(frame.CommandBuffer));
    const VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit = {};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &acquired;
    submit.pWaitDstStageMask = &stage;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &frame.CommandBuffer;
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &rendered;
    check(vkQueueSubmit(queue, 1, &submit, frame.Fence));
}

void
Gpu::State::present() {
    if (rebuild) {
        return;
    }
    const VkSemaphore rendered = semaphores().RenderCompleteSemaphore;
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &rendered;
    info.swapchainCount = 1;
    info.pSwapchains = &swapchain.Swapchain;
    info.pImageIndices = &swapchain.FrameIndex;
    const VkResult result = vkQueuePresentKHR(queue, &info);
    rebuild |= result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR;
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return;
    }
    check(result);
    swapchain.SemaphoreIndex = (swapchain.SemaphoreIndex + 1) % swapchain.SemaphoreCount;
}

std::unique_ptr<Gpu>
Gpu::open(const char* title, std::string* why) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        *why = std::format("no video: {}", SDL_GetError());
        return nullptr;
    }
    auto state = std::make_unique<State>();
    std::unique_ptr<Gpu> gpu(new Gpu(std::move(state)));
    State& s = *gpu->state_;
    s.window = SDL_CreateWindow(title, WINDOW_WIDTH, WINDOW_HEIGHT,
                                SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (s.window == nullptr) {
        *why = std::format("no window: {}", SDL_GetError());
        return nullptr;
    }
    set_icon(s.window);
    if (!s.open_instance(why) || !s.open_device(why) || !s.open_swapchain(why) || !s.open_imgui(why)) {
        return nullptr;
    }
    return gpu;
}

Gpu::Gpu(std::unique_ptr<State> state) : state_(std::move(state)) {}

Gpu::~Gpu() {
    State& s = *state_;
    if (s.device != VK_NULL_HANDLE) {
        check(vkDeviceWaitIdle(s.device));
    }
    if (s.imgui_vulkan) {
        ImGui_ImplVulkan_Shutdown();
    }
    if (s.imgui_sdl) {
        ImGui_ImplSDL3_Shutdown();
    }
    if (s.imgui_context) {
        ImGui::DestroyContext();
    }
    s.textures.clear();
    if (s.swapchain.Swapchain != VK_NULL_HANDLE) {
        ImGui_ImplVulkanH_DestroyWindow(s.instance, s.device, &s.swapchain, nullptr);
    }
    if (s.surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(s.instance, s.surface, nullptr);
    }
    if (s.device != VK_NULL_HANDLE) {
        vkDestroyDevice(s.device, nullptr);
    }
    if (s.instance != VK_NULL_HANDLE) {
        vkDestroyInstance(s.instance, nullptr);
    }
    if (s.window != nullptr) {
        SDL_DestroyWindow(s.window);
    }
    SDL_Quit();
}

bool
Gpu::begin_frame() {
    State& s = *state_;
    for (;;) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                return false;
            }
        }
        if ((SDL_GetWindowFlags(s.window) & SDL_WINDOW_MINIMIZED) == 0) {
            break;
        }
        SDL_WaitEventTimeout(nullptr, MINIMIZED_WAIT_MS);
    }
    int width = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(s.window, &width, &height);
    if (width > 0 && height > 0 && (s.rebuild || s.swapchain.Width != width || s.swapchain.Height != height)) {
        ImGui_ImplVulkan_SetMinImageCount(MIN_IMAGE_COUNT);
        ImGui_ImplVulkanH_CreateOrResizeWindow(s.instance, s.physical, s.device, &s.swapchain, s.queue_family, nullptr,
                                               width, height, MIN_IMAGE_COUNT, 0);
        s.swapchain.FrameIndex = 0;
        s.rebuild = false;
    }
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    return true;
}

void
Gpu::end_frame() {
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    if (draw_data->DisplaySize.x > 0 && draw_data->DisplaySize.y > 0) {
        state_->render(draw_data);
        state_->present();
    }
    // A dropped texture the backend has destroyed leaves ImGui, both its user
    // list and the backend's, before the Gpu frees it.
    std::erase_if(state_->textures, [](const std::unique_ptr<ImTextureData>& texture) {
        const bool gone = texture->Status == ImTextureStatus_Destroyed && texture->WantDestroyNextFrame;
        if (gone) {
            ImGui::UnregisterUserTexture(texture.get());
            ImGui::GetPlatformIO().Textures.find_erase(texture.get());
        }
        return gone;
    });
}

ImTextureData&
Gpu::add_texture(int width, int height) {
    auto& texture = state_->textures.emplace_back(std::make_unique<ImTextureData>());
    texture->Create(ImTextureFormat_RGBA32, width, height);
    // The backend creates it at the next frame's end and frees it at shutdown.
    ImGui::RegisterUserTexture(texture.get());
    return *texture;
}

void
Gpu::drop_texture(ImTextureData& texture) {
    // ImGui asks the backend to destroy it next frame, once no frame in flight draws it.
    texture.WantDestroyNextFrame = true;
}

void
Gpu::set_title(const std::string& title) {
    SDL_SetWindowTitle(state_->window, title.c_str());
}

SDL_Window*
Gpu::sdl_window() const {
    return state_->window;
}

} // namespace loupe::window
