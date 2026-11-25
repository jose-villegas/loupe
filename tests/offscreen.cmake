# How loupe's window draws with no display and no GPU: SDL's offscreen video
# driver and Mesa's lavapipe, Vulkan on the CPU, with the Khronos validation
# layer watching every Vulkan call where it is installed
# (tools/ci/linux-packages.txt installs both).

set(VALIDATION_LAYER "/usr/share/vulkan/explicit_layer.d/VkLayer_khronos_validation.json")

# Sets `out` to lavapipe's Vulkan driver file, or to nothing where it is not installed.
function(find_lavapipe out)
    file(GLOB icds "/usr/share/vulkan/icd.d/lvp_icd*.json")
    set(icd "")
    if(icds)
        list(GET icds 0 icd)
    endif()
    set(${out} "${icd}" PARENT_SCOPE)
endfunction()

# draw_offscreen(DATA NEED_LAYER): sets this script's environment to draw
# offscreen, the window's recent list in DATA, not the user's. Without
# lavapipe, or without the validation layer when NEED_LAYER, it says SKIP
# (tests/CMakeLists.txt skips on it) and returns from the calling script,
# being a macro.
macro(draw_offscreen data need_layer)
    find_lavapipe(icd)
    if(NOT icd OR (${need_layer} AND NOT EXISTS "${VALIDATION_LAYER}"))
        message("SKIP: needs Mesa's lavapipe and the Khronos validation layer (tools/ci/linux-packages.txt)")
        return()
    endif()
    set(ENV{SDL_VIDEO_DRIVER} offscreen)
    set(ENV{VK_DRIVER_FILES} "${icd}")
    if(EXISTS "${VALIDATION_LAYER}")
        set(ENV{VK_INSTANCE_LAYERS} VK_LAYER_KHRONOS_validation)
    endif()
    set(ENV{XDG_DATA_HOME} "${data}")
endmacro()

# Fails the script when the validation layer reported an error in `out`.
function(fail_on_validation out)
    if(out MATCHES "Validation Error")
        message(FATAL_ERROR "the validation layer reported an error, above")
    endif()
endfunction()
