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

#ifndef VESVERTEXATTRIBUTE_H
#define VESVERTEXATTRIBUTE_H

#include "vesMaterial.h"

// VES includes
#include "vesGeometryData.h"
#include "vesGL.h"
#include "vesMapper.h"
#include "vesRenderState.h"
#include "vesSetGet.h"
#include "vesShaderProgram.h"

// C++ includes
#include <cassert>
#include <string>

class vesVertexAttribute
{
public:
  vesTypeMacro(vesVertexAttribute);

  vesVertexAttribute(const std::string &name) :
    m_name(name)
  {
  }

  virtual ~vesVertexAttribute()
  {
  }

  const std::string& name() const { return this->m_name; }

  virtual void bind(const vesRenderState &renderState)
    { vesNotUsed(renderState); }
  virtual void unbind(const vesRenderState &renderState)
    { vesNotUsed(renderState); }
  virtual void setup(const vesRenderState &renderState)
    { vesNotUsed(renderState); }

  virtual void bindVertexData(const vesRenderState &renderState, int key)
    { vesNotUsed(renderState); vesNotUsed(key); }
  virtual void unbindVertexData(const vesRenderState &renderState, int key)
    { vesNotUsed(renderState); vesNotUsed(key); }
  virtual void setupVertexData(const vesRenderState &renderState, int key)
    { vesNotUsed(renderState); vesNotUsed(key); }

  virtual void update(const vesRenderState &renderState,
                      const vesShaderProgram &shaderProgram, int key)
    { vesNotUsed(renderState); vesNotUsed(shaderProgram); vesNotUsed(key); }

protected:
  std::string m_name;
};


class vesPositionVertexAttribute : public vesVertexAttribute
{
public:
  vesPositionVertexAttribute(const std::string &name="vertexPosition") :
    vesVertexAttribute(name)
  {
  }

  virtual void bindVertexData(const vesRenderState &renderState, int key)
  {
    vesNotUsed(key);
    assert(renderState.m_material && renderState.m_material->shaderProgram());

    vesGeometryData::Ptr geometryData = renderState.m_mapper->geometryData();
    assert(geometryData);

    vesSourceData::Ptr sourceData = geometryData->sourceData(key);
    assert(sourceData);

    glVertexAttribPointer(renderState.m_material->shaderProgram()->
                          attributeLocation(this->m_name),
                          sourceData->numberOfComponents(key),
                          sourceData->attributeDataType(key),
                          sourceData->isAttributeNormalized(key),
                          sourceData->attributeStride(key),
                          (void*)sourceData->attributeOffset(key));

    glEnableVertexAttribArray(renderState.m_material->shaderProgram()->
                              attributeLocation(this->m_name));
  }

  virtual void unbindVertexData(const vesRenderState &renderState, int key)
  {
    vesNotUsed(key);
    assert(renderState.m_material && renderState.m_material->shaderProgram());

    glDisableVertexAttribArray(renderState.m_material->shaderProgram()->
                               attributeLocation(this->m_name));
  }
};


class vesNormalVertexAttribute : public vesVertexAttribute
{
public:
  vesNormalVertexAttribute(const std::string &name="vertexNormal") :
    vesVertexAttribute(name)
  {
  }

  virtual void bindVertexData(const vesRenderState &renderState, int key)
  {
    vesNotUsed(key);
    assert(renderState.m_material && renderState.m_material->shaderProgram());

    vesGeometryData::Ptr geometryData = renderState.m_mapper->geometryData();
    assert(geometryData);

    vesSourceData::Ptr sourceData = geometryData->sourceData(key);
    assert(sourceData);

    glVertexAttribPointer(renderState.m_material->shaderProgram()->
                          attributeLocation(this->m_name),
                          sourceData->numberOfComponents(key),
                          sourceData->attributeDataType(key),
                          sourceData->isAttributeNormalized(key),
                          sourceData->attributeStride(key),
                          (void*)sourceData->attributeOffset(key));

    glEnableVertexAttribArray(renderState.m_material->shaderProgram()->
                              attributeLocation(this->m_name));
  }

  virtual void unbindVertexData(const vesRenderState &renderState, int key)
  {
    vesNotUsed(key);
    assert(renderState.m_material && renderState.m_material->shaderProgram());

    glDisableVertexAttribArray(renderState.m_material->shaderProgram()->
                               attributeLocation(this->m_name));
  }
};


class vesColorVertexAttribute : public vesVertexAttribute
{
public:
  vesColorVertexAttribute(const std::string &name="vertexColor") :
    vesVertexAttribute(name)
  {
  }

  virtual void bindVertexData(const vesRenderState &renderState, int key)
  {
    vesNotUsed(key);
    assert(renderState.m_material && renderState.m_material->shaderProgram());

    vesGeometryData::Ptr geometryData = renderState.m_mapper->geometryData();
    assert(geometryData);

    vesSourceData::Ptr sourceData = geometryData->sourceData(key);
    assert(sourceData);

    glVertexAttribPointer(renderState.m_material->shaderProgram()->
                          attributeLocation(this->m_name),
                          sourceData->numberOfComponents(key),
                          sourceData->attributeDataType(key),
                          sourceData->isAttributeNormalized(key),
                          sourceData->attributeStride(key),
                          (void*)sourceData->attributeOffset(key));

    glEnableVertexAttribArray(renderState.m_material->shaderProgram()->
                              attributeLocation(this->m_name));
  }

  virtual void unbindVertexData(const vesRenderState &renderState, int key)
  {
    vesNotUsed(key);
    assert(renderState.m_material && renderState.m_material->shaderProgram());

    glDisableVertexAttribArray(renderState.m_material->shaderProgram()->
                               attributeLocation(this->m_name));
  }
};



class vesTextureCoordinateVertexAttribute : public vesVertexAttribute
{
public:
  vesTextureCoordinateVertexAttribute(const std::string &name="vertexTextureCoordinate") :
    vesVertexAttribute(name)
  {
  }

  virtual void bindVertexData(const vesRenderState &renderState, int key)
  {
    vesNotUsed(key);
    assert(renderState.m_material && renderState.m_material->shaderProgram());

    vesGeometryData::Ptr geometryData = renderState.m_mapper->geometryData();
    assert(geometryData);

    vesSourceData::Ptr sourceData = geometryData->sourceData(key);
    assert(sourceData);

    glVertexAttribPointer(renderState.m_material->shaderProgram()->
                          attributeLocation(this->m_name),
                          sourceData->numberOfComponents(key),
                          sourceData->attributeDataType(key),
                          sourceData->isAttributeNormalized(key),
                          sourceData->attributeStride(key),
                          (void*)sourceData->attributeOffset(key));

    glEnableVertexAttribArray(renderState.m_material->shaderProgram()->
                              attributeLocation(this->m_name));
  }

  virtual void unbindVertexData(const vesRenderState &renderState, int key)
  {
    vesNotUsed(key);
    assert(renderState.m_material && renderState.m_material->shaderProgram());

    glDisableVertexAttribArray(renderState.m_material->shaderProgram()->
                               attributeLocation(this->m_name));
  }
};

#endif // VESVERTEXATTRIBUTE_H
