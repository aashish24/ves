/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.
  Copyright 2012 Willow Garage, Inc.

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

#include "vesBackground.h"

// VES includes.
#include "vesActor.h"
#include "vesDepth.h"
#include "vesGeometryData.h"
#include "vesGLTypes.h"
#include "vesMapper.h"
#include "vesMaterial.h"
#include "vesSourceData.h"
#include "vesTexture.h"
//#include "vesUniform.h"
#include "vesVertexAttribute.h"
#include "vesVertexAttributeKeys.h"
#include "vesViewport.h"

class vesBackground::vesInternal
{
public:
  vesInternal() :
    m_backgroundActor(new vesActor()),
    m_backgroundMapper(new vesMapper()),
    m_backgroundMaterial(new vesMaterial()),
    m_positionVertexAttribute(new vesPositionVertexAttribute()),
    m_normalVertexAttribute(new vesNormalVertexAttribute()),
    m_colorVertexAttribute(new vesColorVertexAttribute()),
    m_textureCoodinateAttribute(new vesTextureCoordinateVertexAttribute()),
    m_depth(new vesDepth())
  {
    this->m_backgroundMaterial->addAttribute(
      this->m_positionVertexAttribute);
    this->m_backgroundMaterial->addAttribute(
      this->m_normalVertexAttribute);
    this->m_backgroundMaterial->addAttribute(
      this->m_colorVertexAttribute);
    this->m_backgroundMaterial->addAttribute(
      this->m_textureCoodinateAttribute);

    this->m_depth->disable();
  }

  ~vesInternal()
  {
  }

  void deleteBackground();
  void createBackground(vesBackground *background, const vesVector4f &topColor,
                        const vesVector4f &bottomColor);
  void createBackground(const vesVector4f &topColor,
                        const vesVector4f &bottomColor);
  vesSharedPtr<vesGeometryData> createBackgroundPlane(
    const vesVector4f &topColor, const vesVector4f &bottomColor);

  vesSharedPtr<vesActor> m_backgroundActor;
  vesSharedPtr<vesMapper> m_backgroundMapper;
  vesSharedPtr<vesMaterial> m_backgroundMaterial;
  vesSharedPtr<vesPositionVertexAttribute> m_positionVertexAttribute;
  vesSharedPtr<vesNormalVertexAttribute> m_normalVertexAttribute;
  vesSharedPtr<vesColorVertexAttribute> m_colorVertexAttribute;
  vesSharedPtr<vesTextureCoordinateVertexAttribute> m_textureCoodinateAttribute;
  vesSharedPtr<vesGeometryData> m_backgroundPlaneData;
  vesSharedPtr<vesDepth> m_depth;
  vesSharedPtr<vesImage> m_image;
  vesSharedPtr<vesTexture> m_texture;

  vesVector4f m_topColor;
  vesVector4f m_bottomColor;
};


void vesBackground::vesInternal::deleteBackground()
{
}


void vesBackground::vesInternal::createBackground(const vesVector4f &topColor,
  const vesVector4f &bottomColor)
{
  this->m_backgroundActor = vesActor::Ptr(new vesActor());
  this->m_backgroundMapper = vesMapper::Ptr(new vesMapper());

  this->m_backgroundPlaneData =
    this->createBackgroundPlane(topColor, bottomColor);

  std::string vertShaderText;
  std::string fragShaderText;
}


void vesBackground::vesInternal::createBackground(vesBackground *background,
  const vesVector4f &topColor, const vesVector4f &bottomColor)
{
  if (this->m_backgroundActor) {
    background->removeChild(this->m_backgroundActor);
  }

  this->deleteBackground();
  this->createBackground(topColor, bottomColor);

  this->m_backgroundActor->setMapper(this->m_backgroundMapper);
  this->m_backgroundMapper->setGeometryData(this->m_backgroundPlaneData);
  this->m_backgroundActor->setMaterial(this->m_backgroundMaterial);
  this->m_backgroundMaterial->addAttribute(this->m_depth);

  if(this->m_image)
  {
    this->m_texture = vesSharedPtr<vesTexture>(new vesTexture());
    this->m_texture->setImage(this->m_image);
    this->m_backgroundMaterial->addAttribute(this->m_texture);
  }

  background->addChild(this->m_backgroundActor);
}


