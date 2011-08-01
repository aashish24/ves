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
  vsg/Core/vsgChildNode.cpp
  vsg/Core/vsgNode.cpp
  vsg/Core/vsgSensorNode.cpp
  vsg/Navigation/Viewpoint.cpp
  vsg/Navigation/vsgViewpointNode.cpp
  vsg/Shape/vsgAppearanceChildNode.cpp
  vsg/Shape/vsgAppearanceNode.cpp
  vsg/Grouping/vsgBoundedObject.cpp
  vsg/Grouping/vsgGroupingNode.cpp
  vsg/Grouping/Transform.cpp
  vsg/Rendering/vsgGeometryNode.cpp
  vsg/Shader/vsgProgrammableShaderObject.cpp
  vsg/Shader/vsgShaderNode.cpp
  vsg/Shape/vsgShapeNode.cpp
  vsg/Shape/Appearance.cpp
  vsg/Shape/Shape.cpp

  # Brought in from cleaner repo -nix
  # Core
  vsg/Core/vsgBindableNode.cpp
  vsg/Texture/vsgTextureCoordinateNode.cpp
  vsg/Rendering/vsgGeometricPropertyNode.cpp
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
