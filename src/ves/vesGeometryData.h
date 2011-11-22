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
#include "vesSharedPtr.h"
#include "vesSetGet.h"
#include "vesVertexAttributeKeys.h"

// C/C++ includes
#include <iostream>
#include <map>
#include <string>
#include <vector>

/// Predefined vertex data structures
struct vesVertexDataP3f
{
  vesVector3f m_position;
};

struct vesVertexDataN3f
{
  vesVector3f m_normal;
};

struct vesVertexDataC3f
{
  vesVector3f m_color;
};

struct vesVertexDataT3f
{
  vesVector3f m_textureCoordinate;
};

struct vesVertexDataP3N3f
{
  vesVector3f m_position;
  vesVector3f m_normal;
};

struct vesVertexDataP3N3T3f
{
  vesVector3f m_position;
  vesVector3f m_normal;
  vesVector3f m_texureCoordinates;
};

struct vesVertexDataP3N3C3f
{
  vesVector3f m_position;
  vesVector3f m_normal;
  vesVector3f m_color;
};

struct vesVertexDataP3N3TC3f
{
  vesVector3f m_position;
  vesVector3f m_normal;
  vesVector3f m_texureCoordinates;
  vesVector3f m_color;
};

struct vesVertexDataP3N3T3C3S3f
{
  vesVector3f m_position;
  vesVector3f m_normal;
  vesVector3f m_texureCoordinates;
  vesVector3f m_color;
  vesVector3f m_scalar;
};

/// Base class for source data
class vesSourceData
{
public:

  vesTypeMacro(vesSourceData);

  struct AttributeData
  {
    // Number of components per group
    unsigned int m_numberOfComponents;

    // Type of data type (GL_FLOAT etc)
    unsigned int m_dataType;

    // Size of data type
    unsigned int m_dataTypeSize;

    // Specifies whether fixed-point data values should be normalized
    // (true) or converted directly as fixed-point values (false)
    // when they are accessed.
    bool m_normalized;

    // Strides for each attribute.
    unsigned m_stride;

    // Offset
    unsigned int m_offset;
  };

  typedef int Key;
  typedef std::map<Key, AttributeData> AttributeMap;
  typedef AttributeMap::iterator AttributeIterator;
  typedef AttributeMap::const_iterator AttributeConstIterator;

  virtual void* data() = 0;
  virtual unsigned int sizeOfArray() const = 0;
  virtual unsigned int sizeInBytes() const = 0;

  virtual bool hasKey(int key) const = 0;
  virtual std::vector<int> keys() const = 0;

  virtual unsigned int numberOfAttributes() const = 0;

  virtual unsigned int numberOfComponents(int key) const = 0;
  virtual bool setNumberOfComponents(int key, unsigned int count) = 0;

  virtual bool isAttributeNormalized(int key) const = 0;
  virtual bool setIsAttributeNormalized(int key, bool flag) = 0;

  virtual unsigned int sizeOfAttributeDataType(int key) const = 0;
  virtual bool setSizeOfAttributeDataType(int key, unsigned int size) = 0;

  virtual unsigned int attributeDataType(int key) const = 0;
  virtual bool setAttributeDataType(int key, unsigned int type) = 0;

  virtual int attributeOffset(int key) const = 0;
  virtual bool setAttributeOffset(int key, int offset) = 0;

  virtual int attributeStride(int key) const = 0;
  virtual bool setAttributeStride(int key, int stride) = 0;
};

/// Generic implementation for the source data
template <typename T>
class vesGenericSourceData : public vesSourceData
{
public:
  vesTypeMacro(vesGenericSourceData);

  virtual ~vesGenericSourceData()
  {
  }

  inline std::vector<T> array()
  {
    return this->m_data;
  }

  inline const std::vector<T>& array() const
  {
    return this->m_data;
  }

  virtual void* data()
  {
    return &this->m_data.front();
  }

  virtual unsigned int sizeOfArray() const
  {
    return this->m_data.size();
  }

  virtual unsigned int sizeInBytes() const
  {
    unsigned int sizeInByes = 0;

    std::vector<int> keys = this->keys();
    std::vector<int>::const_iterator constItr = keys.begin();

    for (; constItr != keys.end(); ++constItr) {
      sizeInByes
        += this->numberOfComponents(*constItr) * this->sizeOfAttributeDataType(*constItr);
    }

    sizeInByes *= this->sizeOfArray();

    return sizeInByes;
  }

