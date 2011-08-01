/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vsgVisitor.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vsgVisitor.h"

// --------------------------------------------------------------------includes
// #include "vsgChildNode.h"
// #include "vsgGeometryNode.h"
// #include "Appearance.h"
// #include "Transform.h"
// #include "Shape.h"
#include <iostream>
#include <vector>

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vsgVisitorInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vsgVisitor::vsgVisitor()
{
  this->Internal = new vsgVisitorInternal();
}

// -----------------------------------------------------------------------destr
vsgVisitor::~vsgVisitor()
{
  delete this->Internal;
}

// // ---------------------------------------------------------------------public
// bool vsgVisitor::visitTransform(vsg::Transform* transform)
// {
//   std::cout << "Visiting: Transform" << std::endl;
//   // If there are children nodes then iternate through and render
//   MFNode children = transform->get_children();
//   if (children.size())
//   {
//     for (int i = 0; i < children.size(); ++i)
//     {
//       children[i]->accept(this);
//     }
//   }
//   return true;
// }

// // ---------------------------------------------------------------------public
// bool vsgVisitor::visitShape(vsg::Shape* shape)
// {
//   std::cout << "Visiting: Shape" <<std::endl;
//   shape->get_appearance()->accept(this);
//   shape->get_geometry()->accept(this);
//   return true;
// }

// // ---------------------------------------------------------------------public
// bool vsgVisitor::visitAppearance(vsg::Appearance *appearance)
// {
//   std::cout << "Visiting: Appearance" << std::endl;
//   MFNode shaders = appearance->get_shaders();
//   for(int i =0; i< shaders.size(); ++i)
//   {
//     shaders[i]->accept(this);
//   }

//   return true;
// }

// // ----------------------------------------------------------------------public
// bool vsgVisitor::visitClipPlane(vsg::ClipPlane* clipPlane)
// {
//   std::cout << "Visiting: ClipPlane" << std::endl;

// return true;
// }

// // ----------------------------------------------------------------------public
// bool vsgVisitor::visitColor(vsg::Color* color)
// {
//   std::cout << "Visiting: Color" << std::endl;

// return true;
// }

// // ----------------------------------------------------------------------public
// bool vsgVisitor::visitColorRGBA(vsg::ColorRGBA* colorRGBA)
// {
//   std::cout << "Visiting: ColorRGBA" << std::endl;

// return true;
// }

// // ----------------------------------------------------------------------public
// bool vsgVisitor::visitCoordinate(vsg::Coordinate* coordinate)
// {
//   std::cout << "Visiting: Coordinate" << std::endl;

// return true;
// }


// // ---------------------------------------------------------------------public
// bool vsgVisitor::visitIndexedTriangleSet(vsg::IndexedTriangleSet* indexedTriangleSet)
// {
//   std::cout << "Visiting: IndexedTriangleSet" <<std::endl;
//   return true;
// }


// // ----------------------------------------------------------------------public
// bool vsgVisitor::visitNormal(vsg::Normal* normal)
// {
//   std::cout << "Visiting: Normal" << std::endl;

// return true;
// }

// // ----------------------------------------------------------------------public
// bool vsgVisitor::visitComposedShader(vsg::ComposedShader* composedShader)
// {
//   std::cout << "Visiting: ComposedShader" << std::endl;

// return true;
// }

// // ----------------------------------------------------------------------public
// bool vsgVisitor::visitFloatVertexAttribute(vsg::FloatVertexAttribute * data)
// {
//   std::cout << "Visiting: FloatVertexAttribute" << std::endl;

// return true;
// }

// // ----------------------------------------------------------------------public
// bool vsgVisitor::visitMatrix3VertexAttribute(vsg::Matrix3VertexAttribute* data)
// {
//   std::cout << "Visiting: Matrix3VertexAttribute" << std::endl;

// return true;
// }

// // ----------------------------------------------------------------------public
// bool vsgVisitor::visitMatrix4VertexAttribute(vsg::Matrix4VertexAttribute* data)
// {
//   std::cout << "Visiting: MatrixVertexAttribute" << std::endl;

// return true;
// }

// // ----------------------------------------------------------------------public
// bool vsgVisitor::visitProgramShader(vsg::ProgramShader *programShader)
// {
//   std::cout << "Visiting: ProgramShader" << std::endl;
//   return true;
// }

// // ----------------------------------------------------------------------public
// bool vsgVisitor::visitShaderProgram(ShaderProgram *shaderProgram)
// {
//   std::cout << "Visiting: ShaderProgram" << std::endl;
//   return true;
// }

// // ----------------------------------------------------------------------public
// bool vsgVisitor::visitShaderPart(vsg::ShaderPart* shaderPart)
// {
//   std::cout << "Visiting: ShaderPart" << std::endl;
//   return true;
// }

// #if 0
// // ---------------------------------------------------------------------public
// bool vsgVisitor::visitShader(vsg::Shader * shader)
// {
//   std::cout << "Visiting: Shader" <<std::endl;
//   std::vector<vsg::ShaderProgram*> temp;
//   if(shader->GetPrograms(&temp))
//     {
//       for (int i = 0; i < temp.size(); ++i)
//         {
//           temp[i]->accept(this);
//         }
//     }
//   return true;
// }

// // ---------------------------------------------------------------------public
// bool vsgVisitor::visitShaderProgram(vsg::ShaderProgram *shaderProg)
// {
//   std::cout << "Visiting: ShaderProgram" <<std::endl;
//   return true;
// }

// # endif

