/*=========================================================================

  Program:   Visualization Toolkit
  Module:    Appearance.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "Appearance.h"
#include <iostream>

// --------------------------------------------------------------------includes
#include "vsgShaderNode.h"

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct AppearanceInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
Appearance::Appearance()
{
  this->Internal = new AppearanceInternal();
}

// -----------------------------------------------------------------------destr
Appearance::~Appearance()
{
  delete this->Internal;
}

void Appearance::Render(Painter* render)
{
  this->Shader->Render(render);
}

bool Appearance::Read()
{
  this->Shader->Read();
  return true;
}