  virtual bool hasKey(int key) const
  {
    bool success = true;

    AttributeConstIterator constItr = this->m_attributeMap.find(key);

    if (constItr != this->m_attributeMap.end()) {
      return success;
    }

    return !success;
  }

  virtual std::vector<int> keys() const
  {
    std::vector<int> keys;

    AttributeConstIterator constItr = this->m_attributeMap.begin();

    for (; constItr != this->m_attributeMap.end(); ++constItr) {

      keys.push_back(constItr->first);
    }

    return keys;
  }

  virtual unsigned int numberOfAttributes() const
  {
    return static_cast<unsigned int>(this->m_attributeMap.size());
  }

  virtual unsigned int numberOfComponents(int key) const
  {
    AttributeConstIterator constItr = this->m_attributeMap.find(key);

    if (constItr != this->m_attributeMap.end()) {

      return constItr->second.m_numberOfComponents;
    }

    return 0;
  }

  virtual bool setNumberOfComponents(int key, unsigned int count)
  {
    bool success = true;

    this->m_attributeMap[key].m_numberOfComponents = count;

    return success;
  }

  virtual bool isAttributeNormalized(int key) const
  {
    AttributeConstIterator constItr
      = this->m_attributeMap.find(key);

    if (constItr != this->m_attributeMap.end()) {
      return constItr->second.m_normalized;
    }

    return 0;
  }

  virtual bool setIsAttributeNormalized(int key, bool flag)
  {
    bool success = true;

    this->m_attributeMap[key].m_normalized = flag;

    return success;
  }

  virtual unsigned int sizeOfAttributeDataType(int key) const
  {
    AttributeConstIterator constItr
      = this->m_attributeMap.find(key);

    if (constItr != this->m_attributeMap.end()) {
      return constItr->second.m_dataTypeSize;
    }

    return 0;
  }

  virtual bool setSizeOfAttributeDataType(int key, unsigned int size)
  {
    bool success = true;

    this->m_attributeMap[key].m_dataTypeSize = size;

    return success;
  }

  unsigned int attributeDataType(int key) const
  {
    AttributeConstIterator constItr
      = this->m_attributeMap.find(key);

    if (constItr != this->m_attributeMap.end()) {
      return constItr->second.m_dataType;
    }

    return 0;
  }

  virtual bool setAttributeDataType(int key, unsigned int type)
  {
    this->m_attributeMap[key].m_dataType = type;

    return true;
  }

  virtual int attributeOffset(int key) const
  {
    AttributeConstIterator constItr
      = this->m_attributeMap.find(key);

    if (constItr != this->m_attributeMap.end()) {
      return constItr->second.m_offset;
    }

    return -1;
  }

  virtual bool setAttributeOffset(int key, int offset)
  {
    this->m_attributeMap[key].m_offset = offset;

    return true;
  }

  virtual int attributeStride(int key) const
  {
    AttributeConstIterator constItr
      = this->m_attributeMap.find(key);

    if (constItr != this->m_attributeMap.end()) {
      return constItr->second.m_stride;
    }

    return -1;
  }

  virtual bool setAttributeStride(int key, int stride)
  {
    this->m_attributeMap[key].m_stride = stride;

    return true;
  }

  // Mesh data
  std::vector<T> m_data;

protected:
  AttributeMap m_attributeMap;
};

class vesSourceDataP3f : public vesGenericSourceData<vesVertexDataP3f>
{
public:
  vesTypeMacro(vesSourceDataP3f);

  vesSourceDataP3f() : vesGenericSourceData<vesVertexDataP3f>()
  {
    this->setAttributeDataType(vesVertexAttributeKeys::Position, GL_FLOAT);
    this->setAttributeOffset(vesVertexAttributeKeys::Position, 0);
    this->setAttributeStride(vesVertexAttributeKeys::Position, 12);
    this->setNumberOfComponents(vesVertexAttributeKeys::Position, 3);
    this->setSizeOfAttributeDataType(vesVertexAttributeKeys::Position, sizeof(float));
    this->setIsAttributeNormalized(vesVertexAttributeKeys::Position, false);
  }
};

