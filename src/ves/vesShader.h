/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vesShader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vesShader - Concrete class which implementes vtkProgramShader
// .SECTION Description
// vesShader [refer ISO/IEC 19775-1:2008 31.4.6]

#ifndef __vesShader_h
#define __vesShader_h
// --------------------------------------------------------------------includes
#include "vtkProgramShader.h"

// -----------------------------------------------------------------pre-defines
class vesShaderInternal;

// -----------------------------------------------------------------------class
class vesShader : public vtkProgramShader
{
public:
  // ............................................................public-methods
  vesShader(vesShaderProgram* program);
  ~vesShader();
  bool Read();
  void Render(vtkPainter *render);
protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX

private:
  vesShaderInternal *Internal;
//ETX
  // .......................................................................ETX


};

#endif // __vesShader_h
