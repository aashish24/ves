
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(sources
  vtkPolyDataToTriangleData.cpp)


set(deps
  vtkCommonDataModel)

ves_add_library(vtkVES "${sources}" "${deps}")

