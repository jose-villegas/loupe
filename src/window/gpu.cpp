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
#include <optional>
#include <string>
#include <utility>
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

// A memory type of `bits` with every flag of `wanted`; none when the device has none.
std::optional<uint32_t>
memory_type(VkPhysicalDevice physical, uint32_t bits, VkMemoryPropertyFlags wanted) {
    VkPhysicalDeviceMemoryProperties properties = {};
    vkGetPhysicalDeviceMemoryProperties(physical, &properties);
    for (uint32_t type = 0; type < properties.memoryTypeCount; type++) {
        if ((bits & (1U << type)) != 0 && (properties.memoryTypes[type].propertyFlags & wanted) == wanted) {
            return type;
        }
    }
    return std::nullopt;
}

// One step of an image's layout, each pair from before to after: the
// layouts, the work's access and its stages.
struct LayoutStep {
    std::pair<VkImageLayout, VkImageLayout> layout;
    std::pair<VkAccessFlags, VkAccessFlags> access;
    std::pair<VkPipelineStageFlags, VkPipelineStageFlags> stage;
};

void
image_barrier(VkCommandBuffer commands, VkImage image, const LayoutStep& step) {
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = step.access.first;
    barrier.dstAccessMask = step.access.second;
    barrier.oldLayout = step.layout.first;
    barrier.newLayout = step.layout.second;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                .baseMipLevel = 0,
                                .levelCount = 1,
                                .baseArrayLayer = 0,
                                .layerCount = 1};
    vkCmdPipelineBarrier(commands, step.stage.first, step.stage.second, 0, 0, nullptr, 0, nullptr, 1, &barrier);
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
    // TRANSFER_SRC where the surface allows it, so a presented picture can be read back.
    VkImageUsageFlags image_usage = 0;
    bool rebuild = false;
    // The read-back: asked of the frame being drawn, recorded into its
    // commands, and the host-visible buffer the swapchain image is copied into.
    bool read_asked = false;
    bool read_recorded = false;
    VkBuffer read_buffer = VK_NULL_HANDLE;
    VkDeviceMemory read_memory = VK_NULL_HANDLE;
    VkDeviceSize read_size = 0;
    // What of ImGui is up, for ~Gpu to take down.
    bool imgui_context = false;
    bool imgui_sdl = false;
    bool imgui_vulkan = false;
    // Freed after ImGui's Vulkan backend has freed their images.
    std::vector<std::unique_ptr<ImTextureData>> textures;

    bool open_instance(std::string* why);
    bool open_device(std::string* why);
    bool open_swapchain(std::string* why);
    bool open_imgui(std::optional<float> content_scale, std::string* why);
    // A read buffer of `size` bytes; false when none can be made.
    bool keep_read_buffer(VkDeviceSize size);
    void drop_read_buffer();
    // Copies `image`, about to be presented, into the read buffer after the frame's drawing.
    void record_read_back(VkCommandBuffer commands, VkImage image);

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
    VkSurfaceCapabilitiesKHR capabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, surface, &capabilities);
    image_usage = capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    int width = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(window, &width, &height);
    ImGui_ImplVulkanH_CreateOrResizeWindow(instance, physical, device, &swapchain, queue_family, nullptr, width, height,
                                           MIN_IMAGE_COUNT, image_usage);
    // Before ImGui's Vulkan backend is up, its helpers say nothing of a failure.
    if (swapchain.Swapchain == VK_NULL_HANDLE || swapchain.RenderPass == VK_NULL_HANDLE) {
        *why = "no Vulkan swapchain for the window";
        return false;
    }
    return true;
}

bool
Gpu::State::open_imgui(std::optional<float> content_scale, std::string* why) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    imgui_context = true;
    // No imgui.ini beside wherever Loupe was started.
    ImGui::GetIO().IniFilename = nullptr;
    // The scene's panels dock into one window (run_window.cpp lays them out).
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    const float scale = content_scale.value_or(SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay()));
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
    if (read_asked) {
        record_read_back(frame.CommandBuffer, frame.Backbuffer);
    }
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

bool
Gpu::State::keep_read_buffer(VkDeviceSize size) {
    if (read_buffer != VK_NULL_HANDLE && read_size == size) {
        return true;
    }
    drop_read_buffer();
    VkBufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = size;
    info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(device, &info, nullptr, &read_buffer) != VK_SUCCESS) {
        read_buffer = VK_NULL_HANDLE;
        return false;
    }
    VkMemoryRequirements needs = {};
    vkGetBufferMemoryRequirements(device, read_buffer, &needs);
    const std::optional<uint32_t> type = memory_type(
        physical, needs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VkMemoryAllocateInfo allocate = {};
    allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate.allocationSize = needs.size;
    allocate.memoryTypeIndex = type.value_or(0);
    if (!type || vkAllocateMemory(device, &allocate, nullptr, &read_memory) != VK_SUCCESS
        || vkBindBufferMemory(device, read_buffer, read_memory, 0) != VK_SUCCESS) {
        drop_read_buffer();
        return false;
    }
    read_size = size;
    return true;
}

