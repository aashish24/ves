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
/// \class vesGeometryData
/// \ingroup ves
/// \see vesMapper vesActor

#ifndef VESGEOMETRYDATA_H
#define VESGEOMETRYDATA_H

// VES includes
#include "vesGL.h"
#include "vesMath.h"
#include "vesPrimitive.h"
#include "vesSetGet.h"
#include "vesSharedPtr.h"
#include "vesSourceData.h"
#include "vesVertexAttributeKeys.h"

// C/C++ includes
#include <algorithm>
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

  inline std::string name()
  {
    return this->m_name;
  }

  inline const std::string& name() const
  {
    return this->m_name;
  }

  inline bool setName(const std::string &name)
  {
    bool success = true;

    this->m_name = name;

    return success;
  }

  inline bool addSource(vesSharedPtr<vesSourceData> source)
  {
    bool success = true;

    if (std::find(this->m_sources.begin(), this->m_sources.end(), source)
      == this->m_sources.end())
    {
      this->m_sources.push_back(source);
      return success;
    }

    return !success;
  }

  inline bool addPrimitive(vesSharedPtr<vesPrimitive> primitive)
  {
    bool success = true;

    if (std::find(this->m_primitives.begin(), this->m_primitives.end(),
      primitive) == this->m_primitives.end())
    {
      this->m_primitives.push_back(primitive);
      return success;
    }

    return !success;
  }

  inline vesSharedPtr<vesPrimitive> primitive(unsigned int index)
  {
    return this->m_primitives[index];
  }

  inline const vesSharedPtr<vesPrimitive> primitive(unsigned int index) const
  {
    return this->m_primitives[index];
  }

  inline vesSharedPtr<vesSourceData> source(unsigned int index)
  {
    return this->m_sources[index];
  }

  inline const vesSharedPtr<vesSourceData> source(unsigned int index) const
  {
    return this->m_sources[index];
  }

  inline unsigned int numberOfPrimitiveTypes() const
  {
    return static_cast<unsigned int>(this->m_primitives.size());
  }

  inline unsigned int numberOfSources() const
  {
    return static_cast<unsigned int>(this->m_sources.size());
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

private:
  void addAndUpdateNormal(unsigned int index, float n1, float n2, float n3,
                          void *data, unsigned int stride, unsigned int offset,
                          unsigned int sizeOfDataType);

  /// The ID of the geometry element
  std::string m_name;

  std::vector<vesPrimitive::Ptr> m_primitives;

  Sources m_sources;

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
