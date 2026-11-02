# Opens the window on SCENE, or on the start panel when SCENE is empty, for
# TICKS ticks under SDL's offscreen video driver and Mesa's lavapipe, Vulkan
# on the CPU, so it needs no display and no GPU. The Khronos validation layer
# watches every Vulkan call; one error fails it. Its recent list goes in OUT,
# not the user's. Says SKIP where either is not installed (tests/CMakeLists.txt skips on it).
file(GLOB icds "/usr/share/vulkan/icd.d/lvp_icd*.json")
set(layer "/usr/share/vulkan/explicit_layer.d/VkLayer_khronos_validation.json")
if(NOT icds OR NOT EXISTS "${layer}")
    message("SKIP: needs Mesa's lavapipe and the Khronos validation layer (tools/ci/linux-packages.txt)")
    return()
endif()
list(GET icds 0 icd)
file(REMOVE_RECURSE "${OUT}")
set(ENV{SDL_VIDEO_DRIVER} offscreen)
set(ENV{VK_DRIVER_FILES} "${icd}")
set(ENV{VK_INSTANCE_LAYERS} VK_LAYER_KHRONOS_validation)
set(ENV{XDG_DATA_HOME} "${OUT}")
# Unquoted, an empty SCENE passes no argument.
execute_process(COMMAND "${LOUPE}" ${SCENE} --ticks "${TICKS}" RESULT_VARIABLE status OUTPUT_VARIABLE out
                ERROR_VARIABLE out)
message("${out}")
if(NOT status EQUAL 0)
    message(FATAL_ERROR "loupe ${SCENE} --ticks ${TICKS} on ${icd}: exit ${status}")
endif()
if(out MATCHES "Validation Error")
    message(FATAL_ERROR "the validation layer reported an error, above")
endif()
