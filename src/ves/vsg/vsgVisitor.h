/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

// .NAME vsgVisitor -
// .SECTION Description
// vsgVisitor

#ifndef __vsgVisitor_h
#define __vsgVisitor_h

#include "vsg/Grouping/Transform.h"

#include "vsg/Rendering/ClipPlane.h"
#include "vsg/Rendering/Color.h"
#include "vsg/Rendering/ColorRGBA.h"
#include "vsg/Rendering/Coordinate.h"
#include "vsg/Rendering/IndexedTriangleSet.h"
#include "vsg/Rendering/Normal.h"

#include "vsg/Shader/ComposedShader.h"
#include "vsg/Shader/FloatVertexAttribute.h"
#include "vsg/Shader/Matrix3VertexAttribute.h"
#include "vsg/Shader/Matrix4VertexAttribute.h"
// #include "vsg/Shader/ProgramShader.h"
#include "vsg/Shader/ShaderPart.h"
// #include "vsg/Shader/ShaderProgram.h"

#include "vsg/Shape/Shape.h"
#include "vsg/Shape/Appearance.h"

class vsgVisitorInternal;

class vsgVisitor
{
public:
  vsgVisitor();
  ~vsgVisitor();

  virtual bool visitTransform(vsg::Transform* transform);

  virtual bool visitClipPlane(vsg::ClipPlane* clipPlane);
  virtual bool visitColor(vsg::Color* color);
  virtual bool visitColorRGBA(vsg::ColorRGBA* colorRGBA);
  virtual bool visitCoordinate(vsg::Coordinate* coordinate);
  virtual bool visitIndexedTriangleSet(vsg::IndexedTriangleSet* indexedTriangleSet);
  virtual bool visitNormal(vsg::Normal* normal);

  virtual bool visitComposedShader(vsg::ComposedShader* composedShader);
  virtual bool visitFloatVertexAttribute(vsg::FloatVertexAttribute * data);
  virtual bool visitMatrix3VertexAttribute(vsg::Matrix3VertexAttribute* data);
  virtual bool visitMatrix4VertexAttribute(vsg::Matrix4VertexAttribute* data);
  // virtual bool visitProgramShader(vsg::ProgramShader *programShader);
  virtual bool visitShaderPart(vsg::ShaderPart* shaderPart);
  // virtual bool visitShaderProgram(ShaderProgram *shaderProgram);

  virtual bool visitShape(vsg::Shape *shape);
  virtual bool visitAppearance(vsg::Appearance *appearance);

private:
  vsgVisitorInternal *Internal;
};
#endif // __vsgVisitor_h
