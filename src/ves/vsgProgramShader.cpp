/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vsgProgramShader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vsgProgramShader.h"

// --------------------------------------------------------------------includes
#include <iostream>

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vsgProgramShaderInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vsgProgramShader::vsgProgramShader()
{
  this->Internal = new vsgProgramShaderInternal();
}

// -----------------------------------------------------------------------destr
vsgProgramShader::~vsgProgramShader()
{
  delete this->Internal;
}

bool vsgProgramShader::Read()
{
  std::cout << "Read: vsgProgramShader" << std::endl;
  return true;
}

void vsgProgramShader::Render(Painter *render)
{
  std::cout << "Render vsgProgramShader" << std::endl;
}


