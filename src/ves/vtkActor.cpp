/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkActor.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkActor.h"

// --------------------------------------------------------------------includes
#include "vtkAppearance.h"
#include "vtkShape.h"
#include "vtkMapper.h"
#include "vtkShader.h"
#include "vtkSetGet.h"
#include "vtkPainter.h"

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
class vtkActorInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vtkActor::vtkActor(vtkShader *shader,vtkMapper* mapper,vtkMultitouchWidget *widget)
{
  this->Internal = new vtkActorInternal();
  this->Shape = new vtkShape();
  this->Appearance = new vtkAppearance();
  this->Appearance->SetShader(shader);
  this->Shape->SetGeometry(mapper);
  this->Shape->SetAppearance(this->Appearance);
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
}


// -----------------------------------------------------------------------destr
vtkActor::~vtkActor()
{
  delete this->Internal;
  delete this->Appearance;
  delete this->Shape;
}

vtkMatrix4x4f vtkActor::Eval()
{
  this->Matrix *= this->vtkTransform::Eval();
  return this->Matrix;
}

bool vtkActor::Read()
{
  std::cout << "Read: Actor" <<std::endl;
  for (int i = 0; i < this->Children.size(); ++i)
    {
      this->Children[i]->Read();
    }
  return true;
}

// void vtkActor::Render(vtkShaderProgram *program)
// {
//   if (this->Mapper)
//   {
//   this->Mapper->Render(program);
//   glVertexAttrib4f(program->GetAttribute("a_texcoord"), 0.8, 0.8, 0.8, 1.0);
//   }
// }


void vtkActor::Render(vtkPainter* render)
{
  render->Actor(this);
}

void vtkActor::AddShapeChild(vtkShape* shape)
{
  std::vector<vtkChildNode*> temp;
  temp.push_back(shape);
  SetChildren(temp);

}

void vtkActor::ComputeBounds()
{
  Shape->ComputeBounds();
  SetBBoxCenter(Shape->GetMin(),Shape->GetMax());
  SetBBoxSize(Shape->GetMin(), Shape->GetMax());
}


