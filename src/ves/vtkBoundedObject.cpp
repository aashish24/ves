/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBoundedObject.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkBoundedObject.h"

// -----------------------------------------------------------------------cnstr
vtkBoundedObject::vtkBoundedObject()
{
  this->BBoxCenter[0] = 0;
  this->BBoxCenter[1] = 0;
  this->BBoxCenter[2] = 0;
  this->BBoxSize[0] = -1;
  this->BBoxSize[1] = -1;
  this->BBoxSize[2] = -1;
}

// -----------------------------------------------------------------------destr
vtkBoundedObject::~vtkBoundedObject()
{

}

void vtkBoundedObject::SetBBoxCenter( vtkVector3f center )
{
  this->BBoxCenter = center;
}

void vtkBoundedObject::SetBBoxSize( vtkVector3f size )
{
  this->BBoxSize = size;
}

void vtkBoundedObject::SetBBoxCenter(vtkVector3f min, vtkVector3f max)
{
  this->BBoxCenter = max + min;
  this->BBoxCenter /= 2.0f;
}

void vtkBoundedObject::SetBBoxSize(vtkVector3f min, vtkVector3f max)
{
  this->BBoxSize = max-min;
  for (int i=0; i<3; ++i) {
    if(this->BBoxSize[i]<0)
    {
      this->BBoxSize *= -1;
    }
  }
}

vtkVector3f vtkBoundedObject::GetBBoxCenter()
{
  return this->BBoxCenter;
}

vtkVector3f vtkBoundedObject::GetBBoxSize()
{
  return this->BBoxSize;
}

float vtkBoundedObject::GetBBoxRadius()
{
  vtkVector3f temp = this->BBoxSize;
  return sqrt(temp[0]*temp[0]+
              temp[1]*temp[1]+
              temp[2]*temp[2]);
}