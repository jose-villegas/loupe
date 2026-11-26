cmake_minimum_required(VERSION 3.25...3.31)
# The one version: --version, About, the release's file names and its tag (v0.1.0).
project(loupe VERSION 0.1.0 LANGUAGES C CXX)

set(AUTANA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/autana" CACHE PATH
    "The autana checkout Loupe builds against; the submodule by default")
if(NOT EXISTS "${AUTANA_DIR}/launcher/tools/render/CMakeLists.txt")
    message(FATAL_ERROR "No autana at ${AUTANA_DIR}: run `git submodule update --init` or pass -DAUTANA_DIR=<path>")
endif()
# The pack tool builds a .scene.toml's pack with autana's build_pack.py, so
# only a build with it needs Python; without it, loupe opens .apak files alone.
option(LOUPE_PACK_TOOL "Open a .scene.toml by building its pack with autana's build_pack.py" ON)
# The release's samples: every autana demo scene's pack, installed (cmake/package.cmake).
option(LOUPE_SAMPLES "Install every autana demo scene's pack as a sample" OFF)
# loupe-storyboard, which runs a recipe through the real window: Dear ImGui's
# test engine drives it (third_party/CMakeLists.txt). Off, as its licence is not
# the GPL's; the development presets turn it on, the downloads never do.
option(LOUPE_STORYBOARD "Build loupe-storyboard, with Dear ImGui's test engine" OFF)
# Where this build's updates come from (src/version.hpp); the release workflow
# sets it for the downloads, and a local build stays None.
set(LOUPE_CHANNEL None CACHE STRING "The channel this build is for: None, Releases or Main")
set(channels None Releases Main)
set_property(CACHE LOUPE_CHANNEL PROPERTY STRINGS ${channels})
if(NOT LOUPE_CHANNEL IN_LIST channels)
    message(FATAL_ERROR "LOUPE_CHANNEL is ${LOUPE_CHANNEL}, not one of ${channels}")
endif()
file(STRINGS "${AUTANA_DIR}/scripts/lib/python-min-version" python_min LIMIT_COUNT 1)
if(LOUPE_PACK_TOOL OR LOUPE_SAMPLES)
    find_package(Python3 ${python_min} REQUIRED COMPONENTS Interpreter)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Loupe has no modules; the scan adds gcc-only flags clang-tidy refuses.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
if(MINGW)
    # One file that runs anywhere, not whichever libstdc++ is first on PATH.
    add_link_options(-static)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT AUTANA_CPP_SANITIZE)
    # GCC's runtime inside too, so a Linux loupe needs only the C library;
    # the sanitizers keep theirs shared.
    add_link_options(-static-libstdc++ -static-libgcc)
endif()

# autana's C++ rules: sanitizers (AUTANA_CPP_SANITIZE) before its subdirectory,
# so they cover its C too; flags and clang-tidy (AUTANA_CPP_CLANG_TIDY) after it,
# on Loupe's own C++. SYSTEM: autana's headers answer to autana's warnings.
include("${AUTANA_DIR}/scripts/cpp/standards.cmake")
autana_cpp_sanitize()
add_subdirectory("${AUTANA_DIR}/launcher/tools/render" autana SYSTEM)
add_subdirectory(third_party)
autana_cpp_checks()

# Everything but the command line. Autana's sources compile into this library
# alone; its usage requirements stay private, and only src/engine/autana.hpp
# includes autana's engine headers (tests/include_gate.cmake). autana's math
# and kinematics packages, below the engine, are public: src/engine/pose.hpp's
# poses cross into the window. Objects, not an archive: an archive would keep
# render_host.c's weak stand-ins and drop the scene manager's real definitions.
add_library(loupe_core OBJECT
    src/board/boards.cpp
    src/editor/camera_path.cpp
    src/editor/doc.cpp
    src/editor/placement.cpp
    src/editor/scene_tool.cpp
    src/engine/engine_frame.cpp
    src/engine/frames.cpp
    src/engine/path_curve.cpp
    src/library.cpp
    src/options.cpp
    src/pack.cpp
    src/paths.cpp
    src/process.cpp
    src/record/stream_saver.cpp
    src/scratch.cpp
    src/tools.cpp
    src/update/sha256.cpp
    src/update/update.cpp
    src/version.cpp)
