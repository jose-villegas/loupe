# Runs LOUPE on SCENE with --compare COMPARE, B_ARGS as --b-args when given,
# over FRAMES frames at --dt DT into OUT/compare.mp4. Fails unless it wrote the
# video and OUT/compare.csv, render_compare.py's numbers per frame, says that
# every frame's difference is nothing (EXPECT same) or something (EXPECT
# differ). Says SKIP without ffmpeg or the Python modules render_compare.py
# imports (tests/CMakeLists.txt skips on it).
execute_process(COMMAND "${PYTHON}" -c "import numpy, PIL" RESULT_VARIABLE no_modules OUTPUT_QUIET ERROR_QUIET)
find_program(ffmpeg ffmpeg)
if(no_modules OR NOT ffmpeg)
    message("SKIP: needs ffmpeg, and numpy and Pillow in ${PYTHON}")
    return()
endif()
file(REMOVE_RECURSE "${OUT}")
file(MAKE_DIRECTORY "${OUT}")
set(b_args)
if(B_ARGS)
    set(b_args --b-args "${B_ARGS}")
endif()
execute_process(COMMAND "${LOUPE}" "${SCENE}" --compare "${COMPARE}" ${b_args} --frames "${FRAMES}" --dt "${DT}"
                        --video "${OUT}/compare.mp4" RESULT_VARIABLE status)
if(NOT status EQUAL 0)
    message(FATAL_ERROR "loupe exited with ${status}")
endif()
file(SIZE "${OUT}/compare.mp4" size)
if(size EQUAL 0)
    message(FATAL_ERROR "an empty ${OUT}/compare.mp4")
endif()

# frame,time_ms,changed_pct,mean_abs,holes_a,holes_b: one row a frame, after the header.
file(STRINGS "${OUT}/compare.csv" rows)
list(POP_FRONT rows)
list(LENGTH rows count)
if(NOT count EQUAL FRAMES)
    message(FATAL_ERROR "${count} rows in compare.csv, not ${FRAMES}")
endif()
foreach(row IN LISTS rows)
    string(REPLACE "," ";" cells "${row}")
    list(GET cells 2 changed)
    if(EXPECT STREQUAL "same" AND NOT changed STREQUAL "0.0000")
        message(FATAL_ERROR "A and B differ: ${row}")
    elseif(EXPECT STREQUAL "differ" AND changed STREQUAL "0.0000")
        message(FATAL_ERROR "A and B are the same: ${row}")
    endif()
endforeach()
message(STATUS "every frame ${EXPECT}: ${OUT}/compare.csv")
