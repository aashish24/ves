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

#include "vesShader.h"

#include "vesShaderProgram.h"
#include "Painter.h"
#include <vector>

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vesShaderInternal
{
  double value; // sample
};

vesShader::vesShader(vesShaderProgram* shader)
{
  std::vector<vesShaderProgram*> temp;
  temp.push_back(shader);
  SetPrograms(temp);
  this->Internal = new vesShaderInternal();
}

vesShader::~vesShader()
{
  delete this->Internal;
}

bool vesShader::Read()
{
  //std::cout << "Read: Shader" <<std::endl;
  return true;
}

void vesShader::Render(Painter *render)
{
  render->Shader(this);
}
