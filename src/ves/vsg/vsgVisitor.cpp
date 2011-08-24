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

#include "vsgVisitor.h"

// #include "vsgChildNode.h"
// #include "vsgGeometryNode.h"
// #include "Appearance.h"
// #include "Transform.h"
// #include "Shape.h"
#include <iostream>
#include <vector>

// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vsgVisitorInternal
{
  double value; // sample
};

vsgVisitor::vsgVisitor()
{
  this->Internal = new vsgVisitorInternal();
}

vsgVisitor::~vsgVisitor()
{
  delete this->Internal;
}

bool vsgVisitor::visitTransform(vsg::Transform* transform)
{
  std::cout << "Visiting: Transform" << std::endl;
  // If there are children nodes then iternate through and render
  MFNode children = transform->get_children();
  if (children.size())
  {
    for (int i = 0; i < children.size(); ++i)
    {
      children[i]->accept(this);
    }
  }
  return true;
}

bool vsgVisitor::visitClipPlane(vsg::ClipPlane* clipPlane)
{
  std::cout << "Visiting: ClipPlane" << std::endl;
  return true;
}

bool vsgVisitor::visitColor(vsg::Color* color)
{
  std::cout << "Visiting: Color" << std::endl;
  return true;
}

bool vsgVisitor::visitColorRGBA(vsg::ColorRGBA* colorRGBA)
{
  std::cout << "Visiting: ColorRGBA" << std::endl;
  return true;
}

bool vsgVisitor::visitCoordinate(vsg::Coordinate* coordinate)
{
  std::cout << "Visiting: Coordinate" << std::endl;
  return true;
}

bool vsgVisitor::visitIndexedTriangleSet(vsg::IndexedTriangleSet* indexedTriangleSet)
{
  std::cout << "Visiting: IndexedTriangleSet" <<std::endl;
  return true;
}

bool vsgVisitor::visitNormal(vsg::Normal* normal)
{
  std::cout << "Visiting: Normal" << std::endl;
  return true;
}

bool vsgVisitor::visitComposedShader(vsg::ComposedShader* composedShader)
{
  std::cout << "Visiting: ComposedShader" << std::endl;

return true;
}

bool vsgVisitor::visitFloatVertexAttribute(vsg::FloatVertexAttribute * data)
{
  std::cout << "Visiting: FloatVertexAttribute" << std::endl;

return true;
}

bool vsgVisitor::visitMatrix3VertexAttribute(vsg::Matrix3VertexAttribute* data)
{
  std::cout << "Visiting: Matrix3VertexAttribute" << std::endl;

return true;
}

bool vsgVisitor::visitMatrix4VertexAttribute(vsg::Matrix4VertexAttribute* data)
{
  std::cout << "Visiting: MatrixVertexAttribute" << std::endl;

return true;
}

bool vsgVisitor::visitShaderPart(vsg::ShaderPart* shaderPart)
{
  std::cout << "Visiting: ShaderPart" << std::endl;
  return true;
}

bool vsgVisitor::visitShape(vsg::Shape* shape)
{
  std::cout << "Visiting: Shape" <<std::endl;
  shape->get_appearance()->accept(this);
  shape->get_geometry()->accept(this);
  return true;
}

bool vsgVisitor::visitAppearance(vsg::Appearance *appearance)
{
  std::cout << "Visiting: Appearance" << std::endl;
  MFNode shaders = appearance->get_shaders();
  for(int i =0; i< shaders.size(); ++i)
  {
    shaders[i]->accept(this);
  }

  return true;
}
