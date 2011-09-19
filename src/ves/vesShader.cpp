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

#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
vesShader::vesShader(ShaderType type) :
  Type(type)
{
}

//-----------------------------------------------------------------------------
vesShader::vesShader(ShaderType type, const std::string &source) :
  Type        (type),
  ShaderSource(source)
{
}

//-----------------------------------------------------------------------------
bool vesShader::SetShaderType(ShaderType type)
{
  if (this->Type == type)
  {
    return true;
  }

  if (this->Type != UNDEFINED)
  {
    std::cerr << "ERROR: Cannot change type of Shader" << std::endl;
    return false;
  }

  this->Type = type;
  return true;
}

//-----------------------------------------------------------------------------
bool vesShader::LoadShaderSourceFromFile(const std::string& fileName)
{
  // \todo: Implement this function.
  // this->Modified();

  return true;
}

//-----------------------------------------------------------------------------
void vesShader::CompileShader()
{
  GLint compiled;

  // Create shader
  this->ShaderHandle = glCreateShader(this->Type);

  // Load source
  const GLchar *source = reinterpret_cast<const GLchar*>(this->ShaderSource.c_str());
  glShaderSource(this->ShaderHandle, 1, &source, NULL);
  glCompileShader(this->ShaderHandle);

  glGetShaderiv(this->ShaderHandle, GL_COMPILE_STATUS, &compiled);
  if (!compiled)
  {
    GLint infoLen =0;
    glGetShaderiv(this->ShaderHandle, GL_INFO_LOG_LENGTH, &infoLen);
    if(infoLen > 1)
    {
      char *infoLog = (char*) malloc(sizeof(char)*infoLen);
      glGetShaderInfoLog(this->ShaderHandle,infoLen,NULL,infoLog);
      std::cout << "Error compiling shader:"
                << std::endl
                << infoLog
                << std::endl;
      free(infoLog);
    }

    glDeleteShader(this->ShaderHandle);
  }
}

//-----------------------------------------------------------------------------
void vesShader::AttachShader(GLuint program) const
{
   glAttachShader(program, this->ShaderHandle);
}
