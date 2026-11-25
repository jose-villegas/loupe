set(demo "${AUTANA_DIR}/launcher/demo")
set(sponza_toml "${demo}/sponza/sponza.scene.toml")
find_program(SH NAMES sh REQUIRED)
# The tests build their packs with autana's build_pack.py, even where loupe itself has no pack tool.
find_package(Python3 ${python_min} REQUIRED COMPONENTS Interpreter)

# The packs the tests open, built once: every test but parity_scene_toml opens
# a .apak, so the suite runs in builds without the pack tool too.
set(packs "${CMAKE_CURRENT_BINARY_DIR}/packs")
set(sponza "${packs}/sponza.apak")
# Sponza's flight: the scene's first camera stands still, so tests that fly
# its path name this one, as autana's app and viewer test do.
set(sponza_camera camera)
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
       "--object|atrium_fitted_full|--camera|tour|--view|meshlets|--set|render.batched=0|--frames|4|--dt|50|--video")

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
path_frame(frame "${sponza}" "--camera|${sponza_camera}|--view|meshlets" "" "")
refused(frame_past_end "${sponza}|--frames|3|--dt|50|--frame|3|-o|f.png" 2 "the path has frames 0 to 2 at --dt 50, not 3")
if(LOUPE_PACK_TOOL)
    # The scene's own checkout: a folder, and its HEAD checked out as a worktree.
    get_filename_component(autana_dir "${AUTANA_DIR}" ABSOLUTE)
    path_frame(frame_compare "${sponza_toml}" "--camera|${sponza_camera}" "${autana_dir}"
               "--camera ${sponza_camera} --view meshlets --object atrium_fitted_full")
    # A's --set must not reach B: B's frame is a fresh run's under its own flags.
    path_frame(frame_compare_set "${sponza_toml}" "--camera|${sponza_camera}|--set|render.cull=0" "${autana_dir}"
               "--camera ${sponza_camera} --view shaded")
    # HEAD's worktree clones its submodules, from the network.
    path_frame(frame_compare_head "${sponza_toml}" "--camera|${sponza_camera}" HEAD "")
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
    refused(compare_bad_rev "${sponza_toml}|--camera|${sponza_camera}|--compare|no-such-rev|--frame|1|-o|f.png" 2
            "not a revision or directory: no-such-rev")
    # A folder that is no checkout of the scene: this test's own.
    refused(compare_no_scene "${sponza_toml}|--camera|${sponza_camera}|--compare|.|--frame|1|-o|f.png" 2 ". has no launcher/demo/sponza/sponza.scene.toml")
else()
    refused(compare_refused "${sponza_toml}|--compare|HEAD|--video|v.mp4" 2 "built without the pack tool --compare needs")
endif()

# A frame record stream: a host run's --record opened as a .framerec and fed live.
add_executable(stream_test stream_test.cpp)
target_link_libraries(stream_test PRIVATE loupe_core)
add_test(NAME stream COMMAND stream_test "${sponza}" "${CMAKE_CURRENT_BINARY_DIR}/stream")
on_packs(stream)

# A board's recording against a fake autana record, and the board list's parse.
add_executable(fake_record fake_record.cpp)
add_executable(recording_test recording_test.cpp)
target_link_libraries(recording_test PRIVATE loupe_core loupe_window)
add_test(NAME recording COMMAND recording_test "$<TARGET_FILE:fake_record>" "${CMAKE_CURRENT_BINARY_DIR}/recording")

add_executable(seek_test seek_test.cpp)
target_link_libraries(seek_test PRIVATE loupe_core)
add_test(NAME seek COMMAND seek_test "${sponza}" "${CMAKE_CURRENT_BINARY_DIR}/seek.bmp" ${sponza_camera})
on_packs(seek)

# The observer: Frames::observe on the cull view, and its start speed and ImGui steering.
add_executable(observe_test observe_test.cpp)
target_link_libraries(observe_test PRIVATE loupe_core)
add_test(NAME observe COMMAND observe_test "${sponza}" "${CMAKE_CURRENT_BINARY_DIR}/observe.bmp" ${sponza_camera})
on_packs(observe)
add_executable(observer_test observer_test.cpp)
target_link_libraries(observer_test PRIVATE loupe_core loupe_window)
add_test(NAME observer_steer COMMAND observer_test)

# The panels drawn headless at several widths fit them, and are kept as BMPs to look at.
add_executable(panel_fit_test panel_fit_test.cpp)
target_link_libraries(panel_fit_test PRIVATE loupe_core loupe_window)
add_test(NAME panel_fit COMMAND panel_fit_test "${sponza}" "${CMAKE_CURRENT_BINARY_DIR}/panel_fit")
on_packs(panel_fit)

add_executable(whole_path_test whole_path_test.cpp)
target_link_libraries(whole_path_test PRIVATE loupe_core)
add_test(NAME whole_path COMMAND whole_path_test "${sponza}" "${CMAKE_CURRENT_BINARY_DIR}/whole_path.bmp"
                                   ${sponza_camera})
on_packs(whole_path)

