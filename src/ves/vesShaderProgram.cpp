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

#include "vesShader.h"

#include "Painter.h"

#include <vector>

std::string vesShaderProgram::preDefinedAttributeNames[vesShaderProgram::COUNT_ATTRIBUTE_INDEX] =
{
  "vertexPosition",
  "vertexNormal",
  "vertexTextureCoordinate",
  "vertexColor"
  "vertexScalar"
};


vesShaderProgram::vesShaderProgram()
{
  // \todo: Perform lazy binding.
  for (size_t i=0; i < COUNT_ATTRIBUTE_INDEX; ++i)
    this->addBindAttributeLocation(vesShaderProgram::preDefinedAttributeNames[i], i);
}


vesShaderProgram::~vesShaderProgram()
{
  for (std::list<vesShader*>::iterator it=this->m_shaders.begin();
       it!=this->m_shaders.end(); ++it) {
    delete *it;
    it = this->m_shaders.erase(it);
  }
}


void vesShaderProgram::use()
{
  glUseProgram(this->m_programHandle);
}


int vesShaderProgram::uniform(string value)
{
  return glGetUniformLocation(this->m_programHandle, value.c_str());
}


int vesShaderProgram::attribute(string value)
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


void vesShaderProgram::setUniformMatrix4x4f(string str, vesMatrix4x4f& mat)
{
  glUniformMatrix4fv(this->uniform(str), 1, GL_FALSE, mat.mData);
}


void vesShaderProgram::setUniformMatrix3x3f(string str, vesMatrix3x3f& mat)
{
  glUniformMatrix3fv(this->uniform(str), 1, GL_FALSE, mat.mData);
}


void vesShaderProgram::setUniformVector3f(string str, vesVector3f point)
{
  glUniform3fv(this->uniform(str), 1, point.mData);
}


void vesShaderProgram::setUniformVector2f(string str, vesVector2f point)
{
  glUniform2fv(this->uniform(str), 1, point.mData);
}


void vesShaderProgram::setUniformFloat(string str, float value)
{
  glUniform1f(this->uniform(str), value);
}


void vesShaderProgram::setUniformInt(string str, int value)
{
  glUniform1i(this->uniform(str), value);
}


void vesShaderProgram::enableVertexArray(unsigned int location)
{
  glEnableVertexAttribArray(location);
}


void vesShaderProgram::disableVertexArray(unsigned int location)
{
  glDisableVertexAttribArray(location);
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

  //  \todo: Implement this.
  //  shader->addProgramReference(this);
  //  this->Modified();

  return true;
}


bool vesShaderProgram::addBindAttributeLocation(const std::string &name, unsigned int location)
{
  this->m_attributes[name] = location;

  // \todo: Make it modified or dirty.
  //  this->Modified();
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

  for(;constItr != this->m_attributes.end(); ++constItr) {
    glBindAttribLocation(this->m_programHandle, constItr->second, constItr->first.c_str());
  }
}


void vesShaderProgram::cleanUp()
{
  this->deleteVertexAndFragment();
  this->deleteProgram();
}


void vesShaderProgram::deleteVertexAndFragment()
{
// \todo: FixMe.
//  if (this->VertexShader)
//  {
//    glDeleteShader(this->VertexShader);
//    this->VertexShader = 0;
//  }
//  if (this->FragmentShader)
//  {
//    glDeleteShader(this->FragmentShader);
//    this->FragmentShader = 0;
//  }
}


void vesShaderProgram::render(Painter *render)
{
  std::cout << "vesShaderProgram::render" << std::endl;

  // \todo: Check if it is in modified state.
  if(!this->m_programHandle)
  {
    this->m_programHandle = glCreateProgram();

    if(this->m_programHandle == 0)
    {
      std::cout << "ERROR: Cannot create Program Object" <<std::endl;
      return;
    }

    // Compile shaders.
    for (std::list<vesShader*>::iterator it=this->m_shaders.begin();
         it!=this->m_shaders.end(); ++it) {
      std::cout << "Compiling shaders: " << std::endl;

      (*it)->compileShader();

      (*it)->attachShader(this->m_programHandle);
    }

    this->bindAttributes();

    // link program
    if(!this->link()) {
      std::cout<< "ERROR: Failed to link Program" << std::endl;
      this->cleanUp();
    }
  }

  // bind uniforms
  // \todo: FixMe.
//  this->BindUniforms(this->Uniforms);

  // delete the vertex and fragment shaders (dont ask why? Cause I dont know either)
  this->deleteVertexAndFragment();

  render->setShaderProgram(this);
}