class vesSourceDataN3f : public vesGenericSourceData<vesVertexDataN3f>
{
public:
  vesTypeMacro(vesSourceDataN3f);

  vesSourceDataN3f() : vesGenericSourceData<vesVertexDataN3f>()
  {
    this->setAttributeDataType(vesVertexAttributeKeys::Normal, GL_FLOAT);
    this->setAttributeOffset(vesVertexAttributeKeys::Normal, 0);
    this->setAttributeStride(vesVertexAttributeKeys::Normal, 12);
    this->setNumberOfComponents(vesVertexAttributeKeys::Normal, 3);
    this->setSizeOfAttributeDataType(vesVertexAttributeKeys::Normal, sizeof(float));
    this->setIsAttributeNormalized(vesVertexAttributeKeys::Normal, false);
  }
};

class vesSourceDataC3f : public vesGenericSourceData<vesVertexDataC3f>
{
public:
  vesTypeMacro(vesSourceDataC3f);

  vesSourceDataC3f() : vesGenericSourceData<vesVertexDataC3f>()
  {
    this->setAttributeDataType(vesVertexAttributeKeys::Color, GL_FLOAT);
    this->setAttributeOffset(vesVertexAttributeKeys::Color, 0);
    this->setAttributeStride(vesVertexAttributeKeys::Color, 12);
    this->setNumberOfComponents(vesVertexAttributeKeys::Color, 3);
    this->setSizeOfAttributeDataType(vesVertexAttributeKeys::Color, sizeof(float));
    this->setIsAttributeNormalized(vesVertexAttributeKeys::Color, false);
  }
};

class vesSourceDataT3f : public vesGenericSourceData<vesVertexDataT3f>
{
public:
  vesTypeMacro(vesSourceDataT3f);

  vesSourceDataT3f() : vesGenericSourceData<vesVertexDataT3f>()
  {
    this->setAttributeDataType(vesVertexAttributeKeys::TextureCoordinate, GL_FLOAT);
    this->setAttributeOffset(vesVertexAttributeKeys::TextureCoordinate, 0);
    this->setAttributeStride(vesVertexAttributeKeys::TextureCoordinate, 12);
    this->setNumberOfComponents(vesVertexAttributeKeys::TextureCoordinate, 3);
    this->setSizeOfAttributeDataType(vesVertexAttributeKeys::TextureCoordinate, sizeof(float));
    this->setIsAttributeNormalized(vesVertexAttributeKeys::TextureCoordinate, false);
  }
};

class vesSourceDataP3N3f : public vesGenericSourceData<vesVertexDataP3N3f>
{
public:
  vesTypeMacro(vesSourceDataP3N3f);

  vesSourceDataP3N3f() : vesGenericSourceData<vesVertexDataP3N3f>()
  {
    this->setAttributeDataType(vesVertexAttributeKeys::Position, GL_FLOAT);
    this->setAttributeDataType(vesVertexAttributeKeys::Normal, GL_FLOAT);
    this->setAttributeOffset(vesVertexAttributeKeys::Position, 0);
    this->setAttributeOffset(vesVertexAttributeKeys::Normal, 12);
    this->setAttributeStride(vesVertexAttributeKeys::Position, 24);
    this->setAttributeStride(vesVertexAttributeKeys::Normal, 24);
    this->setNumberOfComponents(vesVertexAttributeKeys::Position, 3);
    this->setNumberOfComponents(vesVertexAttributeKeys::Normal, 3);
    this->setSizeOfAttributeDataType(vesVertexAttributeKeys::Position, sizeof(float));
    this->setSizeOfAttributeDataType(vesVertexAttributeKeys::Normal, sizeof(float));
    this->setIsAttributeNormalized(vesVertexAttributeKeys::Position, false);
    this->setIsAttributeNormalized(vesVertexAttributeKeys::Normal, false);
  }
};

class vesSourceDataP3N3C3f : public vesGenericSourceData<vesVertexDataP3N3C3f>
{
public:
  vesTypeMacro(vesSourceDataP3N3C3f);

