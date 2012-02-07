/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.
  Copyright 2012 Willow Garage, Inc.

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

// C/C++ includes
#include <cstdlib>
#include <iostream>
#include <limits>

vesShader::vesShader(ShaderType type) :
  m_type          (type),
  m_shaderHandle  (std::numeric_limits<unsigned>::max())
{
}


vesShader::vesShader(ShaderType type, const std::string &source) :
  m_type        (type),
  m_shaderHandle(std::numeric_limits<unsigned>::max()),
  m_shaderSource(source)
{
}


vesShader::~vesShader()
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

  // \todo: Make state dirty.
  return true;
}


void vesShader::compileShader()
{
  GLint compiled;

  // Create shader
  this->m_shaderHandle = glCreateShader(this->m_type);

  // Load source
  const char *source = reinterpret_cast<const char*>(this->m_shaderSource.c_str());
  glShaderSource(this->m_shaderHandle, 1, &source, NULL);
  glCompileShader(this->m_shaderHandle);

  glGetShaderiv(this->m_shaderHandle, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLint infoLen =0;
    glGetShaderiv(this->m_shaderHandle, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen > 1)
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


void vesShader::attachShader(unsigned int program) const
{
   glAttachShader(program, this->m_shaderHandle);
}
