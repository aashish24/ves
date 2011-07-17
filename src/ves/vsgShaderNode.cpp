/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vsgShaderNode.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vsgShaderNode.h"

// --------------------------------------------------------------------includes

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vsgShaderNodeInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vsgShaderNode::vsgShaderNode()
{
  this->Language = "GLSL";
  this->Internal = new vsgShaderNodeInternal();
}

// -----------------------------------------------------------------------destr
vsgShaderNode::~vsgShaderNode()
{
  delete this->Internal;
}


