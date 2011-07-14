project(ves)
cmake_minimum_required(VERSION 2.8)

set(sources
  Appearance.cpp
  Painter.cpp
  ProgramShader.cpp
  Shape.cxx
  Transform.cpp
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
  vsg/Core/vsgBindableNode.cpp
  vsg/Core/vsgChildNode.cpp
  vsg/Core/vsgNode.cpp
  vsg/Core/vsgSensorNode.cpp
  vsg/Navigation/Viewpoint.cpp
  vsg/Navigation/vsgViewpointNode.cpp
  vsgAppearanceChildNode.cpp
  vsgAppearanceNode.cpp
  vsgBoundedObject.cpp
  vsgGeometryNode.cpp
  vsgGroupingNode.cpp
  vsgProgrammableShaderObject.cpp
  vsgShaderNode.cxx
  vsgShapeNode.cpp
)

include_directories(vsg vsg/Core vsg/Navigation vsg/Utility)
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/..)
add_library(ves ${sources})
