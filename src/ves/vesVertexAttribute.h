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
#include <stdint.h>

class vesVertexAttribute : public vesMaterialAttribute
{
public:
  vesTypeMacro(vesVertexAttribute);

  vesVertexAttribute(int key, const std::string &name) : vesMaterialAttribute(),
    m_name(name)
  {
    this->m_key = key;
    this->m_type = vesMaterialAttribute::Undefined;
    this->m_binding = vesMaterialAttribute::BindAll;
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
  int m_key;
};


#include <iostream>
class vesGenericVertexAttribute : public vesVertexAttribute
{
public:
  vesTypeMacro(vesGenericVertexAttribute);

  vesGenericVertexAttribute(int key, const std::string &name="vertexGeneric") :
    vesVertexAttribute(key, name)
  {
  }

  virtual void bindVertexData(const vesRenderState &renderState, int key)
  {
    if (key != this->m_key) {
      return;
    }

    assert(renderState.m_material);

    vesGeometryData::Ptr geometryData = renderState.m_mapper->geometryData();
    assert(geometryData);

    vesSourceData::Ptr sourceData = geometryData->sourceData(key);
    assert(sourceData);

//    std::cerr << "sourceData->sizeOfAttributeDataType(key) is " << sourceData->sizeOfAttributeDataType(key) << std::endl;
//    std::cerr << "sourceData->attributeDataType(key) is " << sourceData->attributeDataType(key) << std::endl;
//    std::cerr << " sourceData->attributeStride(key) is " <<  sourceData->attributeStride(key) << std::endl;
//    std::cerr << "sourceData->attributeOffset is " << sourceData->attributeOffset(key) << std::endl;

    glVertexPointer(sourceData->sizeOfAttributeDataType(key),
                    sourceData->attributeDataType(key),
                    sourceData->attributeStride(key),
                    (void*)static_cast<intptr_t>(sourceData->attributeOffset(key)));

//    glVertexAttribPointer(renderState.m_material->shaderProgram()->
//                          attributeLocation(this->m_name),
//                          sourceData->numberOfComponents(key),
//                          sourceData->attributeDataType(key),
//                          sourceData->isAttributeNormalized(key),
//                          sourceData->attributeStride(key),
//                          (void*)static_cast<intptr_t>(sourceData->attributeOffset(key)));

//    glEnableVertexAttribArray(renderState.m_material->shaderProgram()->
//                              attributeLocation(this->m_name));

     glEnableClientState(GL_VERTEX_ARRAY);
  }

  virtual void unbindVertexData(const vesRenderState &renderState, int key)
  {
    if (key != this->m_key) {
      return;
    }

    assert(renderState.m_material);

//    glDisableVertexAttribArray(renderState.m_material->shaderProgram()->
//                               attributeLocation(this->m_name));

    glDisableClientState(GL_VERTEX_ARRAY);
  }
};


class vesPositionVertexAttribute : public vesGenericVertexAttribute
{
public:
  vesTypeMacro(vesPositionVertexAttribute);

  vesPositionVertexAttribute(int key=vesVertexAttributeKeys::Position,
                             const std::string &name="vertexPosition") :
    vesGenericVertexAttribute(key, name)
  {
    this->m_type = vesMaterialAttribute::VertexPositionAttribute;
  }
};


class vesNormalVertexAttribute : public vesGenericVertexAttribute
{
public:
  vesTypeMacro(vesNormalVertexAttribute);

  vesNormalVertexAttribute(int key=vesVertexAttributeKeys::Normal,
                           const std::string &name="vertexNormal") :
    vesGenericVertexAttribute(key, name)
  {
    this->m_type = vesMaterialAttribute::VertexNormalAttribute;
  }

  virtual void bindVertexData(const vesRenderState &renderState, int key)
  {
    if (key != this->m_key) {
      return;
    }

    assert(renderState.m_material);

    vesGeometryData::Ptr geometryData = renderState.m_mapper->geometryData();
    assert(geometryData);

    vesSourceData::Ptr sourceData = geometryData->sourceData(key);
    assert(sourceData);

    glNormalPointer(sourceData->attributeDataType(key),
                    sourceData->attributeStride(key),
                    (void*)static_cast<intptr_t>(sourceData->attributeOffset(key)));

//    glVertexAttribPointer(renderState.m_material->shaderProgram()->
//                          attributeLocation(this->m_name),
//                          sourceData->numberOfComponents(key),
//                          sourceData->attributeDataType(key),
//                          sourceData->isAttributeNormalized(key),
//                          sourceData->attributeStride(key),
//                          (void*)static_cast<intptr_t>(sourceData->attributeOffset(key)));

//    glEnableVertexAttribArray(renderState.m_material->shaderProgram()->
//                              attributeLocation(this->m_name));

     glEnableClientState(GL_NORMAL_ARRAY);
  }

