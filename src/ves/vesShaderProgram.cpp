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

#include "vesShaderProgram.h"

// VES includes
#include "vesShader.h"
#include "vesUniform.h"

#include "Painter.h"

// C++ includes
#include <vector>

std::string vesShaderProgram::preDefinedAttributeNames
  [vesShaderProgram::CountAttributeIndex] =
{
  "vertexPosition",
  "vertexNormal",
  "vertexTextureCoordinate",
  "vertexColor",
  "vertexScalar"
};


vesShaderProgram::vesShaderProgram()
{
  this->m_programHandle = 0;

  // \todo: Delay it further.
  for (size_t i=0; i < CountAttributeIndex; ++i)
    this->addBindAttributeLocation(vesShaderProgram::preDefinedAttributeNames[i], i);
}


vesShaderProgram::~vesShaderProgram()
{
  this->cleanUp();
}


bool vesShaderProgram::addShader(vesShader *shader)
{
  if(!shader)
    return false;

  // \todo: Memory management.
  for (std::list<vesShader*>::iterator it=this->m_shaders.begin();
       it!=this->m_shaders.end(); ++it) {

    if(shader == *it)
      return false;

    if((*it)->shaderType() == shader->shaderType()) {
      this->m_shaders.erase(it);
      break;
    }
  }

  this->m_shaders.push_back(shader);

  // \todo: Implement this and make its state dirty.
  // shader->addProgramReference(this);

  return true;
}


bool vesShaderProgram::addUniform(vesUniform *uniform)
{
  if (!uniform)
  {
    return false;
  }

  std::list<vesUniform*>::const_iterator constItr =
    this->m_uniforms.begin();

  for (constItr; constItr != this->m_uniforms.end(); ++constItr)
  {
    if (uniform == *constItr)
      return false;
  }

  this->m_uniforms.push_back(uniform);

  // \todo: Make it modified or dirty.
}


bool vesShaderProgram::addBindAttributeLocation(const std::string &name, unsigned int location)
{
  this->m_attributes[name] = location;

  // \todo: Make it modified or dirty.
}


void vesShaderProgram::use()
{
  glUseProgram(this->m_programHandle);
}


int vesShaderProgram::uniformLocation(string value)
{
  return glGetUniformLocation(this->m_programHandle, value.c_str());
}


int vesShaderProgram::attributeLocation(string value)
{
  return glGetAttribLocation(this->m_programHandle, value.c_str());
}


void vesShaderProgram::deleteProgram()
{
  if (this->m_programHandle) {
    glDeleteProgram(this->m_programHandle);
    this->m_programHandle = 0;
  }
}


bool vesShaderProgram::link()
{
  GLint status;

  glLinkProgram(this->m_programHandle);

  GLint logLength;
  glGetProgramiv(this->m_programHandle, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
    char *log = (char *)malloc(logLength);
    glGetProgramInfoLog(this->m_programHandle, logLength, &logLength, log);
    std::cerr  << "Program link log:" << std::endl << log << std::endl;
    free(log);
  }

  glGetProgramiv(this->m_programHandle, GL_LINK_STATUS, &status);
  if (status == 0)
    return false;

  return true;
}


bool vesShaderProgram::validate()
{
  GLint logLength, status;

  glValidateProgram(this->m_programHandle);
  glGetProgramiv(this->m_programHandle, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
    char *log = (char *)malloc(logLength);
    glGetProgramInfoLog(this->m_programHandle, logLength, &logLength, log);
    std::cerr << "Program validate log:" <<std::endl << log << std::endl;
    free(log);
  }

  glGetProgramiv(this->m_programHandle, GL_VALIDATE_STATUS, &status);
  if (status == 0)
    return false;

  return true;
}


void vesShaderProgram::bindAttributes()
{
  AttributeBindingMap::const_iterator constItr = this->m_attributes.begin();

  for (;constItr != this->m_attributes.end(); ++constItr) {
    glBindAttribLocation(this->m_programHandle, constItr->second, constItr->first.c_str());
  }
}

void vesShaderProgram::bindUniforms()
{
  std::list<vesUniform*>::const_iterator constItr = this->m_uniforms.begin();
  for (;constItr != this->m_uniforms.end(); ++constItr)
  {
    (*constItr)->bind(this);
  }
}


void vesShaderProgram::cleanUp()
{
  for (std::list<vesShader*>::iterator it=this->m_shaders.begin();
       it!=this->m_shaders.end(); ++it) {
    delete (*it);
  }

  this->deleteVertexAndFragment();
  this->deleteProgram();
}


void vesShaderProgram::deleteVertexAndFragment()
{
  // Delete a shader object.
  for (std::list<vesShader*>::iterator it=this->m_shaders.begin();
       it!=this->m_shaders.end(); ++it) {
    glDeleteShader( (*it)->shaderHandle() );
  }
}


vesUniform* vesShaderProgram::uniform(const std::string &name)
{
  std::list<vesUniform*>::iterator itr =
    this->m_uniforms.begin();

  for (; itr != this->m_uniforms.end(); ++itr)
  {
    if ( ((*itr)->name().compare(name)) == 0 )
    {
      return *itr;
    }
  }

  return 0;
}


bool vesShaderProgram::uniformExist(const std::string &name)
{
  std::list<vesUniform*>::const_iterator constItr =
    this->m_uniforms.begin();

  for (; constItr != this->m_uniforms.end(); ++constItr)
  {
    if ( ((*constItr)->name().compare(name)) == 0 )
    {
      return true;
    }
  }

  return false;
}


void vesShaderProgram::updateUniforms()
{
  std::list<vesUniform*>::const_iterator constItr =
    this->m_uniforms.begin();

  for (constItr; constItr != this->m_uniforms.end(); ++constItr)
  {
    (*constItr)->callGL();
  }
}


void vesShaderProgram::render(Painter *render)
{
  // \todo: Check if it is in modified state.
  if(!this->m_programHandle)
  {
    this->m_programHandle = glCreateProgram();

    if(this->m_programHandle == 0)
    {
      std::cerr << "ERROR: Cannot create Program Object" <<std::endl;
      return;
    }

    // Compile shaders.
    for (std::list<vesShader*>::iterator it=this->m_shaders.begin();
         it!=this->m_shaders.end(); ++it) {
      std::cerr << "INFO: Compiling shaders: " << std::endl;

      (*it)->compileShader();

      (*it)->attachShader(this->m_programHandle);
    }

    this->bindAttributes();

    // link program
    if(!this->link()) {
      std::cerr << "ERROR: Failed to link Program" << std::endl;
      this->cleanUp();
    }

    this->bindUniforms();
  }

  render->setShaderProgram(this);
}