add_executable(thinning_test thinning_test.cpp)
target_link_libraries(thinning_test PRIVATE loupe_core)
# Budgets of 1.5, 3.5 and 4.5 checkpoints: by the test's last frame they thin three, two and one times.
foreach(budget IN ITEMS "3;3" "7;2" "9;1")
    list(GET budget 0 halves)
    list(GET budget 1 thinnings)
    add_test(NAME thinning_${halves} COMMAND thinning_test "${sponza}"
             "${CMAKE_CURRENT_BINARY_DIR}/thinning_${halves}.bmp" ${halves} ${thinnings} ${sponza_camera})
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

# The editor: its documents, undo and matrices; Frames::place on the sponza
# pack; and a save through autana's scene_edit.py on a copy of the capybara
# scene, so the test never writes autana's own files.
add_executable(editor_test editor_test.cpp)
target_link_libraries(editor_test PRIVATE loupe_core loupe_window)
add_test(NAME editor_docs COMMAND editor_test)
# Camera paths: the curve against autana's camera_keys.py, the key edits, and
# scene_edit.py's path arguments and parses.
add_executable(camera_path_test camera_path_test.cpp)
target_link_libraries(camera_path_test PRIVATE loupe_core)
add_test(NAME camera_paths COMMAND camera_path_test)
add_executable(path_editor_test path_editor_test.cpp)
target_link_libraries(path_editor_test PRIVATE loupe_core loupe_window)
add_test(NAME path_editor COMMAND path_editor_test)
add_executable(place_test place_test.cpp)
target_link_libraries(place_test PRIVATE loupe_core)
add_test(NAME place COMMAND place_test "${sponza}" "${CMAKE_CURRENT_BINARY_DIR}/place.bmp" ${sponza_camera})
on_packs(place)
if(LOUPE_PACK_TOOL)
    set(capybara_copy "${CMAKE_CURRENT_BINARY_DIR}/scene_save/capybara")
    add_test(NAME scene_save_copy COMMAND "${CMAKE_COMMAND}" -E copy_directory "${demo}/capybara" "${capybara_copy}")
    set_tests_properties(scene_save_copy PROPERTIES FIXTURES_SETUP scene_save_copy)
    add_executable(scene_save_test scene_save_test.cpp)
    target_link_libraries(scene_save_test PRIVATE loupe_core loupe_window)
    add_test(NAME scene_save COMMAND scene_save_test "${capybara_copy}/capybara.scene.toml")
    set_tests_properties(scene_save PROPERTIES FIXTURES_REQUIRED scene_save_copy)
    set(tour_copy "${CMAKE_CURRENT_BINARY_DIR}/path_save/tour.keys.toml")
    add_test(NAME path_save_copy COMMAND "${CMAKE_COMMAND}" -E copy "${demo}/sponza/tour.keys.toml" "${tour_copy}")
    set_tests_properties(path_save_copy PROPERTIES FIXTURES_SETUP path_save_copy)
    add_executable(path_save_test path_save_test.cpp)
    target_link_libraries(path_save_test PRIVATE loupe_core)
    add_test(NAME path_save COMMAND path_save_test "${tour_copy}")
    set_tests_properties(path_save PROPERTIES FIXTURES_REQUIRED path_save_copy)
endif()

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

# A build's id, and the uninstaller that marks an installed copy (cmake/package.cmake).
add_executable(build_id_test build_id_test.cpp)
target_link_libraries(build_id_test PRIVATE loupe_core)
set(uninstaller "")
if(WIN32)
    set(uninstaller "${CPACK_NSIS_UNINSTALL_NAME}.exe")
endif()
add_test(NAME build_id COMMAND build_id_test "${CMAKE_CURRENT_BINARY_DIR}/build_id" ${uninstaller})

# --version names the project version, Loupe's commit, the configured channel,
# portable (the build folder has no uninstaller), and the autana commit compiled.
set(channel_words_None "local build")
set(channel_words_Releases release)
set(channel_words_Main "main build")
add_test(NAME version COMMAND loupe --version)
set_tests_properties(version PROPERTIES PASS_REGULAR_EXPRESSION
    "^loupe ${PROJECT_VERSION} \\([0-9a-f]+, ${channel_words_${LOUPE_CHANNEL}}, portable\\), autana [0-9a-f]+\n$")

# Self-update: the digest, the JSON reader, the offers in saved releases, the
# setting, staging, and a running copy swapped for a staged one.
add_executable(update_test update_test.cpp)
target_link_libraries(update_test PRIVATE loupe_core)
add_test(NAME update COMMAND update_test "${CMAKE_CURRENT_SOURCE_DIR}/update" "${CMAKE_CURRENT_BINARY_DIR}/update")
# The main build's tag is the one build.yml publishes.
file(READ "${PROJECT_SOURCE_DIR}/.github/workflows/build.yml" build_yml)
string(FIND "${build_yml}" "publish_main.sh ${LOUPE_MAIN_TAG} " published)
if(published EQUAL -1)
    message(FATAL_ERROR "build.yml does not publish the main build as ${LOUPE_MAIN_TAG} (LOUPE_MAIN_TAG)")
endif()

if(LOUPE_PACK_TOOL)
    add_executable(path_test path_test.cpp)
    target_link_libraries(path_test PRIVATE loupe_core)
    foreach(camera IN ITEMS tour lion)
        add_test(NAME path_${camera} COMMAND path_test "${sponza}" "${CMAKE_CURRENT_BINARY_DIR}/path_${camera}.bmp"
                                             "${demo}/sponza/${camera}.keys.toml" ${camera})
        on_packs(path_${camera})
    endforeach()
endif()
