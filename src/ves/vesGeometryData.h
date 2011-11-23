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

#ifndef VESGEOMETRYDATA_H
#define VESGEOMETRYDATA_H

// VES includes
#include "vesGL.h"
#include "vesGMTL.h"
#include "vesPrimitive.h"
#include "vesSetGet.h"
#include "vesSharedPtr.h"
#include "vesSourceData.h"
#include "vesVertexAttributeKeys.h"

// C/C++ includes
#include <iostream>
#include <map>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
///
/// Geometry data
///
class vesGeometryData
{
public:
  typedef std::vector<vesSourceData::Ptr> Sources;

  vesTypeMacro(vesGeometryData);

  vesGeometryData() :
    m_computeBounds(true),
    m_computeNormals(true)
  {
  }

  unsigned int numberOfPrimitiveTypes() const
  {
    return static_cast<unsigned int>(this->m_primitives.size());
  }

  inline vesVector3f boundsMin()
  {
    if (this->m_computeBounds) {
      this->computeBounds();
    }

    return this->m_boundsMin;
  }

  inline vesVector3f boundsMax()
  {
    if (this->m_computeBounds) {
      this->computeBounds();
    }

    return this->m_boundsMax;
  }

  void computeBounds();

  void computeNormals();

  inline vesSharedPtr<vesPrimitive> triangles();
  inline vesSharedPtr<vesPrimitive> triangleStrips();
  inline vesSharedPtr<vesPrimitive> lines();

  inline vesSharedPtr<vesSourceData> sourceData(int key);

  // The ID of the geometry element
  std::string m_name;

  std::vector<vesPrimitive::Ptr> m_primitives;

  Sources m_sources;

private:
  void addAndUpdateNormal(unsigned int index, float n1, float n2, float n3,
                          void *data, unsigned int stride, unsigned int offset,
                          unsigned int sizeOfDataType);

  bool m_computeBounds;
  bool m_computeNormals;

  vesVector3f m_boundsMin;
  vesVector3f m_boundsMax;
};

vesSharedPtr<vesPrimitive> vesGeometryData::triangles()
{
  for (size_t i=0; i < this->m_primitives.size(); ++i) {
    if (this->m_primitives[i]->primitiveType() == GL_TRIANGLES) {
      return this->m_primitives[i];
    }
  }

  return vesPrimitive::Ptr();
}

vesSharedPtr<vesPrimitive> vesGeometryData::triangleStrips()
{
  for (size_t i=0; i < this->m_primitives.size(); ++i) {
    if (this->m_primitives[i]->primitiveType() == GL_TRIANGLE_STRIP) {
      return this->m_primitives[i];
    }
  }

  return vesPrimitive::Ptr();
}

vesSharedPtr<vesPrimitive> vesGeometryData::lines()
{
  for (size_t i=0; i < this->m_primitives.size(); ++i) {
    if (this->m_primitives[i]->primitiveType() == GL_LINES) {
      return this->m_primitives[i];
    }
  }

  return vesPrimitive::Ptr();
}

vesSharedPtr<vesSourceData>  vesGeometryData::sourceData(int key)
{
  for (size_t i = 0; i < this->m_sources.size(); ++i) {
    if (this->m_sources[i]->hasKey(key)) {
      return this->m_sources[i];
    }
  }

  return vesSharedPtr<vesSourceData>();
}

#endif // VESGEOMETRYDATA_H
