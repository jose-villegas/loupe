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
# includes autana's headers (tests/include_gate.cmake). Objects, not an
# archive: an archive would keep render_host.c's weak stand-ins and drop the
# scene manager's real definitions.
add_library(loupe_core OBJECT
    src/engine/frames.cpp
    src/library.cpp
    src/options.cpp
    src/pack.cpp
    src/paths.cpp
    src/process.cpp
    src/version.cpp)
target_include_directories(loupe_core PUBLIC src)
# The autana commit compiled, for version.cpp; configure runs again when the submodule moves.
find_package(Git QUIET)
set(autana_commit unknown)
if(Git_FOUND)
    execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${AUTANA_DIR}" rev-parse --short=9 HEAD
                    OUTPUT_VARIABLE autana_commit OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${AUTANA_DIR}" rev-parse --absolute-git-dir
                    OUTPUT_VARIABLE autana_git_dir OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    if(EXISTS "${autana_git_dir}/HEAD")
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${autana_git_dir}/HEAD")
    endif()
endif()
set_property(SOURCE src/version.cpp APPEND PROPERTY COMPILE_DEFINITIONS
             "LOUPE_VERSION=\"${PROJECT_VERSION}\"" "LOUPE_AUTANA_COMMIT=\"${autana_commit}\"")
if(WIN32)
    # Every executable that links loupe_core, tests too, runs with UTF-8 as its code page.
    enable_language(RC)
    target_sources(loupe_core PRIVATE src/windows/utf8.rc)
endif()
target_link_libraries(loupe_core PRIVATE autana::scene_provider)
if(LOUPE_PACK_TOOL)
    target_compile_definitions(loupe_core PRIVATE LOUPE_PACK_TOOL=1
        "LOUPE_AUTANA_DIR=\"${AUTANA_DIR}\"" "LOUPE_PYTHON=\"${Python3_EXECUTABLE}\""
        "LOUPE_PACK_CACHE=\"${CMAKE_BINARY_DIR}/packs\"")
else()
    target_compile_definitions(loupe_core PRIVATE LOUPE_PACK_TOOL=0)
endif()

# The window: SDL3, Vulkan through volk, Dear ImGui (third_party/CMakeLists.txt).
# Only runs without -o or --video open it, so those still need no GPU.
add_library(loupe_window OBJECT
    src/window/files.cpp
    src/window/frame_texture.cpp
    src/window/gpu.cpp
    src/window/panels.cpp
    src/window/run_window.cpp)
target_link_libraries(loupe_window PUBLIC loupe_core imgui)
# The window's icon, the app logo, built into the binary as the bytes of its PNG.
set(icon_png "${CMAKE_CURRENT_SOURCE_DIR}/assets/loupe.png")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${icon_png}")
file(READ "${icon_png}" icon_bytes HEX)
string(REGEX REPLACE "(..)" "0x\\1," icon_bytes "${icon_bytes}")
file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/generated/window/icon_png.inc" CONTENT "${icon_bytes}\n")
target_include_directories(loupe_window PRIVATE "${CMAKE_BINARY_DIR}/generated")

add_executable(loupe src/main.cpp)
target_link_libraries(loupe PRIVATE loupe_core loupe_window)

include(cmake/package.cmake)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
