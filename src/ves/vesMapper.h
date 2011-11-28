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
#include "vesSetGet.h"

// C/C++ includes
#include <vector>

// Forward declarations
class vesGeometryData;
class vesPrimitive;
class vesVisitor;
class vesRenderState;

class vesMapper : public vesBoundingObject
{
public:
  vesTypeMacro(vesMapper);

  vesMapper();
  virtual ~vesMapper();

  virtual void computeBounds();

  bool setGeometryData(vesSharedPtr<vesGeometryData> geometryData);
  vesSharedPtr<vesGeometryData> geometryData();
  const vesSharedPtr<vesGeometryData> geometryData() const;

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
  void drawTriangles(const vesRenderState &renderState,
                     vesSharedPtr<vesPrimitive> triangles);

  bool m_initialized;

  const int m_maximumTriangleIndicesPerDraw;

  vesSharedPtr<vesGeometryData> m_geometryData;

  class vesInternal;
  vesInternal *m_internal;
};

#endif
