set(demo "${AUTANA_DIR}/launcher/demo")
set(sponza_toml "${demo}/sponza/sponza.scene.toml")
find_program(SH NAMES sh REQUIRED)
# The tests build their packs with autana's build_pack.py, even where loupe itself has no pack tool.
find_package(Python3 ${python_min} REQUIRED COMPONENTS Interpreter)

# The packs the tests open, built once: every test but parity_scene_toml opens
# a .apak, so the suite runs in builds without the pack tool too.
set(packs "${CMAKE_CURRENT_BINARY_DIR}/packs")
set(sponza "${packs}/sponza.apak")
set(water "${packs}/water.apak")
add_test(NAME packs COMMAND "${Python3_EXECUTABLE}" "${AUTANA_DIR}/launcher/tools/r3d/build_pack.py" -o "${packs}"
                            "${sponza_toml}" "${demo}/water/water.scene.toml")
set_tests_properties(packs PROPERTIES FIXTURES_SETUP packs)

# A test on the packs, one at a time: each run sets the process's asset dir and draws the same scene.
function(on_packs test)
    set_tests_properties(${test} PROPERTIES FIXTURES_REQUIRED packs RESOURCE_LOCK sponza)
endfunction()

# The include path loupe_core compiles with, in order, for the gate to resolve includes against.
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/include_dirs.txt"
     CONTENT "$<JOIN:$<TARGET_PROPERTY:loupe_core,INCLUDE_DIRECTORIES>,\n>\n")
add_test(NAME include_gate COMMAND "${CMAKE_COMMAND}" "-DSOURCE_DIR=${PROJECT_SOURCE_DIR}" "-DAUTANA_DIR=${AUTANA_DIR}"
         "-DINCLUDE_DIRS=${CMAKE_BINARY_DIR}/include_dirs.txt" -P "${CMAKE_CURRENT_SOURCE_DIR}/include_gate.cmake")

# loupe on SCENE and autana's scene_viewer.sh on sponza's .scene.toml, same arguments; ARGS are |-separated.
function(parity name scene args)
    add_test(NAME parity_${name} COMMAND "${CMAKE_COMMAND}" "-DLOUPE=$<TARGET_FILE:loupe>" "-DSH=${SH}"
             "-DAUTANA_DIR=${AUTANA_DIR}" "-DVIEWER_SCENE=${sponza_toml}" "-DSCENE=${scene}" "-DARGS=${args}"
             "-DOUT=${CMAKE_CURRENT_BINARY_DIR}/parity_${name}" -P "${CMAKE_CURRENT_SOURCE_DIR}/parity.cmake")
    on_packs(parity_${name})
endfunction()
parity(defaults "${sponza}" "")
parity(every_flag "${sponza}"
       "--object|atrium|--camera|tour|--view|meshlets|--set|render.batched=0|--frames|4|--dt|50|--video")

# loupe on ARGS (|-separated, run in the test's own folder) exits EXIT, saying REASON (a regex) on stderr.
function(refused name args exit reason)
    add_test(NAME ${name} COMMAND "${CMAKE_COMMAND}" "-DLOUPE=$<TARGET_FILE:loupe>" "-DARGS=${args}" "-DEXIT=${exit}"
             "-DREASON=${reason}" "-DOUT=${CMAKE_CURRENT_BINARY_DIR}/${name}"
             -P "${CMAKE_CURRENT_SOURCE_DIR}/refused.cmake")
    on_packs(${name})
endfunction()
if(LOUPE_PACK_TOOL)
    parity(scene_toml "${sponza_toml}" "")
else()
    refused(scene_toml_refused "${sponza_toml}|-o|out.bmp" 1 "built without the pack tool")
endif()

# A pack of the next format version, refused with autana's own reason, read from its source.
file(STRINGS "${AUTANA_DIR}/launcher/main/asset/asset_pack.h" version REGEX "#define ASSET_PACK_VERSION ")
string(REGEX REPLACE ".*ASSET_PACK_VERSION +([0-9]+)U?.*" "\\1" version "${version}")
math(EXPR next_version "${version} + 1")
file(STRINGS "${AUTANA_DIR}/launcher/main/asset/asset_pack.c" reason REGEX "case ASSET_ERR_VERSION: return")
string(REGEX REPLACE ".*return \"([^\"]+)\".*" "\\1" reason "${reason}")
set(next_pack "${CMAKE_CURRENT_BINARY_DIR}/next_version/sponza.apak")
add_test(NAME next_version_pack COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/set_pack_version.py"
                                        "${sponza}" "${next_version}" "${next_pack}")
