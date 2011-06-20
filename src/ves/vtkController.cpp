/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkController.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkController.h"

// --------------------------------------------------------------------includes

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vtkControllerInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vtkController::vtkController()
{
  this->Internal = new vtkControllerInternal();
}

// -----------------------------------------------------------------------destr
vtkController::~vtkController()
{
  delete this->Internal;
}


void vtkController::Transform(vtkTransform* object)
{

}

void vtkController::Shape(vtkShape* object)
{

}

