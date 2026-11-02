# Read by cpack once per generator: the Windows installer is the setup download.
if(CPACK_GENERATOR STREQUAL "NSIS")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-windows-setup")
endif()
