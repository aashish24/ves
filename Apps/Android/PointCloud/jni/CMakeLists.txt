#notice the "recycling" of CMAKE_C_FLAGS
#this is necessary to pick up android flags
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -fPIC" )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
set( VTK_DEPS vtkIOGeometry vtkCommonDataModel)

find_package(VES REQUIRED)
include_directories(${VES_INCLUDE_DIRS})

set(MY_SRCS
    android_native_app_glue.c
    main.cpp
)

set(LIBRARY_DEPS
  GLESv2
  GLESv1_CM
  log
  android
  EGL
  ${VTK_DEPS}
  ${VES_LIBRARIES}
)

add_library(native-activity SHARED ${MY_SRCS})
target_link_libraries(native-activity ${LIBRARY_DEPS})
