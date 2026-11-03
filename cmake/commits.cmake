# Writes OUT, the commits a build compiles: Loupe's (SOURCE_DIR) as
# LOUPE_COMMIT and autana's (AUTANA_DIR) as LOUPE_AUTANA_COMMIT, from git (GIT),
# each "unknown" without one. Run on every build (CMakeLists.txt), and
# rewritten only when a commit changed.
foreach(name IN ITEMS SOURCE_DIR AUTANA_DIR)
    set(${name}_commit unknown)
    if(GIT)
        execute_process(COMMAND "${GIT}" -C "${${name}}" rev-parse --short=9 HEAD OUTPUT_VARIABLE head
                        OUTPUT_STRIP_TRAILING_WHITESPACE RESULT_VARIABLE status ERROR_QUIET)
        if(status EQUAL 0)
            set(${name}_commit "${head}")
        endif()
    endif()
endforeach()
file(CONFIGURE OUTPUT "${OUT}" CONTENT "#define LOUPE_COMMIT \"@SOURCE_DIR_commit@\"
#define LOUPE_AUTANA_COMMIT \"@AUTANA_DIR_commit@\"
" @ONLY)
