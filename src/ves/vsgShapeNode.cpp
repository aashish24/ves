/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vsgShapeNode.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vsgShapeNode.h"

// --------------------------------------------------------------------includes

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vsgShapeNodeInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vsgShapeNode::vsgShapeNode()
{
  this->Internal = new vsgShapeNodeInternal();
}

// -----------------------------------------------------------------------destr
vsgShapeNode::~vsgShapeNode()
{
  delete this->Internal;
}



