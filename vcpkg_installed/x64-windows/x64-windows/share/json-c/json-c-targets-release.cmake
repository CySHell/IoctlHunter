#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "json-c::json-c" for configuration "Release"
set_property(TARGET json-c::json-c APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(json-c::json-c PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/json-c.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/json-c.dll"
  )

list(APPEND _cmake_import_check_targets json-c::json-c )
list(APPEND _cmake_import_check_files_for_json-c::json-c "${_IMPORT_PREFIX}/lib/json-c.lib" "${_IMPORT_PREFIX}/bin/json-c.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
