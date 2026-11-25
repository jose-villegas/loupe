# The window's libraries, each a pinned submodule. The top level adds this
# directory before autana_cpp_checks(), so Loupe's warning flags and clang-tidy
# stay off their code, and SYSTEM keeps their headers out of Loupe's warnings.

# SDL3: the window, its events, its Vulkan surface and the open-file dialog;
# one static library, without the subsystems Loupe never opens.
set(SDL_SHARED OFF CACHE BOOL "Build a shared SDL")
set(SDL_STATIC ON CACHE BOOL "Build a static SDL")
set(SDL_TEST_LIBRARY OFF CACHE BOOL "Build SDL3_test")
set(SDL_EXAMPLES OFF CACHE BOOL "Build SDL's examples")
set(SDL_INSTALL OFF CACHE BOOL "Install SDL")
foreach(subsystem IN ITEMS AUDIO GPU RENDER CAMERA HAPTIC HIDAPI POWER SENSOR TRAY)
    set(SDL_${subsystem} OFF CACHE BOOL "SDL's ${subsystem} subsystem")
endforeach()
add_subdirectory(SDL SYSTEM)

# volk: Vulkan through SDL's loader at run time, so nothing links a Vulkan SDK.
set(VULKAN_HEADERS_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Headers" CACHE PATH "The Vulkan headers volk builds with")
add_subdirectory(volk SYSTEM)

# Dear ImGui's docking line with its SDL3 and Vulkan backends, the Vulkan one calling through volk.
set(imgui "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
add_library(imgui STATIC
    "${imgui}/imgui.cpp" "${imgui}/imgui_draw.cpp" "${imgui}/imgui_tables.cpp" "${imgui}/imgui_widgets.cpp"
    "${imgui}/backends/imgui_impl_sdl3.cpp" "${imgui}/backends/imgui_impl_vulkan.cpp")
target_include_directories(imgui SYSTEM PUBLIC "${imgui}" "${imgui}/backends")
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_VULKAN_USE_VOLK IMGUI_DISABLE_OBSOLETE_FUNCTIONS
                                        IMGUI_DISABLE_DEMO_WINDOWS)
target_link_libraries(imgui PUBLIC SDL3::SDL3-static volk::volk)
# A storyboard build (LOUPE_STORYBOARD): ImGui with the test engine's hooks,
# and the engine itself, which drives the window through ImGui's own input.
# Its licence is not the GPL's, so the downloads are built without it.
if(LOUPE_STORYBOARD)
    set(test_engine "${CMAKE_CURRENT_SOURCE_DIR}/imgui_test_engine/imgui_test_engine")
    file(GLOB test_engine_sources "${test_engine}/*.cpp")
    target_sources(imgui PRIVATE ${test_engine_sources})
    target_include_directories(imgui SYSTEM PUBLIC "${test_engine}" "${PROJECT_SOURCE_DIR}/src/storyboard/config")
    target_compile_definitions(imgui PUBLIC "IMGUI_USER_CONFIG=\"imgui_config.h\"")

    # yaml-cpp: the recipes are YAML.
    set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Build yaml-cpp's tools")
    set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "Build yaml-cpp's contrib")
    set(YAML_CPP_INSTALL OFF CACHE BOOL "Install yaml-cpp")
    set(YAML_CPP_FORMAT_SOURCE OFF CACHE BOOL "Format yaml-cpp's sources")
    set(YAML_CPP_DISABLE_UNINSTALL ON CACHE BOOL "No uninstall target for yaml-cpp")
    add_subdirectory(yaml-cpp SYSTEM)
endif()

# ImGuizmo: the editor's move/rotate/scale handles and the view cube (ImGuizmo.cpp alone).
set(imguizmo "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo/src")
add_library(imguizmo STATIC "${imguizmo}/ImGuizmo.cpp")
target_include_directories(imguizmo SYSTEM PUBLIC "${imguizmo}")
target_link_libraries(imguizmo PUBLIC imgui)

# nlohmann/json: GitHub's release answers, for the self-update.
set(JSON_BuildTests OFF CACHE BOOL "Build nlohmann/json's tests")
set(JSON_Install OFF CACHE BOOL "Install nlohmann/json")
add_subdirectory(json SYSTEM)

# Mbed TLS (the LTS line): only its SHA-256, for the self-update's digests.
set(mbedtls "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls")
add_library(mbedtls_sha256 STATIC "${mbedtls}/library/sha256.c" "${mbedtls}/library/platform_util.c")
target_include_directories(mbedtls_sha256 SYSTEM PUBLIC "${mbedtls}/include")
target_include_directories(mbedtls_sha256 PRIVATE "${mbedtls}/library")