set_tests_properties(next_version_pack PROPERTIES FIXTURES_REQUIRED packs FIXTURES_SETUP next_version)
refused(wrong_version_refused "${next_pack}|-o|out.bmp" 1 "sponza.apak: ${reason}")
set_tests_properties(wrong_version_refused PROPERTIES FIXTURES_REQUIRED "packs;next_version")

# --frame and --compare (src/path): one frame of the camera path, alone or for
# both sides, is what a straight run to it writes; a compare video's difference
# is nothing against the same pack and something under other flags.
add_executable(same_image same_image.cpp)
target_link_libraries(same_image PRIVATE SDL3::SDL3-static)
add_executable(path_timing_test path_timing_test.cpp)
target_link_libraries(path_timing_test PRIVATE loupe_core loupe_path)
add_test(NAME path_timing COMMAND path_timing_test)
# --frame FRAME on SCENE at --dt 50 under ARGS, with COMPARE and B_ARGS when given (tests/path_frame.cmake).
function(path_frame name scene args compare b_args)
    add_test(NAME ${name} COMMAND "${CMAKE_COMMAND}" "-DLOUPE=$<TARGET_FILE:loupe>"
             "-DSAME_IMAGE=$<TARGET_FILE:same_image>" "-DSCENE=${scene}" "-DPACK=${sponza}" "-DARGS=${args}"
             -DFRAME=3 -DDT=50 "-DCOMPARE=${compare}" "-DB_ARGS=${b_args}" "-DGIT=${GIT_EXECUTABLE}"
             "-DOUT=${CMAKE_CURRENT_BINARY_DIR}/${name}" -P "${CMAKE_CURRENT_SOURCE_DIR}/path_frame.cmake")
    on_packs(${name})
endfunction()
path_frame(frame "${sponza}" "--view|meshlets" "" "")
refused(frame_past_end "${sponza}|--frames|3|--dt|50|--frame|3|-o|f.png" 2 "the path has frames 0 to 2 at --dt 50, not 3")
if(LOUPE_PACK_TOOL)
    # The scene's own checkout: a folder, and its HEAD checked out as a worktree.
    get_filename_component(autana_dir "${AUTANA_DIR}" ABSOLUTE)
    path_frame(frame_compare "${sponza_toml}" "" "${autana_dir}" "--view meshlets --object atrium")
    # A's --set must not reach B: B's frame is a fresh run's under its own flags.
    path_frame(frame_compare_set "${sponza_toml}" "--set|render.cull=0" "${autana_dir}" "--view shaded")
    # HEAD's worktree clones its submodules, from the network.
    path_frame(frame_compare_head "${sponza_toml}" "" HEAD "")
    # --frames 3 at --dt 100 (tests/path_video.cmake).
    function(path_video name b_args expect)
        add_test(NAME ${name} COMMAND "${CMAKE_COMMAND}" "-DLOUPE=$<TARGET_FILE:loupe>" "-DPYTHON=${Python3_EXECUTABLE}"
                 "-DSCENE=${sponza_toml}" "-DCOMPARE=${autana_dir}" "-DB_ARGS=${b_args}" -DFRAMES=3 -DDT=100
                 "-DEXPECT=${expect}" "-DOUT=${CMAKE_CURRENT_BINARY_DIR}/${name}"
                 -P "${CMAKE_CURRENT_SOURCE_DIR}/path_video.cmake")
        on_packs(${name})
        set_tests_properties(${name} PROPERTIES SKIP_REGULAR_EXPRESSION "SKIP:")
    endfunction()
    path_video(compare_same "" same)
    path_video(compare_view "--view meshlets" differ)
    refused(compare_bad_rev "${sponza_toml}|--compare|no-such-rev|--frame|1|-o|f.png" 2
            "not a revision or directory: no-such-rev")
    # A folder that is no checkout of the scene: this test's own.
    refused(compare_no_scene "${sponza_toml}|--compare|.|--frame|1|-o|f.png" 2 ". has no launcher/demo/sponza/sponza.scene.toml")
else()
    refused(compare_refused "${sponza_toml}|--compare|HEAD|--video|v.mp4" 2 "built without the pack tool --compare needs")
endif()

add_executable(seek_test seek_test.cpp)
target_link_libraries(seek_test PRIVATE loupe_core)
add_test(NAME seek COMMAND seek_test "${sponza}" "${CMAKE_CURRENT_BINARY_DIR}/seek.bmp")
on_packs(seek)

