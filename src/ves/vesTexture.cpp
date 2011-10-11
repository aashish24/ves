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


static const GLfloat squareVertices[] = {
  1.0f,  1.0f,
  1.0f, -1.0f,
  -1.0f,  1.0f,
  -1.0f, -1.0f,
};

static const GLfloat textureVertices[] = {
  1.0f, 1.0f,
  1.0f, 0.0f,
  0.0f,  1.0f,
  0.0f,  0.0f,
};

vesTexture::vesTexture()
{
}

vesTexture::~vesTexture()
{
}

void vesTexture::bind(const vesRenderState &renderState)
{
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, this->texID);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void vesTexture::unbind(const vesRenderState &renderState)
{
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
}

void vesTexture::setImageData(SFImage image)
{
  this->Image = image;
  this->setDirtyStateOn();
}

void vesTexture::setup(const vesRenderState &renderState)
{
  if (this->dirtyState()) {
    glGenTextures(1, &this->texID);
    glBindTexture(GL_TEXTURE_2D, this->texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 this->Image.width,
                 this->Image.height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 this->Image.data);

    this->setDirtyStateOff();
  }
}

void vesTexture::Render()
{
#if 0
  if(!loaded)
  {
    this->load();
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texID);

  // Set uniforms
  vesMatrix4x4f orthoProjection = vesOrtho(-1,1,-1,1,-1,1000);

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
#endif 
}
