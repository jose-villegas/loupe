# Builds OUT, every pack a development firmware of the autana at AUTANA_DIR
# carries, with its build_pack.py under PYTHON, and stamps it with that
# autana's commit (git: GIT) in OUT/built-for: the packs a board's recording
# names, ready before Record is pressed. Run on every build (CMakeLists.txt);
# it builds only when the stamp names another commit, so a build that did not
# move the pin pays one git call.
execute_process(COMMAND "${GIT}" -C "${AUTANA_DIR}" rev-parse --short=9 HEAD OUTPUT_VARIABLE commit
                OUTPUT_STRIP_TRAILING_WHITESPACE RESULT_VARIABLE status ERROR_QUIET)
if(NOT status EQUAL 0)
    message(STATUS "firmware packs: ${AUTANA_DIR} has no commit; Record builds them when asked")
    return()
endif()
set(stamp "${OUT}/built-for")
if(EXISTS "${stamp}")
    file(READ "${stamp}" built)
    if(built STREQUAL commit)
        return()
    endif()
endif()
message(STATUS "firmware packs: building autana ${commit}'s into ${OUT}")
file(REMOVE "${stamp}")
# No scene named: build_pack.py builds every pack the firmware carries.
execute_process(COMMAND "${PYTHON}" "${AUTANA_DIR}/launcher/tools/r3d/build_pack.py" -o "${OUT}"
                RESULT_VARIABLE status)
if(NOT status EQUAL 0)
    message(FATAL_ERROR "firmware packs: build_pack.py failed for autana ${commit}")
endif()
file(WRITE "${stamp}" "${commit}")
