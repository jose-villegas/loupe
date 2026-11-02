# Runs LOUPE on SCENE with -o into OUT and fails unless it exits 1 and says
# REASON, a regex, on stderr: a scene loupe refuses, with the reason why.
file(REMOVE_RECURSE "${OUT}")
file(MAKE_DIRECTORY "${OUT}")
execute_process(COMMAND "${LOUPE}" "${SCENE}" -o "${OUT}/out.bmp" RESULT_VARIABLE status ERROR_VARIABLE err)
message("${err}")
if(NOT status EQUAL 1)
    message(FATAL_ERROR "loupe ${SCENE}: exit ${status}, not 1")
endif()
if(NOT err MATCHES "${REASON}")
    message(FATAL_ERROR "loupe ${SCENE} did not say: ${REASON}")
endif()
