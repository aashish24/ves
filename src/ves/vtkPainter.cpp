/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPainter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPainter.h"

// --------------------------------------------------------------------includes
#include "vtkGMTL.h"
#include <iostream>
#include <vector>
#include "vtkChildNode.h"
#include "vtkGeometryNode.h"
#include "vtkAppearance.h"

// -----------------------------------------------------------------------macro


// -----------------------------------------------------------------------cnstr
vtkPainter::vtkPainter()
{

}

// -----------------------------------------------------------------------destr
vtkPainter::~vtkPainter()
{

}

// ----------------------------------------------------------------------public
// void vtkPainter::Transform(vtkTransform* transform)
// {
//   // Push the transformation
//   this->Internal->Push(transform->Eval());

//   // If there are children nodes then tternate through and render
//   std::vector<vtkChildNode*> children;
//   if (transform->GetChildren(&children))
//     {
//     for (int i = 0; i < children.size(); ++i)
//       {
//       children[i]->Render(this);
//       }
//     }

//   // Pop the transformation
//   this->Internal->Pop();
// }


// ----------------------------------------------------------------------public
void vtkPainter::Shader(vtkShader * shader)
{
  std::cout << "Render: Shader" <<std::endl;
  std::vector<vtkShaderProgram*> temp;
  if(shader->GetPrograms(&temp))
    {
      for (int i = 0; i < temp.size(); ++i)
        {
          temp[i]->Render(this);
        }
    }
}

// ----------------------------------------------------------------------public
void vtkPainter::ShaderProgram(vtkShaderProgram *shaderProg)
{
  std::cout << "Render: ShaderProgram" <<std::endl;
  shaderProg->Use();
}

// ----------------------------------------------------------------------public
void vtkPainter::Mapper(vtkMapper *mapper)
{
  std::cout << "Render: Mapper" << std::endl;
}

// ----------------------------------------------------------------------public
void vtkPainter::Actor(vtkActor * actor)
{
  std::cout << "Render: Actor" <<std::endl;
  this->Push(actor->Eval());
  vector<vtkChildNode *> temp;
  actor->GetChildren(&temp);
  temp[0]->Render(this);
  this->Pop();
}

// ----------------------------------------------------------------------public
void vtkPainter::ActorCollection(vtkActorCollection *actor)
{
  std::cout << "Render: ActorCollection" <<std::endl;
  // Push the transformation
  this->Push(actor->Eval());

  // If there are children nodes then tternate through and render
  std::vector<vtkChildNode*> children;
  if (actor->GetChildren(&children))
    {
    for (int i = 0; i < children.size(); ++i)
      {
      children[i]->Render(this);
      }
    }

  // Pop the transformation
  this->Pop();
}

// ----------------------------------------------------------------------public
void vtkPainter::Shape(vtkShape* shape)
{
  std::cout << "Render: Shape" <<std::endl;
  shape->GetAppearance() -> Render(this);
  shape->GetGeometry() -> Render(this);

  std::vector<vtkShaderProgram*> temp;
  vtkShaderProgram * program;
  vtkAppearance *appear = (vtkAppearance*) shape->GetAppearance();
  vtkProgramShader *prog = (vtkProgramShader*) appear->GetShader();
  if(prog->GetPrograms(&temp))
    {
      program = temp[0]; // currently we are only using one shader
    }

  vtkMapper* mapper = (vtkMapper*)shape->GetGeometry();

  // Work out the appropriate matrices
  vtkMatrix4x4f mvp;
  //mvp = _proj * _view * _model * (*mActor)();
  vtkMatrix4x4f mv;
  mv = this->View * this->Model * this->Eval();
  mvp = this->Projection * mv;

  vtkMatrix3x3f normal_matrix = makeNormalMatrix3x3f(makeTransposeMatrix4x4(makeInverseMatrix4x4 (mv)));
  //vtkMatrix4x4f temp = makeNormalizedMatrix4x4(makeTransposeMatrix4x4(_vie);
  vtkPoint3f lightDir = vtkPoint3f(0.0,0.0,.650);

  vtkVector3f light(lightDir.mData[0],lightDir.mData[1],lightDir.mData[2]);
  program->SetUniformMatrix4x4f("u_mvpMatrix",mvp);
  program->SetUniformMatrix3x3f("u_normalMatrix",normal_matrix);
  program->SetUniformVector3f("u_ecLightDir",light);

  // Clear the buffers
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//  glEnable(GL_DEPTH_TEST);

  // Enable our attribute arrays
  program->EnableVertexArray("a_vertex");
  program->EnableVertexArray("a_normal");

  glVertexAttrib4f(program->GetAttribute("a_texcoord"), 0.8, 0.8, 0.8, 1.0);
  glVertexAttribPointer(program->GetAttribute("a_vertex"),
                        3,
                        GL_FLOAT,
                        0,
                        6 * sizeof(float),
                        &(mapper->GetData()->GetPoints()[0]));
  glVertexAttribPointer(program->GetAttribute("a_normal"),
                        3,
                        GL_FLOAT,
                        0,
                        6 * sizeof(float),
                        mapper->GetData()->GetPoints()[0].normal.mData);

  // draw triangles
  glDrawElements(GL_TRIANGLES,
                 mapper->GetData()->GetTriangles().size() * 3,
                 GL_UNSIGNED_SHORT,
                 &mapper->GetData()->GetTriangles()[0]);

  // draw lines
  glDrawElements(GL_LINES,
                 mapper->GetData()->GetLines().size() * 2,
                 GL_UNSIGNED_SHORT,
                 &mapper->GetData()->GetLines()[0]);

  //glBindBuffer(GL_ARRAY_BUFFER, 0);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  program->DisableVertexArray("a_vertex");
  program->DisableVertexArray("a_normal");

}

// --------------------------------------------------------------------internal
void vtkPainter::Push(vtkMatrix4x4f mat)
{
  std::cout << "Pushed Matrix" <<std::endl;
  MatrixStack.push_back(mat);
}

// --------------------------------------------------------------------internal
void vtkPainter::Pop()
{
  std::cout << "Popped Matrix" << std::endl;
  MatrixStack.pop_back();

}


// --------------------------------------------------------------------internal
vtkMatrix4x4f vtkPainter::Eval()
{
  vtkMatrix4x4f temp;
  for (int i = 0; i < MatrixStack.size(); ++i)
    {
      temp *= MatrixStack[i];
    }
  return temp;
}
