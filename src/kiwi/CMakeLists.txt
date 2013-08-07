find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

set(sources
  cJSON.c
  vesKiwiAnimationRepresentation.cpp
  vesKiwiBaseApp.cpp
  vesKiwiBaselineImageTester.cpp
  vesKiwiBrainAtlasRepresentation.cpp
  vesKiwiCameraSpinner.cpp
  vesKiwiCameraInteractor.cpp
  vesKiwiColorMapCollection.cpp
  vesKiwiDataConversionTools.cpp
  vesKiwiDataLoader.cpp
  vesKiwiDataRepresentation.cpp
  vesKiwiImagePlaneDataRepresentation.cpp
  vesKiwiImageWidgetRepresentation.cpp
  vesKiwiPlaneWidget.cpp
  vesKiwiPolyDataRepresentation.cpp
  vesKiwiSceneRepresentation.cpp
  vesKiwiStreamingDataRepresentation.cpp
  vesKiwiText2DRepresentation.cpp
  vesKiwiViewerApp.cpp
  vesKiwiWidgetInteractionDelegate.cpp
  vesKiwiWidgetRepresentation.cpp
  )

set(deps
  ves
  vesShaders
  vtkIOGeometry
  vtkIOXML
  vtkIOImage
  vtkIOLegacy
  vtkIOPLY
  vtkFiltersCore
  vtkFiltersSources
  vtkFiltersGeometry
  vtkFiltersModeling
  vtkImagingCore
  vtkRenderingCore
  vtkRenderingFreeType
  )

option(VES_USE_CURL "Build VES with cURL support?" ON)
if(VES_USE_CURL)
  find_package(CURL REQUIRED)
  include_directories(${CURL_INCLUDE_DIRS})
  list(APPEND deps ${CURL_LIBRARIES})
  list(APPEND sources
    vesKiwiCurlDownloader.cpp
    vesKiwiPVRemoteRepresentation.cpp
    vesPVWebClient.cpp
    vesPVWebDataSet.cpp
    vesMidasClient.cpp
    )
endif()

option(VES_USE_LIBARCHIVE "Build VES with libarchive support?" ON)
if(VES_USE_LIBARCHIVE)
  find_package(LibArchive REQUIRED)
  include_directories(${LibArchive_INCLUDE_DIRS})
  list(APPEND deps ${LibArchive_LIBRARIES})
  list(APPEND sources
    vesKiwiArchiveUtils.cpp
    )
endif()

option(VES_USE_PCL "Build kiwi with PCL support" OFF)
if(VES_USE_PCL)
  add_subdirectory(pcl)
  list(APPEND deps vtkPCL)
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/pcl")
endif()

file(GLOB headers *.h)

# Configure version file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vesKiwiVersion.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/vesKiwiVersion.h @ONLY)

# Configure options file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vesKiwiOptions.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/vesKiwiOptions.h)

list(APPEND headers
  ${CMAKE_CURRENT_BINARY_DIR}/vesKiwiVersion.h
  ${CMAKE_CURRENT_BINARY_DIR}/vesKiwiOptions.h
  )

# ... now add the location of the configured files to the build
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ves_add_library(kiwi "${sources};${headers}" "${deps}")

# Add version info to the target. Currently using a single global version string.
set_target_properties(kiwi PROPERTIES SOVERSION ${VES_VERSION_STR}
                      VERSION ${VES_VERSION_STR})

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
