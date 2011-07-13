/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vsgProgramShader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vsgProgramShader - Defines a collection of individual shaders.
// .SECTION Description
// vsgProgramShader [refer 31.4.6]

#ifndef __vsgProgramShader_h
#define __vsgProgramShader_h
// --------------------------------------------------------------------includes
#include "vsgShaderNode.h"
#include "vesSetGet.h"
#include <vector>

// -----------------------------------------------------------------pre-defines
class vsgProgramShaderInternal;
class vesShaderProgram;

// -----------------------------------------------------------------------class
class vsgProgramShader : public vsgShaderNode
{
public:
  // ............................................................public-methods
  vsgProgramShader();
  ~vsgProgramShader();
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
  vsgProgramShaderInternal *Internal;

//ETX
  // .......................................................................ETX
};

#endif // __vsgProgramShader_h
