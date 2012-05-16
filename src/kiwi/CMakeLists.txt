
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(sources
  vesKiwiAnimationRepresentation.cpp
  vesKiwiBaseApp.cpp
  vesKiwiBaselineImageTester.cpp
  vesKiwiBrainAtlasRepresentation.cpp
  vesKiwiCameraSpinner.cpp
  vesKiwiCameraInteractor.cpp
  vesKiwiDataConversionTools.cpp
  vesKiwiDataLoader.cpp
  vesKiwiDataRepresentation.cpp
  vesKiwiImagePlaneDataRepresentation.cpp
  vesKiwiImageWidgetRepresentation.cpp
  vesKiwiPlaneWidget.cpp
  vesKiwiPolyDataRepresentation.cpp
  vesKiwiStreamingDataRepresentation.cpp
  vesKiwiText2DRepresentation.cpp
  vesKiwiViewerApp.cpp
  vesKiwiWidgetRepresentation.cpp
  )

set(deps
  ves
  vesShaders
  vtkIOGeometry
  vtkIOXML
  vtkIOImage
  vtkIOPLY
  vtkFiltersCore
  vtkFiltersSources
  vtkFiltersGeometry
  vtkFiltersModeling
  vtkImagingCore
  vtkRenderingCore
  vtkRenderingFreeType
  )

# Configure version file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vesKiwiVersion.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/vesKiwiVersion.h @ONLY)

ves_add_library(kiwi "${sources}" "${deps}")

# Add version info to the target. Currently using a single global version string.
set_target_properties(kiwi PROPERTIES SOVERSION ${VES_VERSION_STR}
                      VERSION ${VES_VERSION_STR})

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
