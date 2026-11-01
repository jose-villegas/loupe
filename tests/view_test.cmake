# Runs VIEW_TEST switched once, then straight for every view it names, and
# fails unless each view's two frames are the same bytes both ways.
file(REMOVE_RECURSE "${OUT}")
file(MAKE_DIRECTORY "${OUT}")
execute_process(COMMAND "${VIEW_TEST}" "${SCENE}" "${OUT}" switched RESULT_VARIABLE status)
if(NOT status EQUAL 0)
    message(FATAL_ERROR "view_test switched exited with ${status}")
endif()
file(STRINGS "${OUT}/views.txt" views)
foreach(view IN LISTS views)
    execute_process(COMMAND "${VIEW_TEST}" "${SCENE}" "${OUT}" straight "${view}" RESULT_VARIABLE status)
    if(NOT status EQUAL 0)
        message(FATAL_ERROR "view_test straight ${view} exited with ${status}")
    endif()
    # picked: the shown frame drawn again under the view; sought: an earlier one, restored and stepped.
    foreach(frame picked sought)
        execute_process(COMMAND "${CMAKE_COMMAND}" -E compare_files "${OUT}/straight-${view}-${frame}.bin"
                                "${OUT}/switched-${view}-${frame}.bin" RESULT_VARIABLE differ)
        if(differ)
            message(FATAL_ERROR "${view}: the ${frame} frame differs from the one drawn under it from the start: ${OUT}")
        endif()
    endforeach()
    message(STATUS "same frames under ${view}")
endforeach()
