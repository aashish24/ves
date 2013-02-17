if ("${CMAKE_CXX_PLATFORM_ID}" STREQUAL "QNX")
  set(deps GLES_CM)
else()
  set(deps GLESv1_CM)
endif()

set(sources
  vesActor.cpp
  vesBackground.cpp
  vesBlend.cpp
  vesBlendFunction.cpp
  vesBoundingObject.cpp
  vesCamera.cpp
  vesClipPlane.cpp
  vesCullVisitor.cpp
  vesDepth.cpp
  vesEigen.cpp
  vesGeometryData.cpp
  vesGroupNode.cpp
  vesMapper.cpp
  vesMaterial.cpp
  vesNode.cpp
  vesOpenGLSupport.cpp
  vesRenderer.cpp
  vesRenderStage.cpp
  vesTexture.cpp
  vesTransformNode.cpp
  vesViewport.cpp
  vesVisitor.cpp
)

# Configure version file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vesVersion.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/vesVersion.h @ONLY)

include_directories(${VES_BINARY_DIR})

ves_add_library(ves "${sources}" "")
target_link_libraries(ves ${deps} EGL)

# Add version info to the target. Currently using a single global version string.
set_target_properties(ves PROPERTIES SOVERSION ${VES_VERSION_STR}
                      VERSION ${VES_VERSION_STR})

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
