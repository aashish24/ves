/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vsgVisitor.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vsgVisitor -
// .SECTION Description
// vsgVisitor

#ifndef __vsgVisitor_h
#define __vsgVisitor_h
// --------------------------------------------------------------------includes
// #include "vsg/Group/Transform.h"

// #include "vsg/Shape/Shape.h"
// #include "vsg/Shape/Appearance.h"

#include "vsg/Rendering/ClipPlane.h"
#include "vsg/Rendering/Color.h"
#include "vsg/Rendering/ColorRGBA.h"
#include "vsg/Rendering/Coordinate.h"
#include "vsg/Rendering/IndexedTriangleSet.h"
// #include "vsg/Render/Normal.h"

// #include "vsg/Shader/ComposedShader.h"
// #include "vsg/Shader/FloatVertexAttribute.h"
// #include "vsg/Shader/Matrix3VertexAttribute.h"
// #include "vsg/Shader/Matrix4VertexAttribute.h"
// #include "vsg/Shader/ProgramShader.h"
// #include "vsg/Shader/ShaderPart.h"
// #include "vsg/Shader/ShaderProgram.h"

// -----------------------------------------------------------------pre-defines
class vsgVisitorInternal;

// -----------------------------------------------------------------------class
class vsgVisitor
{
public:
  // ............................................................public-methods
  vsgVisitor();
  ~vsgVisitor();

  // virtual bool visitTransform(vsg::Transform* transform);

  // virtual bool visitShape(vsg::Shape *shape);
  // virtual bool visitAppearance(vsg::Appearance *appearance);

  virtual bool visitClipPlane(vsg::ClipPlane* clipPlane);
  virtual bool visitColor(vsg::Color* color);
  virtual bool visitColorRGBA(vsg::ColorRGBA* colorRGBA);
  virtual bool visitCoordinate(vsg::Coordinate* coordinate);
  virtual bool visitIndexedTriangleSet(vsg::IndexedTriangleSet* indexedTriangleSet);
  // virtual bool visitNormal(vsg::Normal* normal);

  // virtual bool visitComposedShader(vsg::ComposedShader* composedShader);
  // virtual bool visitFloatVertexAttribute(vsg::FloatVertexAttribute * data);
  // virtual bool visitMatrix3VertexAttribute(vsg::Matrix3VertexAttribute* data);
  // virtual bool visitMatrix4VertexAttribute(vsg::Matrix4VertexAttribute* data);
  // virtual bool visitProgramShader(vsg::ProgramShader *programShader);
  // virtual bool visitShaderPart(vsg::ShaderPart* shaderPart);
  // virtual bool visitShaderProgram(ShaderProgram *shaderProgram);
protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX
private:
  vsgVisitorInternal *Internal;
//ETX
  // .......................................................................ETX
};
#endif // __vsgVisitor_h