  vesSourceDataP3N3C3f() : vesGenericSourceData<vesVertexDataP3N3C3f>()
  {
    std::cout << "Initializing " << std::endl;
    this->setAttributeDataType(vesVertexAttributeKeys::Position, GL_FLOAT);
    this->setAttributeDataType(vesVertexAttributeKeys::Normal, GL_FLOAT);
    this->setAttributeDataType(vesVertexAttributeKeys::Color, GL_FLOAT);
    this->setAttributeOffset(vesVertexAttributeKeys::Position, 0);
    this->setAttributeOffset(vesVertexAttributeKeys::Normal, 12);
    this->setAttributeOffset(vesVertexAttributeKeys::Color, 24);
    this->setAttributeStride(vesVertexAttributeKeys::Position, 36);
    this->setAttributeStride(vesVertexAttributeKeys::Normal, 36);
    this->setAttributeStride(vesVertexAttributeKeys::Color, 36);
    this->setNumberOfComponents(vesVertexAttributeKeys::Position, 3);
    this->setNumberOfComponents(vesVertexAttributeKeys::Normal, 3);
    this->setNumberOfComponents(vesVertexAttributeKeys::Color, 3);
    this->setSizeOfAttributeDataType(vesVertexAttributeKeys::Position, sizeof(float));
    this->setSizeOfAttributeDataType(vesVertexAttributeKeys::Normal, sizeof(float));
    this->setSizeOfAttributeDataType(vesVertexAttributeKeys::Color, sizeof(float));
    this->setIsAttributeNormalized(vesVertexAttributeKeys::Position, false);
    this->setIsAttributeNormalized(vesVertexAttributeKeys::Normal, false);
    this->setIsAttributeNormalized(vesVertexAttributeKeys::Color, false);
  }
};

///////////////////////////////////////////////////////////////////////////////
///
/// Primitive
///
class vesPrimitive
{
public:
  typedef std::vector<unsigned short> Indices;

  vesTypeMacro(vesPrimitive);

  vesPrimitive() :
    m_dataTypeSize(sizeof(unsigned short)),
    m_indexCount(0),
    m_primitiveType(0)
  {
  }

  /// Helper functions
  inline void pushBackIndices(unsigned short i1)
  {
    this->m_indices.push_back(i1);
  }

  inline void pushBackIndices(unsigned short i1, unsigned short i2)
  {
    this->m_indices.push_back(i1);
    this->m_indices.push_back(i2);
  }

  inline void pushBackIndices(unsigned short i1, unsigned short i2, unsigned short i3)
  {
    this->m_indices.push_back(i1);
    this->m_indices.push_back(i2);
    this->m_indices.push_back(i3);
  }

  inline unsigned int size() const
  {
    return this->numberOfIndices();
  }

  inline unsigned int sizeInBytes() const
  {
    return this->m_dataTypeSize * this->m_indices.size();
  }

  inline unsigned int numberOfIndices() const
  {
    return static_cast<unsigned int>(this->m_indices.size());
  }

  inline unsigned int primitiveType() const
  {
    return this->m_primitiveType;
  }

  inline unsigned int indexCount() const
  {
    return this->m_indexCount;
  }

  inline int sizeOfDataType() const
  {
    return this->m_dataTypeSize;
  }

  /// Use this method with caution
  inline unsigned short* data()
  {
    return &this->m_indices.front();
  }

  inline const unsigned short* data() const
  {
    return &this->m_indices.front();
  }

  inline unsigned short at(unsigned int index)
  {
    return this->m_indices[index];
  }

  inline const unsigned short& at(unsigned int index) const
  {
    return this->m_indices[index];
  }

  // Size of indices data type
  int m_dataTypeSize;

  // Number of indices used to draw elements
  int m_indexCount;

  // Primitive type
  unsigned int m_primitiveType;

  // Primitive indices
  Indices m_indices;
};

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
    if (this->m_primitives[i]->m_primitiveType == GL_TRIANGLES) {
      return this->m_primitives[i];
    }
  }

  return vesPrimitive::Ptr();
}

vesSharedPtr<vesPrimitive> vesGeometryData::triangleStrips()
{
  for (size_t i=0; i < this->m_primitives.size(); ++i) {
    if (this->m_primitives[i]->m_primitiveType == GL_TRIANGLE_STRIP) {
      return this->m_primitives[i];
    }
  }

  return vesPrimitive::Ptr();
}

vesSharedPtr<vesPrimitive> vesGeometryData::lines()
{
  for (size_t i=0; i < this->m_primitives.size(); ++i) {
    if (this->m_primitives[i]->m_primitiveType == GL_LINES) {
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
