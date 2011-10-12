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

#include "vesMapper.h"

// VES includes
#include "vesMaterial.h"
#include "vesGLTypes.h"
#include "vesRenderData.h"
#include "vesRenderStage.h"
#include "vesShaderProgram.h"
#include "vesTriangleData.h"
#include "vesVertexAttributeKeys.h"

#include "vesGL.h"

// C++ includes
#include <map>
#include <vector>
#include <cstdio>

class vesMapper::vesInternal
{
public:
  vesInternal()
  {
    this->m_color.resize(4);
  }


  ~vesInternal()
  {
    this->cleanUpDrawObjects();
  }


  void setColor(const float &r, const float &g, const float &b, const float &a)
  {
    m_color[0] = r; m_color[1] = g; m_color[2] = b; m_color[3] = a;
  }


  const float* color() const
  {
    return &(this->m_color.front());
  }


  void cleanUpDrawObjects()
  {
    this->m_bufferVertexAttributeMap.clear();
    this->m_buffers.clear();
  }

  std::vector< float >                       m_color;
  std::vector< unsigned int >                m_buffers;
  std::map< unsigned int, std::vector<int> > m_bufferVertexAttributeMap;
};



vesMapper::vesMapper() : vsgBoundedObject(),
  m_initialized(false),
  m_data       (0x0),
  m_maximumTrianglesPerDraw
               (65536 / 3),
  m_internal   (0x0)
{
  this->m_internal = new vesInternal();

  // Default is almost white.
  this->setColor(0.9, 0.9, 0.9, 1.0);
}


vesMapper::~vesMapper()
{
  if (this->m_initialized) {
    // For now we are not checking if the context is valid.
    this->deleteVertexBufferObjects();
  }

  delete this->m_internal; this->m_internal = 0x0;
}


void vesMapper::computeBounds()
{
  vesVector3f min = this->m_data->GetMin();
  vesVector3f max = this->m_data->GetMax();

  this->setBounds(min, max);

  this->setBoundsDirty(false);
}


void vesMapper::setData(vesTriangleData *data)
{
  if (data) {
    this->m_data = data;
    this->m_initialized = false;
  }
}


void vesMapper::normalize()
{
  float r = this->boundsRadius();

  this->m_normalizedMatrix =
      makeScaleMatrix4x4(1/r,1/r,1/r)*
      makeTranslationMatrix4x4(-this->boundsCenter());

  this->setBoundsCenter(transformPoint3f(this->m_normalizedMatrix, this->boundsCenter()));
  this->setBoundsSize(transformPoint3f(this->m_normalizedMatrix, this->boundsSize()));
}


void vesMapper::setColor(float r, float g, float b, float a)
{
  this->m_internal->setColor(r, g, b, a);
}


float* vesMapper::color()
{
  return const_cast<float*>(this->m_internal->color());
}


const float* vesMapper::color() const
{
  return this->m_internal->color();
}


