/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vesShader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vesShader.h"

// --------------------------------------------------------------------includes
#include "vesShaderProgram.h"
#include "Painter.h"
#include <vector>

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vesShaderInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vesShader::vesShader(vesShaderProgram* shader)
{
  std::vector<vesShaderProgram*> temp;
  temp.push_back(shader);
  SetPrograms(temp);
  this->Internal = new vesShaderInternal();
}

// -----------------------------------------------------------------------destr
vesShader::~vesShader()
{
  delete this->Internal;
}

// ----------------------------------------------------------------------public
bool vesShader::Read()
{
  //std::cout << "Read: Shader" <<std::endl;
  return true;
}

// ----------------------------------------------------------------------public
void vesShader::Render(Painter *render)
{
  render->Shader(this);
}

