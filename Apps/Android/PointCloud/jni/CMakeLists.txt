#notice the "recycling" of CMAKE_C_FLAGS
#this is necessary to pick up android flags
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -fPIC" )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

#set( LIBRARY_DEPS GLESv2 log android EGL)
set( LIBRARY_DEPS GLESv2 GLESv1_CM log android EGL native-activity )

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
set( VTK_DEPS vtkIOGeometry vtkIOCore vtkCommonDataModel vtkCommonTransforms vtkCommonSystem vtkCommonMath vtkCommonMisc vtkCommonCore vtksys )
link_directories(${VTK_LIBRARY_DIRS})

set( VES_DEPS ves )
set( VES_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../CMakeBuild/build/CMakeExternals/Build/ves-android )
message("VES_LIBRARY_DIR ${VES_LIBRARY_DIR}")
link_directories(${VES_LIBRARY_DIR})
message("VES_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../src/ves")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../../src/ves ${CMAKE_CURRENT_SOURCE_DIR}/../../../../src
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../../src/ves/vsg/Core
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../../src/ves/vsg/Grouping
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../../src/ves/vsg/Navigation
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../../src/ves/vsg/Rendering
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../../src/ves/vsg/Shader
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../../src/ves/vsg/Utility
)

set(MY_SRCS
    android_native_app_glue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../src/ves/vesPolyDataToTriangleData.cpp
    main.cpp
    )

add_library(native-activity SHARED ${MY_SRCS} )

#link the module like any other
target_link_libraries(native-activity ${LIBRARY_DEPS} ${VTK_DEPS} ${VES_DEPS})
