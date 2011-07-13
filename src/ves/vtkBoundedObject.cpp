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
  this->Min[0] = 0;
  this->Min[1] = 0;
  this->Min[2] = 0;
  this->Max[0] = 0;
  this->Max[1] = 0;
  this->Max[2] = 0;
}

// -----------------------------------------------------------------------destr
vtkBoundedObject::~vtkBoundedObject()
{

}

void vtkBoundedObject::SetBBoxCenter(vesVector3f min, vesVector3f max)
{
  this->Min = (min);
  this->Max = (max);
  this->BBoxCenter = max + min;
  this->BBoxCenter /= 2.0f;
}

void vtkBoundedObject::SetBBoxSize(vesVector3f min, vesVector3f max)
{
  this->Min = (min);
  this->Max = (max);
  this->BBoxSize = max-min;
  for (int i=0; i<3; ++i) {
    if(this->BBoxSize[i]<0)
    {
      this->BBoxSize *= -1;
    }
  }
}

float vtkBoundedObject::GetBBoxRadius()
{
  vesVector3f temp = this->BBoxSize;
  return sqrt(temp[0]*temp[0]+
              temp[1]*temp[1]+
              temp[2]*temp[2]);
}