vesSharedPtr<vesGeometryData> vesBackground::vesInternal::createBackgroundPlane(
  const vesVector4f &topColor, const vesVector4f &bottomColor)
{
  vesGeometryData::Ptr backgroundGeometryData (new vesGeometryData());
  vesSourceDataP4T4C4f::Ptr sourceData(new vesSourceDataP4T4C4f());

  // Points.
  vesVertexDataP4T4C4f v1;
  v1.m_position = vesVector4f(-1.0f, -1.0f, 0.0f, 1.0f);
  v1.m_color = vesVector4f(bottomColor[0], bottomColor[1], bottomColor[2],
                           bottomColor[3]);
  v1.m_texureCoordinates = vesVector4f(0.0f, 0.0f, 0.0f, 0.0f);

  vesVertexDataP4T4C4f v2;
  v2.m_position = vesVector4f(1.0f, -1.0f, 0.0f, 1.0f);
  v2.m_color = vesVector4f(bottomColor[0], bottomColor[1], bottomColor[2],
                           bottomColor[3]);
  v2.m_texureCoordinates = vesVector4f(1.0f, 0.0f, 0.0f, 0.0f);

  vesVertexDataP4T4C4f v3;
  v3.m_position = vesVector4f(1.0f, 1.0f, 0.0f, 1.0f);
  v3.m_color = vesVector4f(topColor[0], topColor[1], topColor[2], topColor[3]);
  v3.m_texureCoordinates = vesVector4f(1.0f, 1.0f, 0.0f, 0.0f);

  vesVertexDataP4T4C4f v4;
  v4.m_position = vesVector4f(-1.0f, 1.0f, 0.0f, 1.0f);
  v4.m_color = vesVector4f(topColor[0], topColor[1], topColor[2], topColor[3]);
  v4.m_texureCoordinates = vesVector4f(0.0f, 1.0f, 0.0f, 0.0f);

  sourceData->pushBack(v1);
  sourceData->pushBack(v2);
  sourceData->pushBack(v3);
  sourceData->pushBack(v4);

  // Triangle cells.
  vesPrimitive::Ptr triangles (new vesPrimitive());
  triangles->setPrimitiveType(vesPrimitiveRenderType::Triangles);
  triangles->setIndicesValueType(vesPrimitiveIndicesValueType::UnsignedShort);
  triangles->setIndexCount(3);
  triangles->setIndicesValueType(vesPrimitiveIndicesValueType::UnsignedShort);

  vesIndices<unsigned short>::Ptr triangleIndices  (new vesIndices<unsigned short>());
  triangleIndices->pushBackIndices(0, 3, 2);
  triangleIndices->pushBackIndices(1, 0, 2);
  triangles->setVesIndices(triangleIndices);

  backgroundGeometryData->setName("BackgroundData");
  backgroundGeometryData->addSource(sourceData);
  backgroundGeometryData->addPrimitive(triangles);
  return backgroundGeometryData;
}


vesBackground::vesBackground() : vesCamera()
{
  this->m_topColor = this->m_clearColor;
  this->m_bottomColor = this->m_topColor;

  this->m_internal = new vesInternal();
  this->setReferenceFrame(Absolute);
  this->setRenderOrder(PreRender);
  this->createBackground();
}


vesBackground::~vesBackground()
{
  delete this->m_internal; this->m_internal = 0x0;
}


void vesBackground::setColor(const vesVector4f &color)
{
  this->setGradientColor(color, color);
}


void vesBackground::setGradientColor(const vesVector4f &topColor,
                                     const vesVector4f &bottomColor)
{
  this->m_topColor = topColor;
  this->m_bottomColor = bottomColor;

  this->createBackground();
}


void vesBackground::setImage(const vesSharedPtr<vesImage> image)
{
  this->m_internal->m_image = image;
  this->createBackground();
}


vesSharedPtr<vesImage> vesBackground::image() const
{
  return this->m_internal->m_image;
}


vesMatrix4x4f vesBackground::modelViewMatrix()
{
  return vesMatrix4x4f::Identity();
}


vesMatrix4x4f vesBackground::projectionMatrix()
{
  vesMatrix4x4f projection2DMatrix = vesOrtho(-1, 1, -1, 1, -1, 1);
  return projection2DMatrix;
}


void vesBackground::createBackground()
{
  this->m_internal->createBackground(this, this->m_topColor, this->m_bottomColor);
}
