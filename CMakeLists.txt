cmake_minimum_required(VERSION 2.8.5)
include(CMake/ves-superbuild.cmake)
if(ves_superbuild_enabled)
  return()
endif()


project(VES)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules")

# set project variables
set(VES_TARGETS_NAME ves-targets)
set(VES_INSTALL_BIN_DIR bin)
set(VES_INSTALL_LIB_DIR lib)
set(VES_INSTALL_INCLUDE_DIR include/ves)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")

# declare options
option(BUILD_SHARED_LIBS "Build VES with shared libraries." OFF)
option(BUILD_TESTING "Build VES with tests enabled." OFF)
option(VES_USE_VTK "Build the kiwi library.  Requires VTK." OFF)
option(VES_USE_DESKTOP_GL "Build VES using desktop OpenGL instead of OpenGL ES." OFF)

# include cmake scripts
include(CMake/ves-macros.cmake)
include(CMake/ves-testing.cmake)
include(CMake/ves-version.cmake)

# build project
add_subdirectory(src)

# documentation
add_subdirectory(Docs)

# project install rules
include(CMake/ves-post-build.cmake)
