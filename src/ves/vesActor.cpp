/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vesActor.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vesActor.h"

// --------------------------------------------------------------------includes
#include "vtkAppearance.h"
#include "vtkShape.h"
#include "vesMapper.h"
#include "vesShader.h"
#include "vesSetGet.h"
#include "vtkPainter.h"

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
class vesActorInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vesActor::vesActor(vesShader *shader,vesMapper* mapper,vesMultitouchWidget *widget)
{
  this->Internal = new vesActorInternal();
  this->Shape = new vtkShape();
  this->Appearance = new vtkAppearance();
  this->Appearance->SetShader(shader);
  this->Shape->SetGeometry(mapper);
  this->Shape->SetAppearance(this->Appearance);
  this->Mapper = mapper;        // This is used to make the actor visible again
  AddShapeChild(this->Shape);
  if(widget)
    {
      this->Sensor = true;
      this->Widget = widget;
      std::cout<< "Adding widget interactor" <<std::endl;
    }
  else
    {
      this->Sensor = false;
    }
  this->Visible = true;
}


// -----------------------------------------------------------------------destr
vesActor::~vesActor()
{
  delete this->Internal;
  delete this->Appearance;
  delete this->Shape;
}

vesMatrix4x4f vesActor::Eval()
{
  this->Matrix *= this->vtkTransform::Eval();
  return this->Matrix;
}

bool vesActor::Read()
{
  std::cout << "Read: Actor" <<std::endl;
  for (int i = 0; i < this->Children.size(); ++i)
    {
      this->Children[i]->Read();
    }
  return true;
}

// void vesActor::Render(vesShaderProgram *program)
// {
//   if (this->Mapper)
//   {
//   this->Mapper->Render(program);
//   glVertexAttrib4f(program->GetAttribute("a_texcoord"), 0.8, 0.8, 0.8, 1.0);
//   }
// }


void vesActor::Render(vtkPainter* render)
{
  render->Actor(this);
}

void vesActor::AddShapeChild(vtkShape* shape)
{
  std::vector<vtkChildNode*> temp;
  temp.push_back(shape);
  SetChildren(temp);
}

void vesActor::ComputeBounds()
{
  Shape->ComputeBounds();
  SetBBoxCenter(Shape->GetMin(),Shape->GetMax());
  SetBBoxSize(Shape->GetMin(), Shape->GetMax());
}

bool vesActor::SetVisible(bool value)
{
  if(value)
    {
      this->Shape->SetGeometry(this->Mapper);
    }
  else
    {
      this->Shape->SetGeometry(NULL);
    }
  this->Visible = value;
}

bool vesActor::isVisible()
{

}


