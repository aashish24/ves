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

#include "vesMaterial.h"

// VES includes
#include "vesRenderData.h"
#include "vesShaderProgram.h"
#include "vesTexture.h"

// C++ includes
#include <map>
#include <cstdio>

class vesMaterial::vesInternal
{
public:

  const vesMaterialAttribute*
    findAttribute(vesMaterialAttribute::AttributeType type) const;


  typedef std::map<vesMaterialAttribute::AttributeType,
    vesMaterialAttribute*> Attributes;

  typedef std::map<unsigned int, vesTexture*> TextureAttributes;

  Attributes        m_attributes;
  TextureAttributes m_textureAttributes;
};


const vesMaterialAttribute* vesMaterial::vesInternal::findAttribute(
  vesMaterialAttribute::AttributeType type) const
{
  if (type == vesMaterialAttribute::Texture) {
    // Return first texture.
    TextureAttributes::const_iterator constItr = this->m_textureAttributes.begin();

    if (constItr != this->m_textureAttributes.end()) {
      return (constItr->second);
    }
    else {
      return 0x0;
    }
  }
  else {
    Attributes::const_iterator constItr = this->m_attributes.find(type);

    if (constItr != this->m_attributes.end()) {
      return (constItr->second);
    }

    return 0x0;
  }
}


vesMaterial::vesMaterial() :
  m_binNumber     (Default),
  m_shaderProgram (0x0)
{
  this->m_internal = new vesInternal;
}


vesMaterial::~vesMaterial()
{
  delete this->m_internal; this->m_internal = 0x0;
}


bool vesMaterial::addAttribute(vesMaterialAttribute *attribute)
{
  if (!attribute) {
    return false;
  }

  if (attribute->type() != vesMaterialAttribute::Texture) {
    vesInternal::Attributes::iterator itr =
      this->m_internal->m_attributes.find(attribute->type());

    if (itr == this->m_internal->m_attributes.end() ||
        ( (itr->second) != attribute )) {
      this->m_internal->m_attributes[attribute->type()] = attribute;

      if (attribute->type() == vesMaterialAttribute::Shader) {
        this->m_shaderProgram = static_cast<vesShaderProgram*>(attribute);
      }
      return true;
    }
  }
  else {
    vesTexture *texture = static_cast<vesTexture*>(attribute);

    // Cache last texture so that we can release graphics resources on it.
    this->m_internal->m_textureAttributes[texture->textureUnit()] = texture;
  }

  return false;
}


bool vesMaterial::setShaderProgram(vesShaderProgram *shaderProgram)
{
  if (!shaderProgram || shaderProgram == this->m_shaderProgram) {
    return false;
  }

  this->m_shaderProgram = shaderProgram;
  this->m_internal->m_attributes[shaderProgram->type()] =
    this->m_shaderProgram;

  return true;
}


vesMaterialAttribute* vesMaterial::attribute(
  vesMaterialAttribute::AttributeType type)
{
  return const_cast<vesMaterialAttribute*>(this->m_internal->findAttribute(type));
}


const vesMaterialAttribute*
  vesMaterial::attribute(vesMaterialAttribute::AttributeType type) const
{
  return (this->m_internal->findAttribute(type));
}



void vesMaterial::render(const vesRenderState &renderState)
{
  this->bind(renderState);
}


void vesMaterial::remove(const vesRenderState &renderState)
{
  // \todo: Implement this.
}


void vesMaterial::setup(const vesRenderState &renderState)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->setup(renderState);
  }

  vesInternal::TextureAttributes::iterator textureItr =
    this->m_internal->m_textureAttributes.begin();

  for (; textureItr != this->m_internal->m_textureAttributes.end();
       ++textureItr) {
    textureItr->second->setup(renderState);
  }
}


void vesMaterial::bind(const vesRenderState &renderState)
{
  this->setup(renderState);

  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->bind(renderState);
  }

  vesInternal::TextureAttributes::iterator textureItr =
    this->m_internal->m_textureAttributes.begin();

  for (; textureItr != this->m_internal->m_textureAttributes.end();
       ++textureItr) {
    textureItr->second->bind(renderState);
  }
}


void vesMaterial::unbind(const vesRenderState &renderState)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->unbind(renderState);
  }

  vesInternal::TextureAttributes::iterator textureItr =
    this->m_internal->m_textureAttributes.begin();

  for (; textureItr != this->m_internal->m_textureAttributes.end();
       ++textureItr) {
    textureItr->second->unbind(renderState);
  }
}


void vesMaterial::setupVertexData(const vesRenderState &renderState, int key)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->setupVertexData(renderState, key);
  }
}


void vesMaterial::bindVertexData(const vesRenderState &renderState, int key)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->bindVertexData(renderState, key);
  }
}


void vesMaterial::unbindVertexData(const vesRenderState &renderState, int key)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->unbindVertexData(renderState, key);
  }
}


void vesMaterial::bindRenderData(const vesRenderState &renderState,
                                 const vesRenderData  &renderData)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->bindRenderData(renderState, renderData);
  }
}
