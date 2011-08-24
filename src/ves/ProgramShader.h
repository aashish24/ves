/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/
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
