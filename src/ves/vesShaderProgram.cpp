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

#include "vesShaderProgram.h"

// VES includes
#include "vesBooleanUniform.h"
#include "vesEngineUniform.h"
#include "vesShader.h"
#include "vesUniform.h"
#include "vesVertexAttribute.h"

// C++ includes
#include <map>
#include <vector>
#include <iostream>

class vesShaderProgram::vesInternal
{
public:

    vesInternal()
    {
      this->m_programHandle = 0;
    }


    ~vesInternal()
    {
      this->m_shaders.clear();
      this->m_uniforms.clear();
      this->m_vertexAttributes.clear();
      this->m_uniformNameToLocation.clear();
      this->m_vertexAttributeNameToLocation.clear();
      this->m_engineUniforms.clear();
    }

  typedef std::map<std::string, int>          UniformNameToLocation;
  typedef std::map<std::string, int>          VertexAttributeNameToLocation;
  typedef std::map<std::string, unsigned int> AttributeBindingMap;

  unsigned int m_programHandle;

  std::vector< vesSharedPtr<vesShader> > m_shaders;
  std::vector< vesSharedPtr<vesUniform> > m_uniforms;
  std::map<int, vesSharedPtr<vesVertexAttribute> >  m_vertexAttributes;
  std::map<int, bool> m_enabledVertexAttributes;

  UniformNameToLocation m_uniformNameToLocation;
  VertexAttributeNameToLocation m_vertexAttributeNameToLocation;

  std::vector< vesSharedPtr<vesEngineUniform> > m_engineUniforms;
};


vesShaderProgram::vesShaderProgram() : vesMaterialAttribute()
{
  this->m_internal = new vesInternal();

  this->m_type    = vesMaterialAttribute::Shader;
  this->m_binding = vesMaterialAttribute::BindAll;

  vesSharedPtr<vesEngineUniform> vertexHasColors (new vesHasVertexColors());
  vesSharedPtr<vesEngineUniform> primitiveType (new vesPrimitiveType());

  this->m_internal->m_engineUniforms.push_back(vertexHasColors);
  this->m_internal->m_engineUniforms.push_back(primitiveType);

  for (size_t i=0; i < this->m_internal->m_engineUniforms.size(); ++i) {
    this->addUniform(this->m_internal->m_engineUniforms[i]->uniform());
  }
}


vesShaderProgram::~vesShaderProgram()
{
  this->cleanUp();
}


bool vesShaderProgram::addShader(vesSharedPtr<vesShader> shader)
{
  if (!shader)
    return false;

  std::vector< vesSharedPtr<vesShader> >::iterator itr
    = this->m_internal->m_shaders.begin();

  // \todo: Memory management.
  for (; itr != this->m_internal->m_shaders.end(); ++itr) {

    if (shader == *itr)
      return false;

    if ((*itr)->shaderType() == shader->shaderType()) {
      this->m_internal->m_shaders.erase(itr);
      break;
    }
  }

  this->m_internal->m_shaders.push_back(shader);

  // shader->addProgramReference(this);

  this->setDirtyStateOn();

  return true;
}


bool vesShaderProgram::addUniform(vesSharedPtr<vesUniform> uniform)
{
  if (!uniform) {
    return false;
  }

  std::vector< vesSharedPtr<vesUniform> >::const_iterator constItr =
    this->m_internal->m_uniforms.begin();

  for (; constItr != this->m_internal->m_uniforms.end(); ++constItr) {
    if (uniform == *constItr)
      return false;
  }

  this->m_internal->m_uniforms.push_back(uniform);

  this->setDirtyStateOn();

  return true;
}


bool vesShaderProgram::addVertexAttribute(
  vesSharedPtr<vesVertexAttribute> attribute, int key)
{
  if (!attribute)
  {
    return false;
  }

  this->m_internal->m_vertexAttributes[key] = attribute;
  this->m_internal->m_enabledVertexAttributes[key] = true;

  this->setDirtyStateOn();

  return true;
}


vesSharedPtr<vesVertexAttribute> vesShaderProgram::attribute(int key)
{
  if (this->m_internal->m_vertexAttributes.find(key)
      != this->m_internal->m_vertexAttributes.end()) {
    return this->m_internal->m_vertexAttributes[key];
  }

  return vesSharedPtr<vesVertexAttribute>();
}


bool vesShaderProgram::enableVertexAttribute(int key, bool value)
{
  if(this->m_internal->m_vertexAttributes.find(key)
     != this->m_internal->m_vertexAttributes.end()) {
    this->m_internal->m_enabledVertexAttributes[key] = value;
    return true;
  }

  return false;
}


