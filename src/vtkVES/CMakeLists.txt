
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(sources
  vesKiwiViewerApp.cpp
  vtkPolyDataToTriangleData.cpp)


set(deps
  vtkCommonDataModel)

ves_add_library(vtkVES "${sources}" "${deps}")

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