target_include_directories(loupe_core PUBLIC src)
# The version, the channel and the commits, for version.cpp. The commits are
# read on every build, so they follow whatever moved Loupe or the submodule.
find_package(Git QUIET)
set(commits "${CMAKE_BINARY_DIR}/generated/commits.inc")
add_custom_target(commits BYPRODUCTS "${commits}"
    COMMAND "${CMAKE_COMMAND}" "-DGIT=${GIT_EXECUTABLE}" "-DSOURCE_DIR=${PROJECT_SOURCE_DIR}"
            "-DAUTANA_DIR=${AUTANA_DIR}" "-DOUT=${commits}" -P "${PROJECT_SOURCE_DIR}/cmake/commits.cmake"
    VERBATIM)
add_dependencies(loupe_core commits)
target_include_directories(loupe_core PRIVATE "${CMAKE_BINARY_DIR}/generated")
set_property(SOURCE src/version.cpp APPEND PROPERTY COMPILE_DEFINITIONS "LOUPE_VERSION=\"${PROJECT_VERSION}\""
             "LOUPE_CHANNEL=${LOUPE_CHANNEL}")
# Where updates come from: the repository's releases, and the rolling
# pre-release every push to main replaces (build.yml's publish job; tests/CMakeLists.txt checks they agree).
set(LOUPE_REPO jose-villegas/loupe)
set(LOUPE_MAIN_TAG main-build)
set_property(SOURCE src/update/update.cpp APPEND PROPERTY COMPILE_DEFINITIONS "LOUPE_REPO=\"${LOUPE_REPO}\""
             "LOUPE_MAIN_TAG=\"${LOUPE_MAIN_TAG}\"")
if(WIN32)
    # Every executable that links loupe_core, tests too, runs with UTF-8 as its code page.
    enable_language(RC)
    target_sources(loupe_core PRIVATE src/windows/utf8.rc)
endif()
# Scenes draw through the scene provider, recordings through the stream provider.
target_link_libraries(loupe_core PRIVATE autana::scene_provider autana::stream_provider)
foreach(package math kinematics)
    if(NOT TARGET ${package})
        message(FATAL_ERROR "autana at ${AUTANA_DIR} has no ${package} package target")
    endif()
endforeach()
target_link_libraries(loupe_core PUBLIC math kinematics)
target_link_libraries(loupe_core PUBLIC nlohmann_json::nlohmann_json mbedtls_sha256)
# The pack tool's own programs, and where it keeps packs; --compare also
# checks out revisions with git and lays the sides out with autana's
# render_compare.py, and the editor saves through autana's scene_edit.py.
# Only src/tools.cpp runs them.
if(LOUPE_PACK_TOOL)
    find_package(Git REQUIRED)
    set_property(SOURCE src/tools.cpp APPEND PROPERTY COMPILE_DEFINITIONS LOUPE_PACK_TOOL=1
                 "LOUPE_AUTANA_DIR=\"${AUTANA_DIR}\"" "LOUPE_PYTHON=\"${Python3_EXECUTABLE}\""
                 "LOUPE_GIT=\"${GIT_EXECUTABLE}\"" "LOUPE_PACK_CACHE=\"${CMAKE_BINARY_DIR}/packs\"")
else()
    set_property(SOURCE src/tools.cpp APPEND PROPERTY COMPILE_DEFINITIONS LOUPE_PACK_TOOL=0)
