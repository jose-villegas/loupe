# Runs LOUPE on ARGS (|-separated) in OUT and fails unless it exits EXIT and
# says REASON, a regex, on stderr: a command loupe refuses, with the reason why.
string(REPLACE "|" ";" args "${ARGS}")
file(REMOVE_RECURSE "${OUT}")
file(MAKE_DIRECTORY "${OUT}")
execute_process(COMMAND "${LOUPE}" ${args} WORKING_DIRECTORY "${OUT}" RESULT_VARIABLE status ERROR_VARIABLE err)
message("${err}")
if(NOT status EQUAL EXIT)
    message(FATAL_ERROR "loupe ${args}: exit ${status}, not ${EXIT}")
endif()
if(NOT err MATCHES "${REASON}")
    message(FATAL_ERROR "loupe ${args} did not say: ${REASON}")
endif()
