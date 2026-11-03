# Read by cpack once per generator: the Windows downloads' names.
if(CPACK_GENERATOR STREQUAL "ZIP" AND CPACK_SYSTEM_NAME STREQUAL "windows")
    set(CPACK_PACKAGE_FILE_NAME "loupe-${CPACK_LOUPE_FILE_VERSION}-windows-portable")
elseif(CPACK_GENERATOR STREQUAL "NSIS")
    set(CPACK_PACKAGE_FILE_NAME "loupe-${CPACK_LOUPE_FILE_VERSION}-windows-setup")
endif()
