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

#include "vesBackground.h"

// VES includes.
#include "vesActor.h"
#include "vesDepth.h"
#include "vesMapper.h"
#include "vesMaterial.h"
#include "vesModelViewUniform.h"
#include "vesProjectionUniform.h"
#include "vesShader.h"
#include "vesShaderProgram.h"
#include "vesTriangleData.h"
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
    m_shaderProgram(new vesShaderProgram()),
    m_vertexShader(new vesShader(vesShader::Vertex)),
    m_fragmentShader(new vesShader(vesShader::Fragment)),
    m_modelViewUniform(new vesModelViewUniform()),
    m_projectionUniform(new vesProjectionUniform()),
    m_positionVertexAttribute(new vesPositionVertexAttribute()),
    m_normalVertexAttribute(new vesNormalVertexAttribute()),
    m_colorVertexAttribute(new vesColorVertexAttribute()),
    m_textureCoodinateAttribute(new vesTextureCoordinateVertexAttribute()),
    m_depth(new vesDepth())
  {
    this->m_depth->disable();

    const std::string vertexShaderSource =
      "uniform highp mat4 modelViewMatrix;\n \
       uniform highp mat4 projectionMatrix;\n \
       attribute highp vec4 vertexPosition;\n \
       attribute mediump vec4 vertexColor;\n \
       attribute mediump vec4 vertexTextureCoordinate;\n \
       varying mediump vec2 textureCoordinate;\n \
       varying mediump vec4 varColor;\n \
       void main()\n \
       {\n \
         gl_Position = vertexPosition;\n \
         textureCoordinate = vertexTextureCoordinate.xy;\n \
         varColor = vertexColor;\n \
       }";

    const std::string fragmentShaderSource =
      "varying mediump vec2 textureCoordinate;\n \
       varying mediump vec4 varColor;\n \
       void main()\n \
       {\n \
         gl_FragColor = varColor;\n \
       }";

    this->m_vertexShader->setShaderSource(vertexShaderSource);
    this->m_fragmentShader->setShaderSource(fragmentShaderSource);

    this->m_shaderProgram->addShader(this->m_vertexShader);
    this->m_shaderProgram->addShader(this->m_fragmentShader);
    this->m_shaderProgram->addUniform(this->m_modelViewUniform);
    this->m_shaderProgram->addUniform(this->m_projectionUniform);
    this->m_shaderProgram->addVertexAttribute(this->m_positionVertexAttribute,
                                              vesVertexAttributeKeys::Position);
    this->m_shaderProgram->addVertexAttribute(this->m_normalVertexAttribute,
                                              vesVertexAttributeKeys::Normal);
    this->m_shaderProgram->addVertexAttribute(this->m_colorVertexAttribute,
                                              vesVertexAttributeKeys::Color);
    this->m_shaderProgram->addVertexAttribute(this->m_textureCoodinateAttribute,
                                              vesVertexAttributeKeys::TextureCoordinate);
  }

  ~vesInternal()
  {
  }

  void deleteBackground();
  void createBackground(vesBackground *background, const vesVector4f &topColor,
                        const vesVector4f &bottomColor);
  void createBackground(const vesVector4f &topColor,
                        const vesVector4f &bottomColor);
  vesSharedPtr<vesTriangleData> createBackgroundPlane(
    const vesVector4f &topColor, const vesVector4f &bottomColor);

  vesSharedPtr<vesActor> m_backgroundActor;
  vesSharedPtr<vesMapper> m_backgroundMapper;
  vesSharedPtr<vesMaterial> m_backgroundMaterial;
  vesSharedPtr<vesShaderProgram> m_shaderProgram;
  vesSharedPtr<vesShader> m_vertexShader;
  vesSharedPtr<vesShader> m_fragmentShader;
  vesSharedPtr<vesModelViewUniform> m_modelViewUniform;
  vesSharedPtr<vesProjectionUniform> m_projectionUniform;
  vesSharedPtr<vesPositionVertexAttribute> m_positionVertexAttribute;
  vesSharedPtr<vesNormalVertexAttribute> m_normalVertexAttribute;
  vesSharedPtr<vesColorVertexAttribute> m_colorVertexAttribute;
  vesSharedPtr<vesTextureCoordinateVertexAttribute> m_textureCoodinateAttribute;
  vesSharedPtr<vesTriangleData> m_backgroundPlaneData;
  vesSharedPtr<vesDepth> m_depth;

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
  this->m_backgroundMapper->setData(this->createBackgroundPlane(topColor, bottomColor));
  this->m_backgroundActor->setMaterial(this->m_backgroundMaterial);
  this->m_backgroundMaterial->addAttribute(this->m_shaderProgram);
  this->m_backgroundMaterial->addAttribute(this->m_depth);

  background->addChild(this->m_backgroundActor);
}


vesSharedPtr<vesTriangleData> vesBackground::vesInternal::createBackgroundPlane(
  const vesVector4f &topColor, const vesVector4f &bottomColor)
{
  vesSharedPtr<vesTriangleData> backgroundPlaneData
    = vesTriangleData::Ptr(new vesTriangleData());

  std::vector<vtkVertex3f> points;

  // Points.
  vtkVertex3f point1;
  point1.point = vesVector3f(-1.0f, -1.0f, 0.0f);
  point1.normal = vesVector3f(0.0f, 0.0f, 1.0f);
  vtkVertex3f point2;
  point2.point = vesVector3f(1.0f, -1.0f, 0.0f);
  point2.normal = vesVector3f(0.0f, 0.0f, 1.0f);
  vtkVertex3f point3;
  point3.point = vesVector3f(1.0f, 1.0f, 0.0f);
  point3.normal = vesVector3f(0.0f, 0.0f, 1.0f);
  vtkVertex3f point4;
  point4.point = vesVector3f(-1.0f, 1.0f, 0.0f);
  point4.normal = vesVector3f(0.0f, 0.0f, 1.0f);

  points.push_back(point1);
  points.push_back(point2);
  points.push_back(point3);
  points.push_back(point4);

  // Triangle cells.
  std::vector<vesVector3us> indices;
  indices.push_back(vesVector3us(0, 3, 2));
  indices.push_back(vesVector3us(1, 0, 2));

  // Colors
  std::vector<vesVector3f> vertexColors;
  vertexColors.push_back(vesVector3f(bottomColor[0], bottomColor[1], bottomColor[2]));
  vertexColors.push_back(vesVector3f(bottomColor[0], bottomColor[1], bottomColor[2]));
  vertexColors.push_back(vesVector3f(topColor[0], topColor[1], topColor[2]));
  vertexColors.push_back(vesVector3f(topColor[0], topColor[1], topColor[2]));

  backgroundPlaneData->SetVertexColors(vertexColors);
  backgroundPlaneData->SetPoints(points);
  backgroundPlaneData->SetTriangles(indices);
  backgroundPlaneData->SetHasNormals(true);

  return backgroundPlaneData;
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


vesMatrix4x4f vesBackground::modelViewMatrix()
{
  return vesMatrix4x4f();
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
