#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "iloj::iloj" for configuration "Release"
set_property(TARGET iloj::iloj APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(iloj::iloj PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libiloj.a"
  )

list(APPEND _cmake_import_check_targets iloj::iloj )
list(APPEND _cmake_import_check_files_for_iloj::iloj "${_IMPORT_PREFIX}/lib/libiloj.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
