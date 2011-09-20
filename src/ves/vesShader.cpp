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

vesShader::vesShader(ShaderType type) :
  m_type(type)
{
}


vesShader::vesShader(ShaderType type, const std::string &source) :
  m_type        (type),
  m_shaderSource(source)
{
}


bool vesShader::setShaderType(ShaderType type)
{
  if (this->m_type == type)
    return true;

  if (this->m_type != Undefined) {
    std::cerr << "ERROR: Cannot change type of Shader" << std::endl;
    return false;
  }

  this->m_type = type;
  return true;
}


bool vesShader::loadShaderSourceFromFile(const std::string& fileName)
{
  // \todo: Implement this function.
  // this->Modified();

  return true;
}


void vesShader::compileShader()
{
  GLint compiled;

  // Create shader
  this->m_shaderHandle = glCreateShader(this->m_type);

  // Load source
  const GLchar *source = reinterpret_cast<const GLchar*>(this->m_shaderSource.c_str());
  glShaderSource(this->m_shaderHandle, 1, &source, NULL);
  glCompileShader(this->m_shaderHandle);

  glGetShaderiv(this->m_shaderHandle, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLint infoLen =0;
    glGetShaderiv(this->m_shaderHandle, GL_INFO_LOG_LENGTH, &infoLen);
    if(infoLen > 1)
    {
      char *infoLog = (char*) malloc(sizeof(char)*infoLen);
      glGetShaderInfoLog(this->m_shaderHandle,infoLen,NULL,infoLog);
      std::cout << "Error compiling shader:"
                << std::endl
                << infoLog
                << std::endl;
      free(infoLog);
    }

    glDeleteShader(this->m_shaderHandle);
  }
}


void vesShader::attachShader(GLuint program) const
{
   glAttachShader(program, this->m_shaderHandle);
}
