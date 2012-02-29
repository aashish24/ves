
#include "vesGeometryData.h"

#include <cassert>

void vesGeometryData::computeBounds()
{
  if (!this->m_computeBounds) {
    return;
  }

  vesSourceData::Ptr sourceData
    = this->sourceData(vesVertexAttributeKeys::Position);
  if (!sourceData) {
    return;
  }

  void* data = sourceData->data();

  unsigned int count = sourceData->sizeOfArray();
  unsigned int sizeOfDataType
    = sourceData->sizeOfAttributeDataType(vesVertexAttributeKeys::Position);
  unsigned int numberOfComponents
    = sourceData->numberOfComponents(vesVertexAttributeKeys::Position);
  unsigned int stride
    = sourceData->attributeStride(vesVertexAttributeKeys::Position);

  assert(numberOfComponents <= 3);

  this->m_boundsMin[0] = this->m_boundsMin[1] = this->m_boundsMin[2] = 0.0f;
  this->m_boundsMax[0] = this->m_boundsMax[1] = this->m_boundsMax[2] = 0.0f;

  for (unsigned int i = 0; i < count; ++i) {
    void* v = static_cast<char*>(data) + i * stride;

    for (unsigned int j = 0; j < numberOfComponents; ++j) {
      float &value = *(static_cast<float*>(v));
      if (i == 0)
      {
        this->m_boundsMin[j] = this->m_boundsMax[j] = value;
      }
      else
      {
        if (value > this->m_boundsMax[j]) this->m_boundsMax[j] = value;
        if (value < this->m_boundsMin[j]) this->m_boundsMin[j] = value;
      }

      v = static_cast<char*>(v) + sizeOfDataType;
    }
  }

  this->m_computeBounds = false;
}


void vesGeometryData::addAndUpdateNormal(unsigned int index, float n1,
  float n2, float n3, void *data, unsigned int stride, unsigned int offset,
  unsigned int sizeOfDataType)
{
  void* v1 = static_cast<char*>(data) + index * stride + offset;
  void* v2 = static_cast<char*>(v1) + sizeOfDataType ;
  void* v3 = static_cast<char*>(v2) + sizeOfDataType;

  float &value1 = *(static_cast<float*>(v1));
  float &value2 = *(static_cast<float*>(v2));
  float &value3 = *(static_cast<float*>(v3));

  value1 += n1;
  value2 += n2;
  value3 += n3;
}
