project(ves)
cmake_minimum_required(VERSION 2.8)

set(sources

  Painter.cpp
  ProgramShader.cpp
  vesActor.cpp
  vesActorCollection.cpp
  vesByteSwap.cpp
  vesCamera.cpp
  vesCameraSensor.cpp
  vesController.cpp
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
  vesShaderProgram.cpp
  vesSTLReader.cpp
  vesTriangleData.cpp



  vsg/Shape/vsgAppearanceNode.cpp
  vsg/Grouping/vsgBoundedObject.cpp

  vsg/Grouping/Transform.cpp

  
  vsg/Shape/vsgShapeNode.cpp
  vsg/Shape/Appearance.cpp
  vsg/Shape/Shape.cpp

  # ------------------------------------------------------------------------vsg
  vsg/vsgVisitor.cpp

  # -----------------------------------------------------------------------Core
  vsg/Core/vsgBindableNode.cpp
  vsg/Core/vsgChildNode.cpp
  vsg/Core/vsgNode.cpp
  vsg/Core/vsgSensorNode.cpp

  # -------------------------------------------------------------------Grouping
  vsg/Grouping/vsgGroupingNode.cpp

  # -----------------------------------------------------------------Navigation
  vsg/Navigation/Viewpoint.cpp
  vsg/Navigation/vsgViewpointNode.cpp

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

  # ----------------------------------------------------------------------Shape
  vsg/Shape/vsgAppearanceChildNode.cpp

  # --------------------------------------------------------------------Texture
  vsg/Texture/vsgTextureCoordinateNode.cpp

)

include_directories( vsg
                     vsg/Core
                     vsg/Grouping
                     vsg/Navigation
                     vsg/Rendering
                     vsg/Shader
                     vsg/Shape
                     vsg/Utility )

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/..)
add_library(ves ${sources})
