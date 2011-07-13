/*=========================================================================

  Program:   Visualization Toolkit
  Module:    ProgramShader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "ProgramShader.h"

// --------------------------------------------------------------------includes
#include <iostream>

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct ProgramShaderInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
ProgramShader::ProgramShader()
{
  this->Internal = new ProgramShaderInternal();
}

// -----------------------------------------------------------------------destr
ProgramShader::~ProgramShader()
{
  delete this->Internal;
}

bool ProgramShader::Read()
{
  std::cout << "Read: ProgramShader" << std::endl;
  return true;
}

void ProgramShader::Render(Painter *render)
{
  std::cout << "Render ProgramShader" << std::endl;
}