endif()
# Every pack the pinned autana's firmware carries, which a board's recording
# names, built with Loupe rather than when Record is pressed
# (cmake/firmware_packs.cmake) and installed beside the samples
# (cmake/package.cmake). Built wherever Python is, so a release carries them.
set(firmware_packs "")
if((LOUPE_PACK_TOOL OR LOUPE_SAMPLES) AND GIT_EXECUTABLE)
    set(firmware_packs "${CMAKE_BINARY_DIR}/firmware")
    add_custom_target(firmware_packs ALL
        COMMAND "${CMAKE_COMMAND}" "-DGIT=${GIT_EXECUTABLE}" "-DAUTANA_DIR=${AUTANA_DIR}"
                "-DPYTHON=${Python3_EXECUTABLE}" "-DOUT=${firmware_packs}"
                -P "${PROJECT_SOURCE_DIR}/cmake/firmware_packs.cmake"
        VERBATIM)
endif()
set_property(SOURCE src/pack.cpp APPEND PROPERTY COMPILE_DEFINITIONS "LOUPE_FIRMWARE_BUILT=\"${firmware_packs}\"")

# The window: SDL3, Vulkan through volk, Dear ImGui (third_party/CMakeLists.txt).
# Only runs that write no file open it, so those need no GPU.
add_library(loupe_window OBJECT
    src/window/editor.cpp
    src/window/files.cpp
    src/window/flow.cpp
    src/window/frame_texture.cpp
    src/window/gpu.cpp
    src/window/observer.cpp
    src/window/panels.cpp
    src/window/path_editor.cpp
    src/window/board_link.cpp
    src/window/run_window.cpp)
target_link_libraries(loupe_window PUBLIC loupe_core imgui imguizmo)
# The window's icon, the app logo, built into the binary as the bytes of its PNG.
set(icon_png "${CMAKE_CURRENT_SOURCE_DIR}/assets/loupe.png")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${icon_png}")
file(READ "${icon_png}" icon_bytes HEX)
string(REGEX REPLACE "(..)" "0x\\1," icon_bytes "${icon_bytes}")
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/generated/window/icon_png.inc" CONTENT "${icon_bytes}\n")
target_include_directories(loupe_window PRIVATE "${CMAKE_BINARY_DIR}/generated")

# A camera path's frame, and the A | B compare; PNGs through SDL.
add_library(loupe_path OBJECT src/path/path.cpp)
target_link_libraries(loupe_path PUBLIC loupe_core SDL3::SDL3-static)

add_executable(loupe src/main.cpp)
if(TARGET firmware_packs)
    add_dependencies(loupe firmware_packs)
endif()
target_link_libraries(loupe PRIVATE loupe_core loupe_path loupe_window)

# loupe-storyboard: a recipe's loupe.* steps, run on the real window through
# the test engine (src/storyboard/).
if(LOUPE_STORYBOARD)
    add_library(loupe_recipe OBJECT src/storyboard/recipe.cpp)
    target_link_libraries(loupe_recipe PUBLIC nlohmann_json::nlohmann_json PRIVATE yaml-cpp::yaml-cpp)
    target_include_directories(loupe_recipe PUBLIC src)
    add_executable(loupe-storyboard src/storyboard/main.cpp src/storyboard/runner.cpp)
    target_link_libraries(loupe-storyboard PRIVATE loupe_core loupe_window loupe_recipe yaml-cpp::yaml-cpp)
endif()

# docs/shots, the docs' pictures of the window, drawn again by the recipes in
# tests/stories/doc_shots offscreen (tests/doc_shots.cmake); the
# doc_shots_current test checks them. The recipes open .scene.toml files.
if(LOUPE_STORYBOARD AND LOUPE_PACK_TOOL)
    add_custom_target(doc_shots
                      COMMAND "${CMAKE_COMMAND}" "-DSTORYBOARD=$<TARGET_FILE:loupe-storyboard>"
                              "-DRECIPES=${PROJECT_SOURCE_DIR}/tests/stories/doc_shots"
                              "-DDOCS=${PROJECT_SOURCE_DIR}/docs/shots" "-DOUT=${CMAKE_BINARY_DIR}/doc_shots"
                              -P "${PROJECT_SOURCE_DIR}/tests/doc_shots.cmake"
                      DEPENDS loupe-storyboard USES_TERMINAL VERBATIM)
endif()

include(cmake/package.cmake)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
