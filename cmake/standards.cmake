# The compiler half of autana's C++ rules (third_party/autana/docs/c-style/Cpp.md):
# warnings as errors on Loupe's C++, sanitizers and clang-tidy as options.
# Included before autana's subdirectory, so a sanitizer build covers its C too.

add_library(loupe_warnings INTERFACE)
target_compile_options(loupe_warnings INTERFACE
    "$<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wpedantic;-Werror;-Werror=vla;-ffp-contract=off;-Wshadow;-Wnon-virtual-dtor;-Wold-style-cast;-Woverloaded-virtual;-Wcast-align;-Wnull-dereference;-Wdouble-promotion;-Wimplicit-fallthrough;-Wextra-semi;-Wdeprecated;-Wzero-as-null-pointer-constant;-Wsuggest-override>")

option(LOUPE_SANITIZE "Build everything with UndefinedBehaviorSanitizer, and AddressSanitizer on Linux" OFF)
if(LOUPE_SANITIZE)
    # autana's host tests' choice: UBSan with its runtime when the toolchain
    # links one, trapping on the first error when it does not (MinGW).
    include(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_FLAGS -fsanitize=undefined)
    set(CMAKE_REQUIRED_LINK_OPTIONS -fsanitize=undefined)
    check_cxx_source_compiles("int main() { return 0; }" LOUPE_UBSAN_RUNTIME)
    unset(CMAKE_REQUIRED_FLAGS)
    unset(CMAKE_REQUIRED_LINK_OPTIONS)
    if(LOUPE_UBSAN_RUNTIME)
        set(sanitizers -fsanitize=undefined -fno-sanitize-recover=all)
        if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            list(APPEND sanitizers -fsanitize=address -fno-omit-frame-pointer)
        endif()
        add_link_options(${sanitizers})
    else()
        set(sanitizers -fsanitize=undefined -fsanitize-undefined-trap-on-error)
    endif()
    add_compile_options(${sanitizers})
    message(STATUS "Sanitizers: ${sanitizers}")
endif()

option(LOUPE_CLANG_TIDY "Run clang-tidy (.clang-tidy) on every C++ file Loupe compiles" OFF)
if(LOUPE_CLANG_TIDY)
    # The LLVM major autana pins for clang-format and clang-tidy alike.
    execute_process(COMMAND "${Python3_EXECUTABLE}" "${AUTANA_DIR}/scripts/lib/pinned_tool.py" clang-tidy
                    OUTPUT_VARIABLE tidy RESULT_VARIABLE tidy_status)
    if(NOT tidy_status EQUAL 0)
        message(FATAL_ERROR "No clang-tidy at autana's pinned LLVM major")
    endif()
    string(STRIP "${tidy}" tidy)
    string(REGEX REPLACE "^[^\n]*\n" "" tidy "${tidy}")
    message(STATUS "clang-tidy: ${tidy}")
endif()

# Loupe's own C++ targets: the warnings, and clang-tidy when it is on.
function(loupe_standards target)
    target_link_libraries(${target} PRIVATE loupe_warnings)
    if(LOUPE_CLANG_TIDY)
        set_target_properties(${target} PROPERTIES CXX_CLANG_TIDY "${tidy}")
    endif()
endfunction()
