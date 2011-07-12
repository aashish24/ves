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
#include "Appearance.h"
#include "Shape.h"
#include "vesMapper.h"
#include "vesShader.h"
#include "vesSetGet.h"
#include "Painter.h"

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
  this->_shape = new Shape();
  this->_appearance = new Appearance();
  this->_appearance->SetShader(shader);
  this->_shape->SetGeometry(mapper);
  this->_shape->SetAppearance(this->_appearance);
  this->Mapper = mapper;        // This is used to make the actor visible again
  AddShapeChild(this->_shape);
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
  delete this->_appearance;
  delete this->_shape;
}

vesMapper* vesActor::GetMapper()
{
  return this->Mapper;
}

vesMatrix4x4f vesActor::Eval()
{
  this->Matrix *= this->Transform::Eval();
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


void vesActor::Render(Painter* render)
{
  render->Actor(this);
}

void vesActor::AddShapeChild(Shape* shape)
{
  std::vector<vsgChildNode*> temp;
  temp.push_back(shape);
  SetChildren(temp);
}

void vesActor::ComputeBounds()
{
  _shape->ComputeBounds();
  SetBBoxCenter(_shape->GetMin(),_shape->GetMax());
  SetBBoxSize(_shape->GetMin(), _shape->GetMax());
}

bool vesActor::SetVisible(bool value)
{
  if(value)
    {
      this->_shape->SetGeometry(this->Mapper);
    }
  else
    {
      this->_shape->SetGeometry(NULL);
    }
  this->Visible = value;
  return true;
}

bool vesActor::isVisible()
{
  return true;
}


