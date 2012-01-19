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
/// \brief Class defines a data structure that constitutes a geometry
///
/// vesGeometryData is modeled after collada (collada.org) and XDMF (xdmf.org).
/// Primarily vesGeometryData constitutes of sources and primitives. Sources define
/// the data in some coordinate space and primitives made references to the source data
/// in order to constitute a meaningful geometry primitive.
///
/// \see vesMapper vesActor vesSourceData vesPrimitive

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

  /// Get name / ID of the geometry data
  inline std::string name()
  {
    return this->m_name;
  }

  /// Get name / ID of the geometry data
  inline const std::string& name() const
  {
    return this->m_name;
  }

  /// Set name / ID of the geometry data
  inline bool setName(const std::string &name)
  {
    bool success = true;

    this->m_name = name;

    return success;
  }

  /// Add a new source to the geometry. Return true on success.
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

  /// Add a new primitive to the geometry. Return true on success.
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

  /// Return a primitive given a index. Return NULL on failure.
  inline vesSharedPtr<vesPrimitive> primitive(unsigned int index)
  {
    return this->m_primitives[index];
  }

  /// Return a primitive given a index. Return NULL on failure.
  inline const vesSharedPtr<vesPrimitive> primitive(unsigned int index) const
  {
    return this->m_primitives[index];
  }

  /// Return a source given a index. Return NULL on failure.
  inline vesSharedPtr<vesSourceData> source(unsigned int index)
  {
    return this->m_sources[index];
  }

  /// Return a source given a index. Return NULL on failure.
  inline const vesSharedPtr<vesSourceData> source(unsigned int index) const
  {
    return this->m_sources[index];
  }

  /// Return number of primitive types contained at the moment in the geometry
  inline unsigned int numberOfPrimitiveTypes() const
  {
    return static_cast<unsigned int>(this->m_primitives.size());
  }

  /// Return number of sources contained at the moment in the geometry
  inline unsigned int numberOfSources() const
  {
    return static_cast<unsigned int>(this->m_sources.size());
  }

  /// Compute and return geometry min bounds
  inline vesVector3f boundsMin()
  {
    if (this->m_computeBounds) {
      this->computeBounds();
    }

    return this->m_boundsMin;
  }

  /// Compute and return geometry max bounds
  inline vesVector3f boundsMax()
  {
    if (this->m_computeBounds) {
      this->computeBounds();
    }

    return this->m_boundsMax;
  }

  /// Compute geometry bounds
  void computeBounds();

  /// Compute normals (per vertex) if possible
  void computeNormals();

  /// Return primitive of type triangles. Return NULL on failure.
  inline vesSharedPtr<vesPrimitive> triangles();

  /// Return primitive of type triangle strips. Return NULL on failure.
  inline vesSharedPtr<vesPrimitive> triangleStrips();

  /// Return primitive of type lines. Return NULL on failure.
  inline vesSharedPtr<vesPrimitive> lines();

  /// Return source data given a key. Return NULL on failure.
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