void
Gpu::State::drop_read_buffer() {
    if (read_buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, read_buffer, nullptr);
    }
    if (read_memory != VK_NULL_HANDLE) {
        vkFreeMemory(device, read_memory, nullptr);
    }
    read_buffer = VK_NULL_HANDLE;
    read_memory = VK_NULL_HANDLE;
    read_size = 0;
}

void
Gpu::State::record_read_back(VkCommandBuffer commands, VkImage image) {
    const auto width = static_cast<uint32_t>(swapchain.Width);
    const auto height = static_cast<uint32_t>(swapchain.Height);
    if ((image_usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) == 0
        || !keep_read_buffer(VkDeviceSize{width} * height * Picture::CHANNELS)) {
        return;
    }
    // The render pass leaves the image ready to present; the copy reads it, then hands it back.
    image_barrier(commands, image,
                  {.layout = {VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL},
                   .access = {VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT},
                   // All commands: the render pass's own move to PRESENT_SRC is ordered only by its
                   // implicit dependency to BOTTOM_OF_PIPE, which COLOR_ATTACHMENT_OUTPUT alone misses.
                   .stage = {VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT}});
    VkBufferImageCopy copy = {};
    copy.imageSubresource = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1};
    copy.imageExtent = {.width = width, .height = height, .depth = 1};
    vkCmdCopyImageToBuffer(commands, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, read_buffer, 1, &copy);
    image_barrier(commands, image,
                  {.layout = {VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR},
                   .access = {VK_ACCESS_TRANSFER_READ_BIT, 0},
                   .stage = {VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT}});
    VkBufferMemoryBarrier to_host = {};
    to_host.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    to_host.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    to_host.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
    to_host.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    to_host.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    to_host.buffer = read_buffer;
    to_host.size = VK_WHOLE_SIZE;
    vkCmdPipelineBarrier(commands, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 0, nullptr, 1,
                         &to_host, 0, nullptr);
    read_recorded = true;
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
Gpu::open(const char* title, std::string* why, std::optional<float> content_scale) {
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
    if (!s.open_instance(why) || !s.open_device(why) || !s.open_swapchain(why) || !s.open_imgui(content_scale, why)) {
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
    s.drop_read_buffer();
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
                                               width, height, MIN_IMAGE_COUNT, s.image_usage);
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

std::optional<Picture>
Gpu::end_frame_read_back(std::string* why) {
    State& s = *state_;
    // Four 8-bit channels in one of the two orders open_swapchain asks for; a
    // surface that offered neither gave another format, read as nothing.
    const VkFormat format = s.swapchain.SurfaceFormat.format;
    const bool readable = (s.image_usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) != 0
                          && (format == VK_FORMAT_B8G8R8A8_UNORM || format == VK_FORMAT_R8G8B8A8_UNORM);
    s.read_asked = readable;
    s.read_recorded = false;
    end_frame();
    s.read_asked = false;
    if (!readable) {
        *why = std::format("the window's swapchain (format {}) cannot be read back", static_cast<int>(format));
        return std::nullopt;
    }
    if (!s.read_recorded) {
        *why = "the frame was not drawn, or no buffer could hold it";
        return std::nullopt;
    }
    check(vkDeviceWaitIdle(s.device));
    void* mapped = nullptr; // NOLINT(misc-const-correctness): vkMapMemory writes through a void**
    check(vkMapMemory(s.device, s.read_memory, 0, s.read_size, 0, &mapped));
    Picture picture{.width = s.swapchain.Width, .height = s.swapchain.Height, .rgba = {}};
    const auto* bytes = static_cast<const uint8_t*>(mapped);
    picture.rgba.assign(bytes, bytes + s.read_size);
    vkUnmapMemory(s.device, s.read_memory);
    // The swapchain is BGRA or RGBA (open_swapchain); the window has no alpha to keep.
    const bool bgra = format == VK_FORMAT_B8G8R8A8_UNORM;
    for (size_t at = 0; at < picture.rgba.size(); at += Picture::CHANNELS) {
        if (bgra) {
            std::swap(picture.rgba[at], picture.rgba[at + 2]);
        }
        picture.rgba[at + 3] = UINT8_MAX;
    }
    return picture;
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
