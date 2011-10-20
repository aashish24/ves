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

#include "vesTexture.h"
#include "vesShaderProgram.h"
#include "vesUniform.h"
#include "vesRenderState.h"

#include "vesGL.h"

#include <cassert>
#include <cstdio>


vesTexture::vesTexture() : vesMaterialAttribute(),
  m_hasImage(false),
  m_width(0),
  m_height(0),
  m_depth(0),
  m_textureHandle(0),
  m_textureUnit(0),
  m_pixelFormat(vesColorDataType::None),
  m_internalFormat()
{
  this->m_type    = vesMaterialAttribute::Texture;
  this->m_binding = vesMaterialAttribute::BindMinimal;
}


vesTexture::~vesTexture()
{
  glDeleteTextures(1, &this->m_textureHandle);
}


void vesTexture::bind(const vesRenderState &renderState)
{
  glActiveTexture(GL_TEXTURE0 + this->m_textureUnit);
  glBindTexture(GL_TEXTURE_2D, this->m_textureHandle);
}


void vesTexture::unbind(const vesRenderState &renderState)
{
  glBindTexture(GL_TEXTURE_2D, 0);
}


void vesTexture::setImage(vesImage image)
{
  if (image.m_data) {
    this->m_hasImage = true;
    this->m_image = image;

    this->updateDimensions();

    this->setDirtyStateOn();
  }
}


vesImage vesTexture::image() const
{
  return this->m_image;
}


void vesTexture::setTextureUnit(unsigned int unit)
{
  this->m_textureUnit = unit;
  this->setDirtyStateOn();
}


bool vesTexture::setWidth(int width)
{
  bool success = true;

  if (this->m_hasImage) {
    return !success;
  }

  this->m_width = width;
  return success;
}


int vesTexture::width() const
{
  bool success = true;

  this->m_width;
}


bool vesTexture::setHeight(int height)
{
  bool success = true;

  if (this->m_hasImage) {
    return !success;
  }

  this->m_height = height;
  return success;
}


int vesTexture::height() const
{
  return this->m_height;
}


bool vesTexture::setDepth(int depth)
{
  bool success = true;

  if (this->m_hasImage) {
    return !success;
  }

  this->m_depth = depth;
  return success;
}


int vesTexture::depth() const
{
  return this->m_depth;
}


bool vesTexture::setPixelFormat(vesColorDataType::PixelFormat pixelFormat)
{
  bool success = true;

  if (this->m_hasImage) {
    return !success;
  }

  this->m_pixelFormat = pixelFormat;
}


vesColorDataType::PixelFormat vesTexture::pixelFormat() const
{
  if (this->m_hasImage) {
    return this->m_image.m_pixelFormat;
  }

  return this->m_pixelFormat;
}


bool vesTexture::setInternalFormat(int internalFormat)
{
  bool success = true;

  this->m_internalFormat = internalFormat;

  return success;
}


int vesTexture::internalFormat() const
{
  return this->m_internalFormat;
}


void vesTexture::setup(const vesRenderState &renderState)
{
  if (this->dirtyState()) {
    glGenTextures(1, &this->m_textureHandle);
    glBindTexture(GL_TEXTURE_2D, this->m_textureHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (this->m_hasImage) {
      this->updateDimensions();
      this->computeInternalFormatUsingImage();

      glTexImage2D(GL_TEXTURE_2D, 0, this->m_internalFormat, this->m_width, this->m_height, 0,
                   this->m_pixelFormat ? this->m_pixelFormat : this->m_internalFormat,
                   GL_UNSIGNED_BYTE, this->m_image.m_data);
    }
    else {
      glTexImage2D(GL_TEXTURE_2D, 0, this->m_internalFormat, this->m_width,
                   this->m_height, 0,
                   this->m_pixelFormat ? this->m_pixelFormat : this->m_internalFormat,
                   GL_UNSIGNED_SHORT_5_6_5, NULL);
    }

    this->setDirtyStateOff();
  }
}


void vesTexture::computeInternalFormatUsingImage()
{
  // Currently image does not define internal format
  // and hence it's pixel format is the only way to query
  // information on how color has been stored.
  switch (this->m_image.m_pixelFormat) {
  case vesColorDataType::RGB:
    this->m_internalFormat = vesColorDataType::RGB;
    break;
  case vesColorDataType::RGBA:
    this->m_internalFormat = vesColorDataType::RGBA;
    break;
  case vesColorDataType::Luminance:
    this->m_internalFormat = vesColorDataType::Luminance;
    break;
  case vesColorDataType::LuminanceAlpha:
    this->m_internalFormat = vesColorDataType::LuminanceAlpha;
    break;
  // Do nothing when image pixel format is none or undefined.
  case vesColorDataType::None:
  default:
    break;
  };
}


void vesTexture::updateDimensions()
{
  if (this->m_hasImage) {
    this->m_width = this->m_image.m_width;
    this->m_height = this->m_image.m_height;
    this->m_depth  = this->m_image.m_depth;
  }
}


#if 0
void vesTexture::Render()
{
  if(!loaded)
  {
    this->load();
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_textureHandle);

  // Set uniforms
  vesMatrix4x4f orthoProjection = vesOrtho(-1,1,-1,1,-1,0.0001);

  vesUniform *mvpUniform = this->ShaderProgram->uniform("modelViewProjectionMatrix");
  assert(mvpUniform);
  mvpUniform->set(orthoProjection);

  this->ShaderProgram->updateUniforms();

  // Assign data
  glVertexAttribPointer(vesShaderProgram::Position,
                        2,
                        GL_FLOAT,
                        0,
                        0,
                        squareVertices);
  glEnableVertexAttribArray(vesShaderProgram::Position);

  glVertexAttribPointer(vesShaderProgram::TextureCoordinate,
                        2,
                        GL_FLOAT,
                        0,
                        0,
                        textureVertices);
  glEnableVertexAttribArray(vesShaderProgram::TextureCoordinate);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Draw arrays
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Disable vertex attributes
  glDisableVertexAttribArray(vesShaderProgram::Position);
  glDisableVertexAttribArray(vesShaderProgram::TextureCoordinate);

  glDisable(GL_BLEND);
}
#endif
