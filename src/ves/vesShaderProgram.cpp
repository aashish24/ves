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

std::string vesShaderProgram::PreDefinedAttributeNames[vesShaderProgram::COUNT_ATTRIBUTE_INDEX] =
{
  "vertexPosition",
  "vertexNormal",
  "vertexTextureCoordinate",
  "vertexColor"
  "vertexScalar"
};


vesShaderProgram::vesShaderProgram()
{
  for(size_t i=0; i < COUNT_ATTRIBUTE_INDEX; ++i)
  {
    this->AddBindAttributeLocation(vesShaderProgram::PreDefinedAttributeNames[i], i);
  }
}

vesShaderProgram::~vesShaderProgram()
{
}

void vesShaderProgram::Use()
{
  glUseProgram(this->ProgramHandle);
}

int vesShaderProgram::GetUniform(string value)
{
  return glGetUniformLocation(this->ProgramHandle, value.c_str());
}

int vesShaderProgram::GetAttribute(string value)
{
  return glGetAttribLocation(this->ProgramHandle, value.c_str());
}

void vesShaderProgram::DeleteProgram()
{
  if (this->ProgramHandle)
  {
    glDeleteProgram(this->ProgramHandle);
    this->ProgramHandle = 0;
  }
}

void vesShaderProgram::SetUniformMatrix4x4f(string str, vesMatrix4x4f& mat)
{
  glUniformMatrix4fv(this->GetUniform(str), 1, GL_FALSE, mat.mData);
}

void vesShaderProgram::SetUniformMatrix3x3f(string str, vesMatrix3x3f& mat)
{
  glUniformMatrix3fv(this->GetUniform(str), 1, GL_FALSE, mat.mData);
}

void vesShaderProgram::SetUniformVector3f(string str, vesVector3f point)
{
  glUniform3fv(this->GetUniform(str), 1, point.mData);
}

void vesShaderProgram::SetUniformVector2f(string str, vesVector2f point)
{
  glUniform2fv(this->GetUniform(str), 1, point.mData);
}

void vesShaderProgram::SetUniformFloat(string str, float value)
{
  glUniform1f(this->GetUniform(str), value);
}

void vesShaderProgram::SetUniformInt(string str, int value)
{
  glUniform1i(this->GetUniform(str), value);
}

void vesShaderProgram::EnableVertexArray(unsigned int location)
{
  glEnableVertexAttribArray(location);
}

void vesShaderProgram::DisableVertexArray(unsigned int location)
{
  glDisableVertexAttribArray(location);
}

bool vesShaderProgram::Link()
{
  GLint status;

  glLinkProgram(this->ProgramHandle);

  GLint logLength;
  glGetProgramiv(this->ProgramHandle, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0)
  {
    char *log = (char *)malloc(logLength);
    glGetProgramInfoLog(this->ProgramHandle, logLength, &logLength, log);
    std::cerr  << "Program link log:" << std::endl << log << std::endl;
    free(log);
  }

  glGetProgramiv(this->ProgramHandle, GL_LINK_STATUS, &status);
  if (status == 0)
    return false;

  return true;
}

bool vesShaderProgram::AddShader(vesShader *shader)
{
  if(!shader)
    {
    return false;
    }

  // \todo: Memory management.
  for (std::list<vesShader*>::iterator it=this->Shaders.begin();
       it!=this->Shaders.end(); ++it)
    {
    if(shader == *it)
      {
      return false;
      }

    if((*it)->GetShaderType() == shader->GetShaderType())
      {
      this->Shaders.remove(*it);
      break;
      }
    }

  this->Shaders.push_back(shader);

  //  \todo: Fix this.
  //  shader->AddProgramReference(this);
  //  this->Modified();

  return true;
}

bool vesShaderProgram::AddBindAttributeLocation(const std::string &name, unsigned int location)
{
  this->Attributes[name] = location;

  // \todo: Make it modified or dirty.
}


bool vesShaderProgram::Validate()
{
  GLint logLength, status;

  glValidateProgram(this->ProgramHandle);
  glGetProgramiv(this->ProgramHandle, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0)
  {
    char *log = (char *)malloc(logLength);
    glGetProgramInfoLog(this->ProgramHandle, logLength, &logLength, log);
    std::cerr << "Program validate log:" <<std::endl << log << std::endl;
    free(log);
  }

  glGetProgramiv(this->ProgramHandle, GL_VALIDATE_STATUS, &status);
  if (status == 0)
    return false;

  return true;
}

void vesShaderProgram::BindAttributes()
{
  AttributeBindingMap::const_iterator constItr = this->Attributes.begin();

  for(;constItr != this->Attributes.end(); ++constItr)
  {
    glBindAttribLocation(this->ProgramHandle, constItr->second, constItr->first.c_str());
  }
}


void vesShaderProgram::Delete()
{
  this->DeleteVertexAndFragment();
  this->DeleteProgram();
}

void vesShaderProgram::DeleteVertexAndFragment()
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
  if(!this->ProgramHandle)
  {
    this->ProgramHandle = glCreateProgram();

    if(this->ProgramHandle == 0)
    {
      std::cout << "ERROR: Cannot create Program Object" <<std::endl;
      return;
    }

    // Compile shaders.
    for (std::list<vesShader*>::iterator it=this->Shaders.begin();
         it!=this->Shaders.end(); ++it)
    {
      std::cout << "Compiling shaders: " << std::endl;

      (*it)->CompileShader();

      (*it)->AttachShader(this->ProgramHandle);
    }

    this->BindAttributes();

    // link program
    if(!this->Link())
    {
      std::cout<< "ERROR: Failed to link Program" << std::endl;
      this->Delete();
    }
  }

  std::cout << "ProgramHandle " << this->ProgramHandle << std::endl;

  // bind uniforms
  // \todo: FixMe.
//  this->BindUniforms(this->Uniforms);

  // delete the vertex and fragment shaders (dont ask why? Cause I dont know either)
  this->DeleteVertexAndFragment();

  render->setShaderProgram(this);
}
