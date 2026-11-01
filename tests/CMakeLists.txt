set(sponza "${AUTANA_DIR}/launcher/demo/sponza/sponza.scene.toml")
find_program(SH NAMES sh bash REQUIRED)

# The include path loupe_core compiles with, in order, for the gate to resolve includes against.
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/include_dirs.txt"
     CONTENT "$<JOIN:$<TARGET_PROPERTY:loupe_core,INCLUDE_DIRECTORIES>,\n>\n")
add_test(NAME include_gate COMMAND "${CMAKE_COMMAND}" "-DSOURCE_DIR=${PROJECT_SOURCE_DIR}" "-DAUTANA_DIR=${AUTANA_DIR}"
         "-DINCLUDE_DIRS=${CMAKE_BINARY_DIR}/include_dirs.txt" -P "${CMAKE_CURRENT_SOURCE_DIR}/include_gate.cmake")

# loupe and autana's scene_viewer.sh on the same arguments; ARGS are |-separated.
function(parity name args)
    add_test(NAME parity_${name} COMMAND "${CMAKE_COMMAND}" "-DLOUPE=$<TARGET_FILE:loupe>" "-DSH=${SH}"
             "-DAUTANA_DIR=${AUTANA_DIR}" "-DSCENE=${sponza}" "-DARGS=${args}"
             "-DOUT=${CMAKE_CURRENT_BINARY_DIR}/parity_${name}" -P "${CMAKE_CURRENT_SOURCE_DIR}/parity.cmake")
    set_tests_properties(parity_${name} PROPERTIES RESOURCE_LOCK sponza)
endfunction()
parity(defaults "--frames|3|--dt|100")
parity(every_flag "--object|atrium|--camera|tour|--view|meshlets|--set|render.batched=0|--frames|4|--dt|50|--video")

add_executable(seek_test seek_test.cpp)
target_link_libraries(seek_test PRIVATE loupe_core)
add_test(NAME seek COMMAND seek_test "${sponza}" "${CMAKE_CURRENT_BINARY_DIR}/seek.bmp")
set_tests_properties(seek PROPERTIES RESOURCE_LOCK sponza)