bool vesShaderProgram::isEnabledVertexAttribute(int key) const
{
  if(this->m_internal->m_enabledVertexAttributes.find(key)
     != this->m_internal->m_enabledVertexAttributes.end()) {
    return this->m_internal->m_enabledVertexAttributes[key];
  }

  return false;
}


bool vesShaderProgram::addBindAttributeLocation(const std::string &name,
                                                int location)
{
  this->m_internal->m_vertexAttributeNameToLocation[name] = location;

  this->setDirtyStateOn();

  return true;
}


int vesShaderProgram::uniformLocation(const std::string &name) const
{
  vesInternal::UniformNameToLocation::const_iterator constItr =
    this->m_internal->m_uniformNameToLocation.find(name);

  if (constItr != this->m_internal->m_uniformNameToLocation.end()) {
    return constItr->second;
  }
  else {
    return -1;
  }
}


int vesShaderProgram::attributeLocation(const std::string &name) const
{
  vesInternal::VertexAttributeNameToLocation::const_iterator constItr =
    this->m_internal->m_vertexAttributeNameToLocation.find(name);

  if (constItr != this->m_internal->m_vertexAttributeNameToLocation.end()) {
    return constItr->second;
  }
  else {
    return -1;
  }
}


void vesShaderProgram::use()
{
  glUseProgram(this->m_internal->m_programHandle);
}


int vesShaderProgram::queryUniformLocation(const std::string &value)
{
  return glGetUniformLocation(this->m_internal->m_programHandle, value.c_str());
}


int vesShaderProgram::queryAttributeLocation(const std::string &value)
{
  return glGetAttribLocation(this->m_internal->m_programHandle, value.c_str());
}


void vesShaderProgram::deleteProgram()
{
  if (this->m_internal->m_programHandle) {
    glDeleteProgram(this->m_internal->m_programHandle);
    this->m_internal->m_programHandle = 0;
  }
}


bool vesShaderProgram::link()
{
  GLint status;

  glLinkProgram(this->m_internal->m_programHandle);

  GLint logLength;
  glGetProgramiv(this->m_internal->m_programHandle, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
    char *log = (char *)malloc(logLength);
    glGetProgramInfoLog(this->m_internal->m_programHandle, logLength, &logLength, log);
    std::cerr  << "Program link log:" << std::endl << log << std::endl;
    free(log);
  }

  glGetProgramiv(this->m_internal->m_programHandle, GL_LINK_STATUS, &status);
  if (status == 0)
    return false;

  return true;
}


bool vesShaderProgram::validate()
{
  GLint logLength, status;

  glValidateProgram(this->m_internal->m_programHandle);
  glGetProgramiv(this->m_internal->m_programHandle, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
    char *log = (char *)malloc(logLength);
    glGetProgramInfoLog(this->m_internal->m_programHandle, logLength, &logLength, log);
    std::cerr << "Program validate log:" <<std::endl << log << std::endl;
    free(log);
  }

  glGetProgramiv(this->m_internal->m_programHandle, GL_VALIDATE_STATUS, &status);
  if (status == 0)
    return false;

  return true;
}


void vesShaderProgram::bindAttributes()
{
  std::map<int, vesSharedPtr<vesVertexAttribute> >::const_iterator constItr =
    this->m_internal->m_vertexAttributes.begin();

  for (;constItr != this->m_internal->m_vertexAttributes.end(); ++constItr) {
    glBindAttribLocation(this->m_internal->m_programHandle, constItr->first,
                         constItr->second->name().c_str());
    this->m_internal->m_vertexAttributeNameToLocation[constItr->second->name()] =
      constItr->first;
  }
}


void vesShaderProgram::bindUniforms()
{
  std::vector< vesSharedPtr<vesUniform> >::const_iterator constItr =
    this->m_internal->m_uniforms.begin();

  for (;constItr != this->m_internal->m_uniforms.end(); ++constItr) {
    this->m_internal->m_uniformNameToLocation[(*constItr)->name()] =
      queryUniformLocation((*constItr)->name());
  }
}


void vesShaderProgram::cleanUp()
{
  this->deleteVertexAndFragment();

  this->deleteProgram();

  delete this->m_internal; this->m_internal = 0x0;
}


void vesShaderProgram::deleteVertexAndFragment()
{
  // Delete a shader object.
  std::vector< vesSharedPtr<vesShader> >::iterator itr
    = this->m_internal->m_shaders.begin();
  for (; itr != this->m_internal->m_shaders.end(); ++itr) {
    glDeleteShader((*itr)->shaderHandle());
  }
}


unsigned int vesShaderProgram::programHandle()
{
  return this->m_internal->m_programHandle;
}


