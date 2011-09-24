
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

# temporary workaround for vtkmodular missing include dirs
include_directories("${VTK_SOURCE_DIR}/Filters/Core"
                    "${VTK_SOURCE_DIR}/Filters/Sources")

set(sources
  vesKiwiDataLoader.cpp
  vesKiwiDataRepresentation.cpp
  vesKiwiViewerApp.cpp
  vtkPolyDataToTriangleData.cpp)


set(deps
  ves
  vtkIOGeometry
  vtkIOXML
  vtkIOImage
  vtkFiltersCore
  vtkFiltersSources
  vtkImagingCore)

ves_add_library(vtkVES "${sources}" "${deps}")

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
