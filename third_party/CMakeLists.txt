# The window's libraries, each a pinned submodule. The top level adds this
# directory before autana_cpp_checks(), so Loupe's warning flags and clang-tidy
# stay off their code, and SYSTEM keeps their headers out of Loupe's warnings.

# SDL3: the window, its events and its Vulkan surface; one static library,
# without the subsystems Loupe never opens.
set(SDL_SHARED OFF CACHE BOOL "Build a shared SDL")
set(SDL_STATIC ON CACHE BOOL "Build a static SDL")
set(SDL_TEST_LIBRARY OFF CACHE BOOL "Build SDL3_test")
set(SDL_EXAMPLES OFF CACHE BOOL "Build SDL's examples")
set(SDL_INSTALL OFF CACHE BOOL "Install SDL")
foreach(subsystem IN ITEMS AUDIO GPU RENDER CAMERA HAPTIC HIDAPI POWER SENSOR DIALOG TRAY)
    set(SDL_${subsystem} OFF CACHE BOOL "SDL's ${subsystem} subsystem")
endforeach()
add_subdirectory(SDL SYSTEM)

# volk: Vulkan through SDL's loader at run time, so nothing links a Vulkan SDK.
set(VULKAN_HEADERS_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Headers" CACHE PATH "The Vulkan headers volk builds with")
add_subdirectory(volk SYSTEM)

# Dear ImGui with its SDL3 and Vulkan backends, the Vulkan one calling through volk.
set(imgui "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
add_library(imgui STATIC
    "${imgui}/imgui.cpp" "${imgui}/imgui_draw.cpp" "${imgui}/imgui_tables.cpp" "${imgui}/imgui_widgets.cpp"
    "${imgui}/backends/imgui_impl_sdl3.cpp" "${imgui}/backends/imgui_impl_vulkan.cpp")
target_include_directories(imgui SYSTEM PUBLIC "${imgui}" "${imgui}/backends")
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_VULKAN_USE_VOLK IMGUI_DISABLE_OBSOLETE_FUNCTIONS
                                        IMGUI_DISABLE_DEMO_WINDOWS)
target_link_libraries(imgui PUBLIC SDL3::SDL3-static volk::volk)
