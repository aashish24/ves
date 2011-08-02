/*=========================================================================

  Program:   Visualization Toolkit
  Module:    Painter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "Painter.h"

// --------------------------------------------------------------------includes
#include "vesGMTL.h"
#include <iostream>
#include <vector>
#include "vsgChildNode.h"
#include "vsgGeometryNode.h"
#include "vsg/Shape/Appearance.h"
#include "vesMultitouchWidget.h"

// -----------------------------------------------------------------------macro
namespace {
void PrintMatrix(vesMatrix4x4f mv)
{
  for (int i = 0; i < 4; ++i)
    {
    std::cerr << mv[i][0] << "," << mv[i][1] << "," << mv[i][2] << "," << mv[i][3] << std::endl;
    }
}
}

// -----------------------------------------------------------------------cnstr
Painter::Painter()
{

}

// -----------------------------------------------------------------------destr
Painter::~Painter()
{

}

// ----------------------------------------------------------------------public
void Painter::Texture(vesTexture* textureBackground)
{

}

// ----------------------------------------------------------------------public
void Painter::Camera(vesCamera *camera)
{
  this->Texture(_textureBackground);
  this->Push(camera->eval());
  // If there are children nodes then tternate through and render
  MFNode children = camera->get_children();
  if (children.size())
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
void Painter::Shader(vesShader * shader)
{
  //std::cout << "Render: Shader" <<std::endl;
  std::vector<vesShaderProgram*> temp;
  if(shader->GetPrograms(&temp))
    {
      for (int i = 0; i < temp.size(); ++i)
        {
          temp[i]->Render(this);
        }
    }
}

// ----------------------------------------------------------------------public
void Painter::ShaderProgram(vesShaderProgram *shaderProg)
{
  //std::cout << "Render: ShaderProgram" <<std::endl;
  shaderProg->Use();
}

// ----------------------------------------------------------------------public
void Painter::Mapper(vesMapper *mapper)
{
  //std::cout << "Render: Mapper" << std::endl;
}

// ----------------------------------------------------------------------public
void Painter::Actor(vesActor * actor)
{
  //std::cout << "Render: Actor" <<std::endl;
  if(actor->GetSensor())
    {
      if(actor->GetWidget()->isActive())
        {
          //std::cout<<"translating the widget" <<std::endl;
          // actor->set_translation(actor->GetWidget()->GetTranslation());
          // actor->set_rotation(actor->GetWidget()->GetRotation());
          // actor->SetScale(actor->GetWidget()->GetScale());
        }
    }
  this->Push(actor->Eval());
  MFNode temp;
  temp = actor->get_children();
  temp[0]->Render(this);
  this->Pop();
}

// ----------------------------------------------------------------------public
void Painter::ActorCollection(vesActorCollection *actor)
{
  //std::cout << "Render: ActorCollection" <<std::endl;
  // Push the transformation
  this->Push(actor->Eval());

  // If there are children nodes then tternate through and render
  MFNode children = actor->get_children();;
  if (children.size())
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
void Painter::visitShape(vsg::Shape* shape)
{
  //std::cout << "Render: Shape" <<std::endl;
  shape->get_appearance() -> Render(this);
  if(shape->get_geometry())
    {
    shape->get_geometry() -> Render(this);
    }
  else
    {
    return;
    }

  std::vector<vesShaderProgram*> temp;
  vesShaderProgram * program;
  vsg::Appearance *appear = (vsg::Appearance*) shape->get_appearance();
  ProgramShader *prog = (ProgramShader*) appear->get_shaders()[0];
  if(prog->GetPrograms(&temp))
    {
      program = temp[0]; // currently we are only using one shader
    }

  vesMapper* mapper = (vesMapper*)shape->get_geometry();

  // Model-view matrix is everything except the top level matrix (the projection matrix).
  // This is needed for normal calculation.
  vesMatrix4x4f mv = this->Eval(1);

  // The model-view-projection matrix includes everything.
  vesMatrix4x4f mvp = this->Eval(0);

  vesMatrix3x3f normal_matrix = makeNormalMatrix3x3f(makeTransposeMatrix4x4(makeInverseMatrix4x4 (mv)));
  vtkPoint3f lightDir = vtkPoint3f(0.0,0.0,.650);

  vesVector3f light(lightDir.mData[0],lightDir.mData[1],lightDir.mData[2]);
  program->SetUniformMatrix4x4f("u_mvpMatrix",mvp);
  program->SetUniformMatrix3x3f("u_normalMatrix",normal_matrix);
  program->SetUniformVector3f("u_ecLightDir",light);

  // Clear the buffers
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Enable our attribute arrays
  program->EnableVertexArray("a_vertex");
  program->EnableVertexArray("a_normal");

//  glVertexAttrib4f(program->GetAttribute("a_texcoord"), 0.8, 0.8, 0.8, 1.0);
  glVertexAttrib4f(program->GetAttribute("a_texcoord"),
                   mapper->GetRed(),
                   mapper->GetGreen(),
                   mapper->GetBlue(),
                   mapper->GetAlpha());

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
  glDisable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  program->DisableVertexArray("a_vertex");
  program->DisableVertexArray("a_normal");

}

// --------------------------------------------------------------------internal
void Painter::Push(vesMatrix4x4f mat)
{
  MatrixStack.push_back(mat);
}

// --------------------------------------------------------------------internal
void Painter::Pop()
{
  MatrixStack.pop_back();
}


// --------------------------------------------------------------------internal
vesMatrix4x4f Painter::Eval(int startIndex)
{
  vesMatrix4x4f temp;
  for (int i = startIndex; i < MatrixStack.size(); ++i)
    {
    temp *= MatrixStack[i];
    }
  return temp;
}
