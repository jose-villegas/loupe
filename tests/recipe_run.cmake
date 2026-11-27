# Runs the recipe RECIPE with loupe-storyboard (STORYBOARD) offscreen
# (offscreen.cmake), its pictures into OUT/shots; a failed step fails it.
# The pictures are kept for a look, not checked.
cmake_minimum_required(VERSION 3.25)
include("${CMAKE_CURRENT_LIST_DIR}/offscreen.cmake")
draw_offscreen("${OUT}/data" FALSE)
file(REMOVE_RECURSE "${OUT}/shots")
execute_process(COMMAND "${STORYBOARD}" --recipe "${RECIPE}" -o "${OUT}/shots" --scale 1.5 RESULT_VARIABLE status
                OUTPUT_VARIABLE out ERROR_VARIABLE out)
message("${out}")
if(NOT status EQUAL 0)
    message(FATAL_ERROR "loupe-storyboard --recipe ${RECIPE} on $ENV{VK_DRIVER_FILES}: exit ${status}")
endif()
