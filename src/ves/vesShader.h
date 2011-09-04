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
// .NAME vesShader - Concrete class which implementes ProgramShader
// .SECTION Description
// vesShader [refer ISO/IEC 19775-1:2008 31.4.6]

#ifndef __vesShader_h
#define __vesShader_h

#include "ProgramShader.h"

class vesShaderInternal;

class vesShader : public ProgramShader
{
public:
  vesShader(vesShaderProgram* program);
  ~vesShader();
  bool read();
  void render(Painter *render);

private:
  vesShaderInternal *Internal;

};

#endif // __vesShader_h
