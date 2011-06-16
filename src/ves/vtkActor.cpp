//
//  vtkActor.cpp
//  kiwi
//
//  Created by kitware on 5/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vtkActor.h"
#include "vtkMapper.h"

vtkActor::vtkActor()
{
 this->Mapper = 0;
  this->Shader =0;
}
vtkActor::vtkActor(vtkShader *shader,vtkMapper* mapper): 
  Shader(shader),Mapper(mapper)
{
}

vtkActor::~vtkActor()
{
  if(this->Mapper)
  {
  delete this->Mapper;
  }
}

void vtkActor::AddChild(vtkActor* actor)
{
  vector<vtkChildNode*> actorList;
  actorList.push_back(actor);
  AddChildren(actorList);
}
vtkMatrix4x4f vtkActor::Eval()
{
//  vtkMatrix4x4f temp = this->vtkTransform::Eval();
//  vtkMatrix4x4f temp1 = this->Mapper->Eval();
//  return temp*temp1;
  return vtkTransform::Eval() * this->Mapper->Eval();
} 

void vtkActor::Read()
{
  for (int i =0; i<this->Children.size(); ++i)
  {
    vtkActor* child = (vtkActor*) this->Children[i];
    child->Read();
  }
  if(this->Mapper)
  {
    this->Mapper->Read();
  }
}

void vtkActor::Render(vtkShaderProgram *program)
{
  for (int i =0; i<this->Children.size(); ++i)
  {
    vtkActor* child = (vtkActor*) this->Children[i];
    child->Render(program);
  }
  if (this->Mapper)
  {
  this->Mapper->Render(program);
  glVertexAttrib4f(program->GetAttribute("a_texcoord"), 0.8, 0.8, 0.8, 1.0);
  }
}



