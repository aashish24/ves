set(sources
  vesActor.cpp
  vesBackground.cpp
  vesBlend.cpp
  vesBlendFunction.cpp
  vesBoundingObject.cpp
  vesCamera.cpp
  vesCullVisitor.cpp
  vesDepth.cpp
  vesFBO.cpp
  vesFBORenderTarget.cpp
  vesEigen.cpp
  vesGeometryData.cpp
  vesGroupNode.cpp
  vesMapper.cpp
  vesMaterial.cpp
  vesNode.cpp
  vesOpenGLSupport.cpp
  vesRenderer.cpp
  vesRenderStage.cpp
  vesRenderToTexture.cpp
  vesShader.cpp
  vesTexture.cpp
  vesTransformNode.cpp
  vesShaderProgram.cpp
  vesUniform.cpp
  vesViewport.cpp
  vesVisitor.cpp
)

# Configure version file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vesVersion.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/vesVersion.h @ONLY)

ves_add_library(ves "${sources}" "${VES_OPENGL_LIBRARIES}")

# Add version info to the target. Currently using a single global version string.
set_target_properties(ves PROPERTIES SOVERSION ${VES_VERSION_STR}
                      VERSION ${VES_VERSION_STR})

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
