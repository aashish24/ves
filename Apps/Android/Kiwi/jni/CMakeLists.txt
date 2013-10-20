set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -fPIC" )


find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(VES REQUIRED)
include_directories(${VES_INCLUDE_DIRS})

# TODO: Verify if this is not required on Windows
if (NOT WIN32)
  find_package(ZLIB REQUIRED)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

option(USE_PCL "Build KiwiViewer with PCL support" OFF)
if(USE_PCL)
  include(pcl.cmake)
endif()


set(sources
  KiwiNative.cpp
)

set(deps
  GLESv2
  log
  ${VES_LIBRARIES}
  ${ZLIB_LIBRARIES}
)

set(kiwi_lib KiwiNative)

add_library(${kiwi_lib} SHARED ${sources})
target_link_libraries(${kiwi_lib} ${deps})
