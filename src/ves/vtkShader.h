/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkShader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkShader - Concrete class which implementes vtkProgramShader
// .SECTION Description
// vtkShader [refer ISO/IEC 19775-1:2008 31.4.6]

#ifndef __vtkShader_h
#define __vtkShader_h
// --------------------------------------------------------------------includes
#include "vtkProgramShader.h"

// -----------------------------------------------------------------pre-defines
class vtkShaderInternal;

// -----------------------------------------------------------------------class
class vtkShader : public vtkProgramShader
{
public:
  // ............................................................public-methods
  vtkShader(vtkShaderProgram* program);
  ~vtkShader();

protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX

private:
  vtkShaderInternal *Internal;
//ETX
  // .......................................................................ETX


};

#endif // __vtkShader_h
