/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkShader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkShader.h"

// --------------------------------------------------------------------includes
#include "vtkShaderProgram.h"
#include <vector>

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vtkShaderInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vtkShader::vtkShader(vtkShaderProgram* shader)
{
  std::vector<vtkShaderProgram*> temp;
  temp.push_back(shader);
  SetPrograms(temp);
  this->Internal = new vtkShaderInternal();
}

// -----------------------------------------------------------------------destr
vtkShader::~vtkShader()
{
  delete this->Internal;
}

