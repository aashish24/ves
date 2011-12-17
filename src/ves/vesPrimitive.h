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

#ifndef VESPRIMITIVE_H
#define VESPRIMITIVE_H

// VES includes
#include "vesSetGet.h"

// C++ includes
#include <vector>

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

  bool setPrimitiveType(unsigned int type)
  {
    this->m_primitiveType = type;
    return true;
  }

  inline unsigned int indexCount() const
  {
    return this->m_indexCount;
  }

  bool setIndexCount(unsigned int count)
  {
    bool success = true;
    this->m_indexCount = count;
    return success;
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

  /// Use this method with caution
  std::vector<unsigned short>* indices()
  {
    return &this->m_indices;
  }

  const std::vector<unsigned short>* indices() const
  {
    return &this->m_indices;
  }

private:
  /// Size of indices data type
  int m_dataTypeSize;

  /// Number of indices used to draw elements
  int m_indexCount;

  /// Primitive type
  unsigned int m_primitiveType;

  /// Primitive indices
  Indices m_indices;
};

#endif // VESPRIMITIVE_H
