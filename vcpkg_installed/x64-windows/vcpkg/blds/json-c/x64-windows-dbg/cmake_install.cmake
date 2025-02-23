# Install script for directory: C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "OFF")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS)
  set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS "OWNER_READ;OWNER_WRITE;OWNER_EXECUTE;GROUP_READ;GROUP_EXECUTE;WORLD_READ;WORLD_EXECUTE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/x64-windows-dbg/json-c.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/x64-windows-dbg/json-c.dll")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/json-c/json-c-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/json-c/json-c-targets.cmake"
         "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/x64-windows-dbg/CMakeFiles/Export/c72427da9e5c73ebf6c111c2977a0759/json-c-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/json-c/json-c-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/json-c/json-c-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/json-c" TYPE FILE FILES "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/x64-windows-dbg/CMakeFiles/Export/c72427da9e5c73ebf6c111c2977a0759/json-c-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/json-c" TYPE FILE FILES "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/x64-windows-dbg/CMakeFiles/Export/c72427da9e5c73ebf6c111c2977a0759/json-c-targets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/json-c" TYPE FILE FILES "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/x64-windows-dbg/json-c-config.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/x64-windows-dbg/json-c.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/json_config.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/json.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/arraylist.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/debug.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/json_c_version.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/json_inttypes.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/json_object.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/json_object_iterator.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/json_tokener.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/json_types.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/json_util.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/json_visit.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/linkhash.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/printbuf.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/json_pointer.h;C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c/json_patch.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/pkgs/json-c_x64-windows/debug/include/json-c" TYPE FILE FILES
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/x64-windows-dbg/json_config.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/x64-windows-dbg/json.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean/arraylist.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean/debug.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean/json_c_version.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean/json_inttypes.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean/json_object.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean/json_object_iterator.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean/json_tokener.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean/json_types.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean/json_util.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean/json_visit.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean/linkhash.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean/printbuf.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean/json_pointer.h"
    "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/src/a22988fb34-2c10eba8ac.clean/json_patch.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/x64-windows-dbg/doc/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/AmirShen/source/repos/WindbgExtensionTest/vcpkg_installed/x64-windows/vcpkg/blds/json-c/x64-windows-dbg/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
