# Runs LOUPE on SCENE with --frame FRAME at --dt DT, and with COMPARE also
# --compare COMPARE and B_ARGS as --b-args when given, writing OUT/pair.png
# (and OUT/pair-b.png). Fails unless each holds the pixels a straight run of
# PACK to that frame writes with -o: A's under ARGS, B's under B_ARGS or
# ARGS; and with B_ARGS, unless the two differ. ARGS are |-separated.
string(REPLACE "|" ";" args "${ARGS}")
file(REMOVE_RECURSE "${OUT}")
file(MAKE_DIRECTORY "${OUT}")

# `name`'s run of loupe with the rest of the arguments, failing unless it exits 0.
function(loupe name)
    execute_process(COMMAND "${LOUPE}" ${ARGN} RESULT_VARIABLE status)
    if(NOT status EQUAL 0)
        message(FATAL_ERROR "${name}: loupe exited with ${status}")
    endif()
endfunction()

# Fails unless SAME_IMAGE says what the rest of its arguments ask.
function(same_image)
    execute_process(COMMAND "${SAME_IMAGE}" ${ARGN} RESULT_VARIABLE status)
    if(NOT status EQUAL 0)
        message(FATAL_ERROR "same_image ${ARGN}: exit ${status}")
    endif()
endfunction()

set(compare)
if(COMPARE)
    set(compare --compare "${COMPARE}")
    if(B_ARGS)
        list(APPEND compare --b-args "${B_ARGS}")
    endif()
endif()
loupe(pair "${SCENE}" ${args} --dt "${DT}" --frame "${FRAME}" ${compare} -o "${OUT}/pair.png")

math(EXPR frames "${FRAME} + 1")
loupe(straight_a "${PACK}" ${args} --frames ${frames} --dt "${DT}" -o "${OUT}/a.bmp")
same_image("${OUT}/pair.png" "${OUT}/a.bmp")
if(NOT COMPARE)
    return()
endif()
set(b_args ${args})
if(B_ARGS)
    separate_arguments(b_args NATIVE_COMMAND "${B_ARGS}")
endif()
loupe(straight_b "${PACK}" ${b_args} --frames ${frames} --dt "${DT}" -o "${OUT}/b.bmp")
same_image("${OUT}/pair-b.png" "${OUT}/b.bmp")
if(B_ARGS)
    same_image(--differ "${OUT}/pair.png" "${OUT}/pair-b.png")
endif()
