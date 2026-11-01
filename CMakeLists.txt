cmake_minimum_required(VERSION 3.25...3.31)
project(loupe LANGUAGES C CXX)

set(AUTANA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/autana" CACHE PATH
    "The autana checkout Loupe builds against; the submodule by default")
if(NOT EXISTS "${AUTANA_DIR}/launcher/tools/render/CMakeLists.txt")
    message(FATAL_ERROR "No autana at ${AUTANA_DIR}: run `git submodule update --init` or pass -DAUTANA_DIR=<path>")
endif()
file(STRINGS "${AUTANA_DIR}/scripts/lib/python-min-version" python_min LIMIT_COUNT 1)
find_package(Python3 ${python_min} REQUIRED COMPONENTS Interpreter)

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

include(cmake/standards.cmake)
# SYSTEM: autana's headers answer to autana's warnings, not Loupe's.
add_subdirectory("${AUTANA_DIR}/launcher/tools/render" autana SYSTEM)

# Everything but the command line. Autana's sources compile into this library
# alone; its usage requirements stay private, and only src/engine/autana.hpp
# includes autana's headers (tests/include_gate.cmake). Objects, not an
# archive: an archive would keep render_host.c's weak stand-ins and drop the
# scene manager's real definitions.
add_library(loupe_core OBJECT
    src/engine/frames.cpp
    src/options.cpp
    src/pack.cpp)
target_include_directories(loupe_core PUBLIC src)
target_link_libraries(loupe_core PRIVATE autana::scene_provider)
loupe_standards(loupe_core)
target_compile_definitions(loupe_core PRIVATE
    "LOUPE_AUTANA_DIR=\"${AUTANA_DIR}\"" "LOUPE_PYTHON=\"${Python3_EXECUTABLE}\""
    "LOUPE_PACK_CACHE=\"${CMAKE_BINARY_DIR}/packs\"")

add_executable(loupe src/main.cpp)
target_link_libraries(loupe PRIVATE loupe_core)
loupe_standards(loupe)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
