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
  mBBoxCenter[0] = 0;
  mBBoxCenter[1] = 0;
  mBBoxCenter[2] = 0;
  mBBoxSize[0] = -1;
  mBBoxSize[1] = -1;
  mBBoxSize[2] = -1;
}

// -----------------------------------------------------------------------destr
vtkBoundedObject::~vtkBoundedObject()
{

}

void vtkBoundedObject::SetCenter( vtkVector3f center )
{
  mBBoxCenter = center;
}

void vtkBoundedObject::SetSize( vtkVector3f size )
{
  mBBoxSize = size;
}

vtkVector3f vtkBoundedObject::GetCenter()
{
  return mBBoxCenter;
}

vtkVector3f vtkBoundedObject::GetSize()
{
  return mBBoxSize;
}
