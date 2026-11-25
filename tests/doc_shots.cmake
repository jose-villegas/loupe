# Draws every doc shot (src/window/doc_shots.cpp) offscreen (offscreen.cmake)
# into OUT. That is the one way the pictures in DOCS are drawn, so the same
# Mesa draws the same pixels on any machine. Without CHECK, they then replace
# DOCS' pictures: the doc_shots target. With CHECK, each must hold the same
# pixels as its copy in DOCS (SAME_IMAGE), and DOCS must hold no picture no
# shot draws: the doc_shots_current test.
cmake_minimum_required(VERSION 3.25)
include("${CMAKE_CURRENT_LIST_DIR}/offscreen.cmake")
set(regenerate "cmake --build <build> --target doc_shots, on Linux with lavapipe (docs/development/Tests.md)")
# The pictures are of autana's scenes as the pinned commit draws them; another
# autana (the nightly's main, or -DAUTANA_DIR) can draw other pixels.
execute_process(COMMAND git -C "${SOURCE}" rev-parse HEAD:third_party/autana OUTPUT_VARIABLE pinned
                OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
execute_process(COMMAND git -C "${AUTANA}" rev-parse HEAD OUTPUT_VARIABLE checked_out OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET)
if(CHECK AND NOT pinned STREQUAL checked_out)
    message("SKIP: autana is at ${checked_out}, not the pinned ${pinned}")
    return()
endif()
find_lavapipe(icd)
if(NOT CHECK AND NOT icd)
    message(FATAL_ERROR "the doc shots draw with Mesa's lavapipe, which is Linux's: run ${regenerate}")
endif()
draw_offscreen("${OUT}/data" FALSE)
file(REMOVE_RECURSE "${OUT}")
execute_process(COMMAND "${LOUPE}" --shot all -o "${OUT}/shots" RESULT_VARIABLE status OUTPUT_VARIABLE out
                ERROR_VARIABLE out)
message("${out}")
if(NOT status EQUAL 0)
    message(FATAL_ERROR "loupe --shot all on ${icd}: exit ${status}")
endif()
fail_on_validation("${out}")
file(GLOB drawn RELATIVE "${OUT}/shots" "${OUT}/shots/*.png")
file(GLOB kept RELATIVE "${DOCS}" "${DOCS}/*.png")

if(NOT CHECK)
    # Copied over first, then only the pictures no shot draws removed: a failed copy loses none.
    file(COPY "${OUT}/shots/" DESTINATION "${DOCS}")
    foreach(name IN LISTS kept)
        if(NOT name IN_LIST drawn)
            file(REMOVE "${DOCS}/${name}")
        endif()
    endforeach()
    list(JOIN drawn ", " drawn)
    message("drew ${drawn} into ${DOCS}")
    return()
endif()

# lavapipe compiles its shaders for the CPU it runs on: two CI runners have
# drawn a panel tab's antialiased edges one level apart, so a channel may be off by this much.
set(LAVAPIPE_LEVELS 1)
set(stale "")
foreach(name IN LISTS drawn)
    if(NOT name IN_LIST kept)
        list(APPEND stale "${name} (missing)")
        continue()
    endif()
    execute_process(COMMAND "${SAME_IMAGE}" --within ${LAVAPIPE_LEVELS} "${DOCS}/${name}" "${OUT}/shots/${name}"
                    RESULT_VARIABLE same)
    if(NOT same EQUAL 0)
        list(APPEND stale "${name}")
    endif()
endforeach()
foreach(name IN LISTS kept)
    if(NOT name IN_LIST drawn)
        list(APPEND stale "${name} (no shot draws it)")
    endif()
endforeach()
if(stale)
    list(JOIN stale ", " stale)
    message(FATAL_ERROR "${DOCS} is not what the shots draw: ${stale}. Redraw them with ${regenerate}; "
                        "CI uploads the pictures it drew (${OUT}/shots) when this fails.")
endif()
