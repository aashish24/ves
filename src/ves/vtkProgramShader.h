/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkProgramShader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkProgramShader - Defines a collection of individual shaders.
// .SECTION Description
// vtkProgramShader [refer 31.4.6]

#ifndef __vtkProgramShader_h
#define __vtkProgramShader_h
// --------------------------------------------------------------------includes
#include "vtkShaderNode.h"
#include "vtkSetGet.h"
#include <vector>

// -----------------------------------------------------------------pre-defines
class vtkProgramShaderInternal;
class vtkShaderProgram;

// -----------------------------------------------------------------------class
class vtkProgramShader : public vtkShaderNode
{
public:
  // ............................................................public-methods
  vtkProgramShader();
  ~vtkProgramShader();
  vtkSetGetVectorMacro(Programs,vtkShaderProgram*)
  bool Read();
  void Render(vtkPainter *render);
protected:
  // ...........................................................protected-ivars
  std::vector<vtkShaderProgram*> Programs;
protected:
//BTX
  // .......................................................................BTX

private:
  vtkProgramShaderInternal *Internal;

//ETX
  // .......................................................................ETX
};

#endif // __vtkProgramShader_h