add_executable(whole_path_test whole_path_test.cpp)
target_link_libraries(whole_path_test PRIVATE loupe_core)
add_test(NAME whole_path COMMAND whole_path_test "${sponza}" "${CMAKE_CURRENT_BINARY_DIR}/whole_path.bmp")
on_packs(whole_path)

add_executable(thinning_test thinning_test.cpp)
target_link_libraries(thinning_test PRIVATE loupe_core)
# Budgets of 1.5, 3.5 and 4.5 checkpoints: by the test's last frame they thin three, two and one times.
foreach(budget IN ITEMS "3;3" "7;2" "9;1")
    list(GET budget 0 halves)
    list(GET budget 1 thinnings)
    add_test(NAME thinning_${halves} COMMAND thinning_test "${sponza}"
             "${CMAKE_CURRENT_BINARY_DIR}/thinning_${halves}.bmp" ${halves} ${thinnings})
    on_packs(thinning_${halves})
endforeach()

add_executable(view_test view_test.cpp)
target_link_libraries(view_test PRIVATE loupe_core)
add_test(NAME view_switch COMMAND "${CMAKE_COMMAND}" "-DVIEW_TEST=$<TARGET_FILE:view_test>" "-DSCENE=${sponza}"
         "-DOUT=${CMAKE_CURRENT_BINARY_DIR}/view_switch" -P "${CMAKE_CURRENT_SOURCE_DIR}/view_test.cmake")
on_packs(view_switch)

# A scene opened again after another closed, in one process, draws what it
# drew opened first, both ways round; and a pack of the same name from another
# folder, here the next-version pack, is read afresh and refused, leaving the
# next opening unharmed.
add_executable(reopen_test reopen_test.cpp)
target_link_libraries(reopen_test PRIVATE loupe_core)
foreach(order IN ITEMS "sponza;water" "water;sponza")
    list(GET order 0 first)
    list(GET order 1 second)
    add_test(NAME reopen_${first} COMMAND reopen_test "${CMAKE_CURRENT_BINARY_DIR}/reopen_${first}" "${${first}}"
                                          "${${second}}" "${${first}}")
    on_packs(reopen_${first})
endforeach()
add_test(NAME reopen_same_name COMMAND reopen_test "${CMAKE_CURRENT_BINARY_DIR}/reopen_same_name" "${sponza}"
                                       --refused "${next_pack}" "${sponza}")
set_tests_properties(reopen_same_name PROPERTIES FIXTURES_REQUIRED "packs;next_version" RESOURCE_LOCK sponza)

# --frame, --compare and --b-args parse only with what they need.
add_executable(options_test options_test.cpp)
target_link_libraries(options_test PRIVATE loupe_core)
add_test(NAME path_flags COMMAND options_test)
# A tool loupe runs gets its arguments and paths intact, its output in the log.
add_executable(process_test process_test.cpp)
target_link_libraries(process_test PRIVATE loupe_core)
add_test(NAME run_logged COMMAND process_test "${CMAKE_CURRENT_BINARY_DIR}/run_logged")

add_executable(library_test library_test.cpp)
target_link_libraries(library_test PRIVATE loupe_core)
add_test(NAME library COMMAND library_test "${CMAKE_CURRENT_BINARY_DIR}/library")

# The window drawn offscreen for a few ticks, on SCENE or on the start panel
# without one: on Linux CI, Mesa's lavapipe.
set(window_ticks 10)
function(window_smoke name scene)
    add_test(NAME ${name} COMMAND "${CMAKE_COMMAND}" "-DLOUPE=$<TARGET_FILE:loupe>" "-DSCENE=${scene}"
             "-DTICKS=${window_ticks}" "-DOUT=${CMAKE_CURRENT_BINARY_DIR}/${name}"
             -P "${CMAKE_CURRENT_SOURCE_DIR}/window_smoke.cmake")
    on_packs(${name})
    set_tests_properties(${name} PROPERTIES SKIP_REGULAR_EXPRESSION "SKIP:")
endfunction()
window_smoke(window_smoke "${sponza}")
window_smoke(window_start "")

# --version names the project version and the autana commit compiled.
add_test(NAME version COMMAND loupe --version)
set_tests_properties(version PROPERTIES PASS_REGULAR_EXPRESSION "^loupe ${PROJECT_VERSION}, autana [0-9a-f]+\n$")
