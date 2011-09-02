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
  vtkIOGeometry
  vtkIOXML
  ${VES_LIBRARIES}
)

set(kiwi_lib kiwi-native)

add_library(${kiwi_lib} SHARED ${sources})
target_link_libraries(${kiwi_lib} ${deps})
