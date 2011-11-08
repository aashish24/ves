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

#ifndef VESMAPPER_H
#define VESMAPPER_H

#include "vesBoundingObject.h"

// VES includes
#include "vesGMTL.h" // Needed for vesMatrix4x4f return.
#include "vesSetGet.h"

// Forward declarations
class vesVisitor;
class vesTriangleData;
class vesRenderState;

class vesMapper : public vesBoundingObject
{
public:
  vesTypeMacro(vesMapper);

  vesMapper();
  virtual ~vesMapper();

  virtual void computeBounds();

  void setData(vesSharedPtr<vesTriangleData> data);
  vesSharedPtr<vesTriangleData> data() { return this->m_data; }
  const vesSharedPtr<vesTriangleData> data() const { return this->m_data; }

  void setColor(float r, float g, float b, float a);
  float* color();
  const float* color() const;

  virtual void render(const vesRenderState &renderState);

private:
  virtual void setupDrawObjects(const vesRenderState &renderState);

  virtual void createVertexBufferObjects();
  virtual void deleteVertexBufferObjects();

  //\todo: Why do we need this?
  void normalize();
  vesMatrix4x4f m_normalizedMatrix;

protected:
  bool m_initialized;

  const int m_maximumTrianglesPerDraw;

  vesSharedPtr<vesTriangleData> m_data;

  class vesInternal;
  vesInternal *m_internal;
};

#endif
