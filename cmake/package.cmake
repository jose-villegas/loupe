# What `cpack` puts in a download. Windows: a portable zip, loupe.exe beside
# scenes/ and LICENSE. Linux: a tarball in the prefix layout, bin/loupe and
# share/ with its samples, .desktop file and icon, to copy into ~/.local or
# /usr/local or run where it lands. LOUPE_SAMPLES adds every autana demo scene's pack.

if(WIN32)
    set(bin_dir .)
    set(scenes_dir scenes)
    set(doc_dir .)
    set(CPACK_GENERATOR ZIP)
    set(CPACK_SYSTEM_NAME windows-portable)
else()
    include(GNUInstallDirs)
    set(bin_dir "${CMAKE_INSTALL_BINDIR}")
    set(scenes_dir "${CMAKE_INSTALL_DATADIR}/loupe/scenes")
    set(doc_dir "${CMAKE_INSTALL_DOCDIR}")
    install(FILES assets/loupe.desktop DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
    install(FILES assets/loupe.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps")
    set(CPACK_GENERATOR TGZ)
    set(CPACK_SYSTEM_NAME "linux-${CMAKE_SYSTEM_PROCESSOR}")
endif()
install(TARGETS loupe RUNTIME DESTINATION "${bin_dir}")
install(FILES LICENSE DESTINATION "${doc_dir}")
# The start panel lists the samples where this installs them.
file(RELATIVE_PATH samples_from_exe "/prefix/${bin_dir}" "/prefix/${scenes_dir}")
set_property(SOURCE src/library.cpp APPEND PROPERTY COMPILE_DEFINITIONS "LOUPE_SAMPLES_DIR=\"${samples_from_exe}\"")

if(LOUPE_SAMPLES)
    # By glob, so a scene autana adds ships without a change here.
    file(GLOB scenes CONFIGURE_DEPENDS "${AUTANA_DIR}/launcher/demo/*/*.scene.toml")
    if(NOT scenes)
        message(FATAL_ERROR "LOUPE_SAMPLES: no .scene.toml under ${AUTANA_DIR}/launcher/demo")
    endif()
    # On every build: only build_pack.py knows a pack's inputs. It rewrites
    # only the packs that changed and removes those of scenes gone.
    set(samples "${CMAKE_BINARY_DIR}/samples")
    add_custom_target(samples ALL
        COMMAND "${Python3_EXECUTABLE}" "${AUTANA_DIR}/launcher/tools/r3d/build_pack.py" -o "${samples}" ${scenes}
        VERBATIM)
    install(DIRECTORY "${samples}/" DESTINATION "${scenes_dir}" FILES_MATCHING PATTERN "*.apak")
endif()

set(CPACK_PACKAGE_VENDOR "jose-villegas")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A frame debugger for the autana engine")
set(CPACK_STRIP_FILES ON)
# CPACK_PACKAGE_FILE_NAME's default: loupe-<version>-<system name>.
include(CPack)
