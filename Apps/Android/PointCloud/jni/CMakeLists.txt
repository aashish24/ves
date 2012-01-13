set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -fPIC" )


find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

find_package(VES REQUIRED)
include_directories(${VES_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(sources
  android_native_app_glue.c
  main.cpp
)

set(deps
  GLESv2
  EGL
  android
  log
  ${VES_LIBRARIES}
)

message(${VES_LIBRARIES})

add_library(PointCloud SHARED ${sources})
target_link_libraries(PointCloud ${deps})
