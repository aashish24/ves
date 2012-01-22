
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

# temporary workaround for vtkmodular missing include dirs
include_directories("${VTK_SOURCE_DIR}/Filters/Core"
                    "${VTK_SOURCE_DIR}/Filters/Sources")

set(sources
  vesKiwiAnimationRepresentation.cpp
  vesKiwiBaseApp.cpp
  vesKiwiBrainAtlasRepresentation.cpp
  vesKiwiDataConversionTools.cpp
  vesKiwiDataLoader.cpp
  vesKiwiDataRepresentation.cpp
  vesKiwiImagePlaneDataRepresentation.cpp
  vesKiwiImageWidgetRepresentation.cpp
  vesKiwiPlaneWidget.cpp
  vesKiwiPolyDataRepresentation.cpp
  vesKiwiText2DRepresentation.cpp
  vesKiwiViewerApp.cpp
  vesKiwiWidgetRepresentation.cpp
  )

include_directories(Text)
include_directories("${VTK_SOURCE_DIR}/Utilities"
                    "${VTK_SOURCE_DIR}/Utilities/vtkfreetype/include")
set(sources ${sources}
    Text/face_arial_bold.cxx
    Text/face_arial_bold_italic.cxx
    Text/face_arial.cxx
    Text/face_arial_italic.cxx
    Text/face_courier_bold.cxx
    Text/face_courier_bold_italic.cxx
    Text/face_courier.cxx
    Text/face_courier_italic.cxx
    Text/face_times_bold.cxx
    Text/face_times_bold_italic.cxx
    Text/face_times.cxx
    Text/face_times_italic.cxx
    Text/vtkStringToImage.cxx
    Text/vtkFreeTypeStringToImage.cxx
    Text/vtkFreeTypeTools.cxx
    Text/vtkTextProperty.cxx
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
  vtkImagingCore)

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
