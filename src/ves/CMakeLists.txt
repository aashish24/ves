
set(sources
  Painter.cpp
  vesActor.cpp
  vesActorCollection.cpp
  vesByteSwap.cpp
  vesCamera.cpp
  vesCameraSensor.cpp
  vesFileReader.cpp
  vesGMTL.cpp
  vesLegacyReader.cpp
  vesMapper.cpp
  vesMultitouchCamera.cpp
  vesMultitouchWidget.cpp
  vesMultitouchSensorNode.cpp
  vesRenderer.cpp
  vesSceneElement.cpp
  vesSceneRender.cpp
  vesShader.cpp
  vesTexture.cpp
  vesShaderProgram.cpp
  vesSTLReader.cpp
  vesTriangleData.cpp
  vesUniform.cpp

  # ------------------------------------------------------------------------vsg
  vsg/vsgVisitor.cpp

  # -----------------------------------------------------------------------Core
  vsg/Core/vsgBindableNode.cpp
  vsg/Core/vsgChildNode.cpp
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
  vsg/Rendering/ClipPlane.cpp
  vsg/Rendering/Color.cpp
  vsg/Rendering/ColorRGBA.cpp
  vsg/Rendering/Coordinate.cpp
  vsg/Rendering/IndexedTriangleSet.cpp
  vsg/Rendering/Normal.cpp

  # ----------------------------------------------------------------------Shape
  vsg/Shape/vsgAppearanceChildNode.cpp
  vsg/Shape/vsgAppearanceNode.cpp
  vsg/Shape/vsgMaterialNode.cpp
  vsg/Shape/vsgShapeNode.cpp
  vsg/Shape/Appearance.cpp
  vsg/Shape/Shape.cpp

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
  vsg/Shader/vsgVertexAttributeNode.cpp
  vsg/Shader/ComposedShader.cpp
  vsg/Shader/FloatVertexAttribute.cpp
  vsg/Shader/Matrix3VertexAttribute.cpp
  vsg/Shader/Matrix4VertexAttribute.cpp
  #  vsg/Shader/ProgramShader.cpp
  vsg/Shader/ShaderPart.cpp
  #  vsg/Shader/ShaderProgram.cpp
)

ves_add_library(ves "${sources}" "")
