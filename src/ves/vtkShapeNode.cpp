/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkShapeNode.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkShapeNode.h"

// --------------------------------------------------------------------includes

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vtkShapeNodeInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vtkShapeNode::vtkShapeNode()
{
  this->Internal = new vtkShapeNodeInternal();
}

// -----------------------------------------------------------------------destr
vtkShapeNode::~vtkShapeNode()
{
  delete this->Internal;
}



