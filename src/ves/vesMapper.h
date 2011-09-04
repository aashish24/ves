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

#ifndef __vesMapper_h
#define __vesMapper_h

#include "vsgGeometryNode.h"
#include "vsgBoundedObject.h"

#include "vesGMTL.h" // Needed for vesMatrix4x4f return.

class vesShaderProgram;
class vesTriangleData;

/**
 * \class vesMapper
 * \brief Provides API analagous to vtkMapper derived classes in VTK.
 *
 * The vesActor class defines an entity that contains various attributes for
 * rendering, along with geometry in the correct form for rendering on the GPU.
 */

class vesMapper : public vsgGeometryNode, public vsgBoundedObject
{
public:
  vesMapper();
  ~vesMapper();
  bool read();
  vesMatrix4x4f eval();
  void render(Painter* render);
  void render(vesShaderProgram *program);
  void setTriangleData(vesTriangleData* data);
  vesTriangleData* triangleData();
  vesTriangleData* data();
  void computeBounds();
  void setColor(float r, float g, float b, float a);

  float red() const { return m_red; }
  float green() const { return m_green; }
  float blue() const { return m_blue; }
  float alpha() const { return m_alpha; }

  void setDrawPoints(bool drawPoints) { m_drawPoints = drawPoints; }
  bool drawPoints() const { return m_drawPoints; }

private:
  void normalize();
  vesMatrix4x4f m_normalizedMatrix;

protected:
  float m_red, m_green, m_blue, m_alpha;
  bool m_isNew;
  vesTriangleData *m_data;
  bool m_initialized;
  bool m_drawPoints;

  // unsigned int mMapperVBO[2];
};

#endif