  virtual void unbindVertexData(const vesRenderState &renderState, int key)
  {
    if (key != this->m_key) {
      return;
    }

    assert(renderState.m_material);

//    glDisableVertexAttribArray(renderState.m_material->shaderProgram()->
//                               attributeLocation(this->m_name));

    glDisableClientState(GL_NORMAL_ARRAY);
  }
};


class vesColorVertexAttribute : public vesGenericVertexAttribute
{
public:
  vesTypeMacro(vesColorVertexAttribute);

  vesColorVertexAttribute(int key=vesVertexAttributeKeys::Color,
                          const std::string &name="vertexColor") :
    vesGenericVertexAttribute(key, name)
  {
    this->m_type = vesMaterialAttribute::VertexColorAttribute;
  }

  virtual void bindVertexData(const vesRenderState &renderState, int key)
  {
    if (key != this->m_key) {
      return;
    }

    assert(renderState.m_material);

    vesGeometryData::Ptr geometryData = renderState.m_mapper->geometryData();
    assert(geometryData);

    vesSourceData::Ptr sourceData = geometryData->sourceData(key);
    assert(sourceData);

    glColorPointer(sourceData->sizeOfAttributeDataType(key),
                   sourceData->attributeDataType(key),
                   sourceData->attributeStride(key),
                   (void*)static_cast<intptr_t>(sourceData->attributeOffset(key)));

//    glVertexAttribPointer(renderState.m_material->shaderProgram()->
//                          attributeLocation(this->m_name),
//                          sourceData->numberOfComponents(key),
//                          sourceData->attributeDataType(key),
//                          sourceData->isAttributeNormalized(key),
//                          sourceData->attributeStride(key),
//                          (void*)static_cast<intptr_t>(sourceData->attributeOffset(key)));

//    glEnableVertexAttribArray(renderState.m_material->shaderProgram()->
//                              attributeLocation(this->m_name));

     glEnableClientState(GL_COLOR_ARRAY);
  }

  virtual void unbindVertexData(const vesRenderState &renderState, int key)
  {
    if (key != this->m_key) {
      return;
    }

    assert(renderState.m_material);

//    glDisableVertexAttribArray(renderState.m_material->shaderProgram()->
//                               attributeLocation(this->m_name));

    glDisableClientState(GL_COLOR_ARRAY);
  }
};


class vesTextureCoordinateVertexAttribute : public vesGenericVertexAttribute
{
public:
  vesTypeMacro(vesTextureCoordinateVertexAttribute);

  vesTextureCoordinateVertexAttribute(int key=vesVertexAttributeKeys::TextureCoordinate,
                                      const std::string &name="vertexTextureCoordinate") :
    vesGenericVertexAttribute(key, name)
  {
    this->m_type = vesMaterialAttribute::VertexTexCoordAttribute;
  }

  virtual void bindVertexData(const vesRenderState &renderState, int key)
  {
    if (key != this->m_key) {
      return;
    }

    assert(renderState.m_material);

    vesGeometryData::Ptr geometryData = renderState.m_mapper->geometryData();
    assert(geometryData);

    vesSourceData::Ptr sourceData = geometryData->sourceData(key);
    assert(sourceData);

    glTexCoordPointer(sourceData->sizeOfAttributeDataType(key),
                      sourceData->attributeDataType(key),
                      sourceData->attributeStride(key),
                      (void*)static_cast<intptr_t>(sourceData->attributeOffset(key)));

//    glVertexAttribPointer(renderState.m_material->shaderProgram()->
//                          attributeLocation(this->m_name),
//                          sourceData->numberOfComponents(key),
//                          sourceData->attributeDataType(key),
//                          sourceData->isAttributeNormalized(key),
//                          sourceData->attributeStride(key),
//                          (void*)static_cast<intptr_t>(sourceData->attributeOffset(key)));

//    glEnableVertexAttribArray(renderState.m_material->shaderProgram()->
//                              attributeLocation(this->m_name));

     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  virtual void unbindVertexData(const vesRenderState &renderState, int key)
  {
    if (key != this->m_key) {
      return;
    }

    assert(renderState.m_material);

//    glDisableVertexAttribArray(renderState.m_material->shaderProgram()->
//                               attributeLocation(this->m_name));

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }
};

#endif // VESVERTEXATTRIBUTE_H
