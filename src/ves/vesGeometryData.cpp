
#include "vesGeometryData.h"

#include <cassert>

void vesGeometryData::computeBounds()
{
  if (!this->m_computeBounds)
  {
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

void vesGeometryData::computeNormals()
{
  if (!this->m_computeNormals)
  {
    return;
  }

  vesPrimitive::Ptr triangles = this->triangles();
  if (!triangles) {
    // \todo Put a log message here
    return;
  }

  vesSourceData::Ptr sourceData
    = this->sourceData(vesVertexAttributeKeys::Normal);
  if (!sourceData) {
    return;
  }

  void* data = sourceData->data();

  unsigned int count = sourceData->sizeOfArray();
  unsigned int sizeOfDataType
    = sourceData->sizeOfAttributeDataType(vesVertexAttributeKeys::Normal);
  unsigned int numberOfComponents
    = sourceData->numberOfComponents(vesVertexAttributeKeys::Normal);
  unsigned int stride
    = sourceData->attributeStride(vesVertexAttributeKeys::Normal);
  unsigned int offset
    = sourceData->attributeOffset(vesVertexAttributeKeys::Normal);

  assert(numberOfComponents <= 3);

  for (unsigned int i = 0; i < count; ++i) {
    void* v = static_cast<char*>(data) + i * stride + offset;

    for (unsigned int j = 0; j < numberOfComponents; ++j) {
      float &value = *(static_cast<float*>(v));
      value = 0.0f;
      v = static_cast<char*>(v) + sizeOfDataType;
    }
  }

  unsigned int numberOfIndices = triangles->numberOfIndices();
  unsigned int indexCount = triangles->indexCount();
  vesSharedPtr< vesIndices< unsigned short > > triangleIndices
      = std::tr1::static_pointer_cast< vesIndices< unsigned short > >
        (triangles->getVesIndices());

  assert(indexCount == 3);

  for (unsigned int i = 0; i < numberOfIndices; i=i+3) {

    void* p1 = static_cast<char*>(data) + triangleIndices->at(i+0) * stride;
    void* p2 = static_cast<char*>(data) + triangleIndices->at(i+1) * stride;
    void* p3 = static_cast<char*>(data) + triangleIndices->at(i+2) * stride;

    vesVector3f p1Vec3f;
    vesVector3f p2Vec3f;
    vesVector3f p3Vec3f;

    for (unsigned int j = 0; j < numberOfComponents; ++j) {
      float &valueP1 = *(static_cast<float*>(p1));
      float &valueP2 = *(static_cast<float*>(p2));
      float &valueP3 = *(static_cast<float*>(p3));

      p1Vec3f[j] = valueP1;
      p2Vec3f[j] = valueP2;
      p3Vec3f[j] = valueP3;

      p1 = static_cast<char*>(p1) + sizeOfDataType;
      p2 = static_cast<char*>(p2) + sizeOfDataType;
      p3 = static_cast<char*>(p3) + sizeOfDataType;
    }

    vesVector3f u = p2Vec3f - p1Vec3f;
    vesVector3f v = p3Vec3f - p1Vec3f;
    vesVector3f n;
    n[0] = u[1]*v[2] - u[2]*v[1];
    n[1] = u[2]*v[0] - u[0]*v[2];
    n[2] = u[0]*v[1] - u[1]*v[0];

    this->addAndUpdateNormal(triangleIndices->at(i+0), n[0], n[1], n[2],
                            data, stride, offset, sizeOfDataType);
    this->addAndUpdateNormal(triangleIndices->at(i+1), n[0], n[1], n[2],
                            data, stride, offset, sizeOfDataType);
    this->addAndUpdateNormal(triangleIndices->at(i+2), n[0], n[1], n[2],
                            data, stride, offset, sizeOfDataType);
  }

  for (unsigned int i = 0; i < count; ++i) {
    void* v1 = static_cast<char*>(data) + i * stride + offset;
    void* v2 = static_cast<char*>(v1) + sizeOfDataType;
    void* v3 = static_cast<char*>(v2) + sizeOfDataType;

    float &value1 = *(static_cast<float*>(v1));
    float &value2 = *(static_cast<float*>(v2));
    float &value3 = *(static_cast<float*>(v3));

    float length = value1 * value1 + value2 * value2 + value3 * value3;
    if (length > 0)
    {
      value1 /= sqrt(length);
      value2 /= sqrt(length);
      value3 /= sqrt(length);
    }
    else
    {
      value1 = 0;
      value2 = 0;
      value3 = 1;
    }
  }

  this->m_computeNormals = false;
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
