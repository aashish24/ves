project(VES)
cmake_minimum_required(VERSION 2.8)

# set project variables
set(VES_MAJOR_VERSION 0)
set(VES_MINOR_VERSION 1)
set(VES_PATCH_VERSION 0)
set(VES_VERSION ${VES_MAJOR_VERSION}.${VES_MINOR_VERSION}.${VES_PATCH_VERSION})

set(VES_TARGETS_NAME ves-targets)
set(VES_INSTALL_BIN_DIR bin)
set(VES_INSTALL_LIB_DIR lib)
set(VES_INSTALL_INCLUDE_DIR include/ves)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")

# declare options
option(BUILD_SHARED_LIBS "Build VES with shared libraries." OFF)
OPTION(BUILD_TESTING "Build VES with tests enabled." OFF)
option(VES_USE_VTK "Build the vtkVES library.  Requires VTK." OFF)

# include cmake scripts
include(CMake/ves-macros.cmake)
include(CMake/ves-testing.cmake)

# build project
add_subdirectory(src)

# documentation
add_subdirectory(Docs)

# project install rules
include(CMake/ves-post-build.cmake)
