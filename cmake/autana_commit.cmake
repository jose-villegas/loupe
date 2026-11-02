# Writes OUT, the autana commit Loupe compiles, as LOUPE_AUTANA_COMMIT from
# AUTANA_DIR's git (GIT), or "unknown" without one. Run on every build
# (CMakeLists.txt), and rewritten only when the commit changed.
set(commit unknown)
if(GIT)
    execute_process(COMMAND "${GIT}" -C "${AUTANA_DIR}" rev-parse --short=9 HEAD OUTPUT_VARIABLE head
                    OUTPUT_STRIP_TRAILING_WHITESPACE RESULT_VARIABLE status ERROR_QUIET)
    if(status EQUAL 0)
        set(commit "${head}")
    endif()
endif()
file(CONFIGURE OUTPUT "${OUT}" CONTENT "#define LOUPE_AUTANA_COMMIT \"@commit@\"\n" @ONLY)
