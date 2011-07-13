/*=========================================================================

  Program:   Visualization Toolkit
  Module:    ProgramShader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME ProgramShader - Defines a collection of individual shaders.
// .SECTION Description
// ProgramShader [refer 31.4.6]

#ifndef __ProgramShader_h
#define __ProgramShader_h
// --------------------------------------------------------------------includes
#include "vsgShaderNode.h"
#include "vesSetGet.h"
#include <vector>

// -----------------------------------------------------------------pre-defines
class ProgramShaderInternal;
class vesShaderProgram;

// -----------------------------------------------------------------------class
class ProgramShader : public vsgShaderNode
{
public:
  // ............................................................public-methods
  ProgramShader();
  ~ProgramShader();
  vesSetGetVectorMacro(Programs,vesShaderProgram*)
  bool Read();
  void Render(Painter *render);
protected:
  // ...........................................................protected-ivars
  std::vector<vesShaderProgram*> Programs;
protected:
//BTX
  // .......................................................................BTX

private:
  ProgramShaderInternal *Internal;

//ETX
  // .......................................................................ETX
};

#endif // __ProgramShader_h