const unsigned int& vesShaderProgram::programHandle() const
{
  return this->m_internal->m_programHandle;
}


vesSharedPtr<vesUniform> vesShaderProgram::uniform(const std::string &name)
{
  std::vector< vesSharedPtr<vesUniform> >::iterator itr =
    this->m_internal->m_uniforms.begin();

  for (; itr != this->m_internal->m_uniforms.end(); ++itr)
  {
    if (((*itr)->name().compare(name)) == 0)
    {
      return *itr;
    }
  }

  return vesSharedPtr<vesUniform>();
}


bool vesShaderProgram::uniformExist(const std::string &name)
{
  std::vector< vesSharedPtr<vesUniform> >::const_iterator constItr =
    this->m_internal->m_uniforms.begin();

  for (; constItr != this->m_internal->m_uniforms.end(); ++constItr) {
    if (((*constItr)->name().compare(name)) == 0) {
      return true;
    }
  }

  return false;
}


void vesShaderProgram::updateUniforms()
{
  std::vector< vesSharedPtr<vesUniform> >::const_iterator constItr =
    this->m_internal->m_uniforms.begin();

  for (; constItr != this->m_internal->m_uniforms.end(); ++constItr) {
    (*constItr)->callGL(this->m_internal->m_uniformNameToLocation[(*constItr)->name()]);
  }
}


void vesShaderProgram::setup(const vesRenderState &renderState)
{
  vesNotUsed(renderState);
}


void vesShaderProgram::bind(const vesRenderState &renderState)
{
  if (!this->m_internal->m_programHandle || this->dirtyState()) {

    this->m_internal->m_programHandle = glCreateProgram();

    if (this->m_internal->m_programHandle == 0)
    {
      std::cerr << "ERROR: Cannot create Program Object" <<std::endl;
      return;
    }

    // Compile shaders.
    std::vector< vesSharedPtr<vesShader> >::iterator itr
      = this->m_internal->m_shaders.begin();
    for (; itr != this->m_internal->m_shaders.end(); ++itr) {
      std::cerr << "INFO: Compiling shaders: " << std::endl;

      (*itr)->compileShader();

      (*itr)->attachShader(this->m_internal->m_programHandle);
    }

    this->bindAttributes();

    // link program
    if (!this->link()) {
      std::cerr << "ERROR: Failed to link Program" << std::endl;
      this->cleanUp();
    }

    this->use();

    this->bindUniforms();

    this->setDirtyStateOff();
  }
  else
  {
    this->use();
  }

  // Call update callback.
  std::vector< vesSharedPtr<vesUniform> >::const_iterator constItr =
    this->m_internal->m_uniforms.begin();

  for (; constItr != this->m_internal->m_uniforms.end(); ++constItr) {
    (*constItr)->update(renderState, *this);
  }

  // Now update values to GL.
  this->updateUniforms();
}


void vesShaderProgram::unbind(const vesRenderState &renderState)
{
  // \todo: Implement this.
  vesNotUsed(renderState);
}


void vesShaderProgram::setupVertexData(const vesRenderState &renderState, int key)
{
  std::map<int, vesSharedPtr<vesVertexAttribute> >::const_iterator constItr =
    this->m_internal->m_vertexAttributes.find(key);

  if (constItr != this->m_internal->m_vertexAttributes.end()) {
    this->m_internal->m_vertexAttributes[key]->setupVertexData(renderState, key);
  }
}


void vesShaderProgram::bindVertexData(const vesRenderState &renderState, int key)
{
  std::map<int, vesSharedPtr<vesVertexAttribute> >::const_iterator constItr =
    this->m_internal->m_vertexAttributes.find(key);

  if (constItr != this->m_internal->m_vertexAttributes.end()) {
    if (this->isEnabledVertexAttribute(key)) {
      this->m_internal->m_vertexAttributes[key]->bindVertexData(renderState, key);
    }
  }
}


void vesShaderProgram::unbindVertexData(const vesRenderState &renderState, int key)
{
  std::map<int, vesSharedPtr<vesVertexAttribute> >::const_iterator constItr =
    this->m_internal->m_vertexAttributes.find(key);

  if (constItr != this->m_internal->m_vertexAttributes.end()) {
    this->m_internal->m_vertexAttributes[key]->unbindVertexData(renderState, key);
  }
}


void vesShaderProgram::bindRenderData(const vesRenderState &renderState,
                                      const vesRenderData  &renderData)
{
  for (size_t i=0; i < this->m_internal->m_engineUniforms.size(); ++i) {
    this->m_internal->m_engineUniforms[i]->bindRenderData(
      renderState, renderData);
  }
}
