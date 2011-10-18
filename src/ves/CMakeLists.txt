set(sources
  vesActor.cpp
  vesBlend.cpp
  vesBlendFunction.cpp
  vesByteSwap.cpp
  vesCamera.cpp
  vesCameraSensor.cpp
  vesCullVisitor.cpp
  vesFBO.cpp
  vesFileReader.cpp
  vesGMTL.cpp
  vesLegacyReader.cpp
  vesMapper.cpp
  vesMaterial.cpp
  vesMultitouchCamera.cpp
  vesMultitouchWidget.cpp
  vesMultitouchSensorNode.cpp
  vesFBORenderTarget.cpp
  vesRenderer.cpp
  vesRenderToTexture.cpp
  vesShader.cpp
  vesTexture.cpp
  vesShaderProgram.cpp
  vesSTLReader.cpp
  vesTriangleData.cpp
  vesUniform.cpp
  vesVisitor.cpp

  # ------------------------------------------------------------------------vsg

  # -----------------------------------------------------------------------Core
  vsg/Core/vsgBindableNode.cpp
  vsg/Core/vsgNode.cpp
  vsg/Core/vsgSensorNode.cpp

  # -------------------------------------------------------------------Grouping
  vsg/Grouping/vsgBoundedObject.cpp
  vsg/Grouping/vsgGroupingNode.cpp
  vsg/Grouping/Transform.cpp

  # ------------------------------------------------------------------Rendering
  vsg/Rendering/vsgColorNode.cpp
  vsg/Rendering/vsgComposedGeometryNode.cpp
  vsg/Rendering/vsgCoordinateNode.cpp
  vsg/Rendering/vsgGeometricPropertyNode.cpp
  vsg/Rendering/vsgGeometryNode.cpp
  vsg/Rendering/vsgNormalNode.cpp

  # ----------------------------------------------------------------------Shape
  vsg/Shape/vsgAppearanceChildNode.cpp
  vsg/Shape/vsgAppearanceNode.cpp
  vsg/Shape/vsgMaterialNode.cpp

  # --------------------------------------------------------------------Texture
  vsg/Texture/vsgTextureCoordinateNode.cpp
  vsg/Texture/vsgTextureNode.cpp
  vsg/Texture/vsgTexture2DNode.cpp
  vsg/Texture/PixelTexture.cpp

  # -----------------------------------------------------------------Navigation
  vsg/Navigation/Viewpoint.cpp
  vsg/Navigation/vsgViewpointNode.cpp

  # -----------------------------------------------------------------EnvEffects
  vsg/EnvEffects/vsgBackgroundNode.cpp
  vsg/EnvEffects/TextureBackground.cpp

  # ---------------------------------------------------------------------Shader
  vsg/Shader/vsgProgrammableShaderObject.cpp
  vsg/Shader/vsgShaderNode.cpp
)

ves_add_library(ves "${sources}" "")
