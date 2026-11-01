# Opens the window on SCENE for TICKS ticks under SDL's offscreen video driver
# and Mesa's lavapipe, Vulkan on the CPU, so it needs no display and no GPU.
# Says SKIP where no lavapipe is installed (tests/CMakeLists.txt skips on it).
file(GLOB icds "/usr/share/vulkan/icd.d/lvp_icd*.json")
if(NOT icds)
    message("SKIP: no Mesa lavapipe (/usr/share/vulkan/icd.d/lvp_icd*.json) to draw the window with")
    return()
endif()
list(GET icds 0 icd)
set(ENV{SDL_VIDEO_DRIVER} offscreen)
set(ENV{VK_DRIVER_FILES} "${icd}")
execute_process(COMMAND "${LOUPE}" "${SCENE}" --ticks "${TICKS}" RESULT_VARIABLE status)
if(NOT status EQUAL 0)
    message(FATAL_ERROR "loupe ${SCENE} --ticks ${TICKS} on ${icd}: exit ${status}")
endif()
