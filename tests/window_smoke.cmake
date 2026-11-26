# Opens the window on SCENE, or on the start panel when SCENE is empty, for
# TICKS ticks offscreen (offscreen.cmake) under the validation layer; one
# error fails it. Its recent list goes in OUT, not the user's.
include("${CMAKE_CURRENT_LIST_DIR}/offscreen.cmake")
draw_offscreen("${OUT}" TRUE)
file(REMOVE_RECURSE "${OUT}")
# Unquoted, an empty SCENE passes no argument.
execute_process(COMMAND "${LOUPE}" ${SCENE} --ticks "${TICKS}" RESULT_VARIABLE status OUTPUT_VARIABLE out
                ERROR_VARIABLE out)
message("${out}")
if(NOT status EQUAL 0)
    message(FATAL_ERROR "loupe ${SCENE} --ticks ${TICKS} on $ENV{VK_DRIVER_FILES}: exit ${status}")
endif()
fail_on_validation("${out}")
