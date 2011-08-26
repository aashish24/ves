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
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define VTK_STR_LIST_CNSTR(NAME)              \
vtkStringList  NAME (string value)            \
  {                                           \
  vtkStringList temp;                         \
  temp.List.push_back(value);                 \
  return temp;                                \
  }

VTK_STR_LIST_CNSTR(_att)
VTK_STR_LIST_CNSTR(_uni)

void vesShaderProgram::Use()
{
  glUseProgram(this->Program);
}

int vesShaderProgram::GetUniform(string value)
{
  return glGetUniformLocation(this->Program, value.c_str());
}

int vesShaderProgram::GetAttribute(string value)
{
  return glGetAttribLocation(this->Program, value.c_str());
}

void vesShaderProgram::DeleteProgram()
{
  if (this->Program)
  {
    glDeleteProgram(this->Program);
    this->Program = 0;
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

void vesShaderProgram::EnableVertexArray(string str)
{
  glEnableVertexAttribArray(this->GetAttribute(str));
}

void vesShaderProgram::DisableVertexArray(string str)
{
  glDisableVertexAttribArray(this->GetAttribute(str));
}

GLuint vesShaderProgram::CompileShader(GLenum type, const char* source)
{
  GLuint shader;
  GLint compiled;

  // Create shader
  shader = glCreateShader(type);
  if(shader ==0)
  {
    return 0;
  }

  // Load source
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled)
  {
    GLint infoLen =0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    if(infoLen > 1)
    {
      char *infoLog = (char*) malloc(sizeof(char)*infoLen);
      glGetShaderInfoLog(shader,infoLen,NULL,infoLog);
      std::cout << "Error compiling shader:"
                << std::endl
                << infoLog
                << std::endl;
      free(infoLog);
    }

    glDeleteShader(shader);
    return 0;
  }

  return shader;
}


bool vesShaderProgram::Link()
{
  GLint status;

  glLinkProgram(this->Program);

  GLint logLength;
  glGetProgramiv(this->Program, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0)
  {
    char *log = (char *)malloc(logLength);
    glGetProgramInfoLog(this->Program, logLength, &logLength, log);
    std::cerr  << "Program link log:" << std::endl << log << std::endl;
    free(log);
  }

  glGetProgramiv(this->Program, GL_LINK_STATUS, &status);
  if (status == 0)
    return false;

  return true;
}

bool vesShaderProgram::Validate()
{
  GLint logLength, status;

  glValidateProgram(this->Program);
  glGetProgramiv(this->Program, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0)
  {
    char *log = (char *)malloc(logLength);
    glGetProgramInfoLog(this->Program, logLength, &logLength, log);
    std::cerr << "Program validate log:" <<std::endl << log << std::endl;
    free(log);
  }

  glGetProgramiv(this->Program, GL_VALIDATE_STATUS, &status);
  if (status == 0)
    return false;

  return true;
}


vesShaderProgram::vesShaderProgram (char* vertexShaderStr,
                                    char* fragmentShaderStr,
                                    vtkStringList uniforms,
                                    vtkStringList attributes)
{
  this->Program = glCreateProgram();
  if(this->Program == 0)
  {
    std::cout << "ERROR: Cannot create Program Object" <<std::endl;
  }
  // compile and load vertex and framgent shader
  this->CompileAndLoadVertexShader(vertexShaderStr);
  this->CompileAndLoadFragmentShader(fragmentShaderStr);

  // bind attributes
  this->BindAttributes(attributes);

  // link program
  if(!this->Link())
  {
    std::cout<< "ERROR: Failed to link Program" << std::endl;
    this->Delete();
  }

  // bind uniforms
  this->BindUniforms(uniforms);

  // delete the vertex and fragment shaders (dont ask why? Cause I dont know either)
  this->DeleteVertexAndFragment();

}

vesShaderProgram::~vesShaderProgram()
{
}

void vesShaderProgram::CompileAndLoadVertexShader(char* vertexShaderStr)
{
  this->VertexShader =  CompileShader(GL_VERTEX_SHADER, vertexShaderStr);
  glAttachShader(this->Program,this->VertexShader);
}

void vesShaderProgram::CompileAndLoadFragmentShader(char* fragmentShaderStr)
{
  this->FragmentShader = CompileShader(GL_FRAGMENT_SHADER,fragmentShaderStr);
  glAttachShader(this->Program, this->FragmentShader);
}

void vesShaderProgram::BindAttributes(vtkStringList attribs)
{
  for(int i=0; i<attribs.List.size(); ++i)
  {
    this->Attributes[attribs.List[i]] = i;
    glBindAttribLocation(this->Program, i, attribs.List[i].c_str());
  }

}

void vesShaderProgram::BindUniforms(vtkStringList uniforms)
{
  for(int i=0 ; i<uniforms.List.size(); ++i)
  {
    this->Uniforms[uniforms.List[i]] = glGetUniformLocation(this->Program, uniforms.List[i].c_str());
  }
}

void vesShaderProgram::Delete()
{
  this->DeleteVertexAndFragment();
  this->DeleteProgram();
}

void vesShaderProgram::DeleteVertexAndFragment()
{
  if (this->VertexShader)
  {
    glDeleteShader(this->VertexShader);
    this->VertexShader = 0;
  }
  if (this->FragmentShader)
  {
    glDeleteShader(this->FragmentShader);
    this->FragmentShader = 0;
  }
}

vesShaderProgram _program(char* vertexShaderStr,
                          char* fragmentShaderStr,
                          vtkStringList uniforms,
                          vtkStringList attributes)
{
  vesShaderProgram prog(vertexShaderStr,
                        fragmentShaderStr,
                        uniforms,
                        attributes);

  return prog;
}

void vesShaderProgram::Render(Painter *render)
{
  render->ShaderProgram(this);
}
