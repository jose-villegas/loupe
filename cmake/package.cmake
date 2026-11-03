# What `cpack` puts in a download. Windows: a portable zip, loupe.exe beside
# scenes/ and LICENSE, and the same files in an installer (NSIS) that adds
# shortcuts and opens .apak files in Loupe. Linux: a tarball in the prefix layout, bin/loupe and
# share/ with its samples, .desktop file and icon, to copy into ~/.local or
# /usr/local or run where it lands. LOUPE_SAMPLES adds every autana demo scene's pack.
# A Main build's files are named main in place of the version, so the rolling
# release's links stay the same from one push to the next.

set(CPACK_PACKAGE_VENDOR "jose-villegas")
if(WIN32)
    set(bin_dir .)
    set(scenes_dir scenes)
    set(doc_dir .)
    # loupe.exe's icon, for Explorer, its shortcuts and its .apak files.
    target_sources(loupe PRIVATE assets/loupe.rc)
    set(CPACK_GENERATOR ZIP NSIS)
    set(CPACK_SYSTEM_NAME windows)
    set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_CURRENT_LIST_DIR}/cpack_generator.cmake")
    # One key and one folder for every version, so a new one replaces the old.
    set(CPACK_PACKAGE_INSTALL_DIRECTORY Loupe)
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY Loupe)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    # Beside loupe.exe in an installed copy only: how a build knows it was installed.
    set(CPACK_NSIS_UNINSTALL_NAME Uninstall)
    set_property(SOURCE src/version.cpp APPEND PROPERTY COMPILE_DEFINITIONS
                 "LOUPE_UNINSTALLER=\"${CPACK_NSIS_UNINSTALL_NAME}.exe\"")
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_NSIS_PACKAGE_NAME Loupe)
    set(CPACK_NSIS_DISPLAY_NAME Loupe)
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/jose-villegas/loupe")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/loupe.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/loupe.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME loupe.exe)
    set(CPACK_NSIS_MANIFEST_DPI_AWARE ON)
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    # The Start menu entry; loupe.exe sits at the install root, not in bin/.
    set(CPACK_PACKAGE_EXECUTABLES loupe Loupe)
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY .)
    # The shortcuts and the .apak association; NSIS wants backslashes.
    foreach(step install uninstall)
        set(${step}_nsh "${CMAKE_CURRENT_LIST_DIR}/nsis/${step}.nsh")
        cmake_path(NATIVE_PATH ${step}_nsh ${step}_nsh)
    endforeach()
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "!include \"${install_nsh}\"")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "!include \"${uninstall_nsh}\"")
    # CPack removes Software\<vendor>\Loupe after the commands above, and
    # never the vendor's key; this hook runs after it.
    set(CPACK_NSIS_DELETE_ICONS_EXTRA "DeleteRegKey /ifempty SHCTX \"Software\\${CPACK_PACKAGE_VENDOR}\"")
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

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A frame debugger for the autana engine")
set(CPACK_STRIP_FILES ON)
# Writes the values above into CPackConfig.cmake as they are: the
# backslashes and quotes of the NSIS lines survive.
set(CPACK_VERBATIM_VARIABLES ON)
# loupe-<version>-<system name>; cpack_generator.cmake names the Windows ones.
if(LOUPE_CHANNEL STREQUAL "Main")
    set(CPACK_LOUPE_FILE_VERSION main)
else()
    set(CPACK_LOUPE_FILE_VERSION "${PROJECT_VERSION}")
endif()
set(CPACK_PACKAGE_FILE_NAME "loupe-${CPACK_LOUPE_FILE_VERSION}-${CPACK_SYSTEM_NAME}")
include(CPack)
