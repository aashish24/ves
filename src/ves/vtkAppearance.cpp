/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAppearance.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkAppearance.h"
#include <iostream>

// --------------------------------------------------------------------includes
#include "vtkShaderNode.h"

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vtkAppearanceInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vtkAppearance::vtkAppearance()
{
  this->Internal = new vtkAppearanceInternal();
}

// -----------------------------------------------------------------------destr
vtkAppearance::~vtkAppearance()
{
  delete this->Internal;
}

void vtkAppearance::Render(vtkPainter* render)
{
  this->Shader->Render(render);
}

bool vtkAppearance::Read()
{
  this->Shader->Read();
  return true;
}