void vesMapper::render(const vesRenderState &renderState)
{
  if (!this->m_initialized) {
    this->setupDrawObjects(renderState);
  }

  // Fixed vertex color.
  glVertexAttrib4fv(vesVertexAttributeKeys::Color, this->color());

  std::map<unsigned int, std::vector<int> >::const_iterator constItr
    = this->m_internal->m_bufferVertexAttributeMap.begin();

  int bufferIndex = 0;
  for (constItr; constItr != this->m_internal->m_bufferVertexAttributeMap.end();
       ++constItr) {
    glBindBuffer(GL_ARRAY_BUFFER, constItr->first);
    for (size_t i = 0; i < constItr->second.size(); ++i) {
      renderState.m_material->bindVertexData(renderState, constItr->second[i]);
    }
    ++bufferIndex;
  }

  const int numberOfTriangles = this->m_data->GetTriangles().size();
  if (numberOfTriangles > 0) {
    int drawnTriangles = 0;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_internal->m_buffers[bufferIndex++]);
    renderState.m_material->bindRenderData(
      renderState, vesRenderData(vesGLTypes::Triangles));
    while (drawnTriangles < numberOfTriangles) {
      int numberOfTrianglesToDraw = numberOfTriangles - drawnTriangles;
      if (numberOfTrianglesToDraw > this->m_maximumTrianglesPerDraw) {
        numberOfTrianglesToDraw = this->m_maximumTrianglesPerDraw;
      }
      glDrawElements(GL_TRIANGLES, numberOfTrianglesToDraw * 3,
                     GL_UNSIGNED_SHORT,
                     (void*)(drawnTriangles*sizeof(vesVector3us)));
      drawnTriangles += numberOfTrianglesToDraw;
    }
  }

  if (!this->m_data->GetLines().empty()) {
    renderState.m_material->bindRenderData(
      renderState, vesRenderData(vesGLTypes::Lines));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_internal->m_buffers[bufferIndex++]);
    glDrawElements(GL_LINES, this->m_data->GetLines().size() * 2,
                   GL_UNSIGNED_SHORT, (void*)0);
    }


  // Unbind.
  bufferIndex = 0;
  constItr = this->m_internal->m_bufferVertexAttributeMap.begin();
  for (; constItr != this->m_internal->m_bufferVertexAttributeMap.end();
       ++constItr) {
    for (size_t i = 0; i < constItr->second.size(); ++i) {
      renderState.m_material->unbindVertexData(renderState, constItr->second[i]);
    }
    ++bufferIndex;
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void vesMapper::setupDrawObjects(const vesRenderState &renderState)
{
  // Delete buffer objects from past if any.
  this->deleteVertexBufferObjects();

  // Now clean up any cache related to draw objects.
  this->m_internal->cleanUpDrawObjects();

  // Now construct the new ones.
  this->createVertexBufferObjects();

  this->m_initialized = true;
}


void vesMapper::createVertexBufferObjects()
{
  const int numberOfFloats = 6;
  size_t sizeOfData =
    this->m_data->GetPoints().size() * numberOfFloats * sizeof(float);

  unsigned int bufferId;

  if (!this->m_data->GetPoints().empty()) {
    glGenBuffers(1, &bufferId);
    this->m_internal->m_buffers.push_back(bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_internal->m_buffers.back());
    glBufferData(GL_ARRAY_BUFFER, sizeOfData,
                 &this->m_data->GetPoints()[0], GL_STATIC_DRAW);

    this->m_internal->m_bufferVertexAttributeMap[
        this->m_internal->m_buffers.back()].push_back(vesVertexAttributeKeys::Position);

    if (this->m_data->GetHasNormals()) {
      this->m_internal->m_bufferVertexAttributeMap[
        this->m_internal->m_buffers.back()].push_back(vesVertexAttributeKeys::Normal);
      }
    }

  if (!this->m_data->GetVertexColors().empty()) {
    glGenBuffers(1, &bufferId);
    this->m_internal->m_buffers.push_back(bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_internal->m_buffers.back());
    glBufferData(GL_ARRAY_BUFFER, this->m_data->GetVertexColors().size() * sizeof(float) * 3,
                 &this->m_data->GetVertexColors()[0], GL_STATIC_DRAW);

    this->m_internal->m_bufferVertexAttributeMap[
      this->m_internal->m_buffers.back()].push_back(vesVertexAttributeKeys::Color);
    }

  if (!this->m_data->GetTextureCoordinates().empty()) {
    glGenBuffers(1, &bufferId);
    this->m_internal->m_buffers.push_back(bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_internal->m_buffers.back());
    glBufferData(GL_ARRAY_BUFFER, this->m_data->GetTextureCoordinates().size() * sizeof(float) * 2,
                 &this->m_data->GetTextureCoordinates()[0], GL_STATIC_DRAW);

    this->m_internal->m_bufferVertexAttributeMap[
      this->m_internal->m_buffers.back()].push_back(vesVertexAttributeKeys::TextureCoordinate);
    }

  if (!this->m_data->GetTriangles().empty()) {
    glGenBuffers(1, &bufferId);
    this->m_internal->m_buffers.push_back(bufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_internal->m_buffers.back());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 this->m_data->GetTriangles().size() *sizeof(unsigned short) * 3,
                 &this->m_data->GetTriangles()[0], GL_STATIC_DRAW);
    }

  if (!this->m_data->GetLines().empty()) {
    glGenBuffers(1, &bufferId);
    this->m_internal->m_buffers.push_back(bufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_internal->m_buffers.back());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 this->m_data->GetLines().size() *sizeof(unsigned short) * 2,
                 &this->m_data->GetLines()[0], GL_STATIC_DRAW);
    }
}


void vesMapper::deleteVertexBufferObjects()
{
  if (!this->m_internal->m_buffers.empty()) {
    glDeleteBuffers(this->m_internal->m_buffers.size(),
                    &this->m_internal->m_buffers.front());
  }
}
