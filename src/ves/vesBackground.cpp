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
#include "vesModelViewUniform.h"
#include "vesProjectionUniform.h"
#include "vesShader.h"
#include "vesShaderProgram.h"
#include "vesTexture.h"
#include "vesUniform.h"
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
    m_colorVertexAttribute(new vesColorVertexAttribute()),
    m_textureCoodinateAttribute(new vesTextureCoordinateVertexAttribute()),
    m_depth(new vesDepth())
  {
    this->m_shaderProgram->addShader(this->m_vertexShader);
    this->m_shaderProgram->addShader(this->m_fragmentShader);
    this->m_shaderProgram->addUniform(this->m_modelViewUniform);
    this->m_shaderProgram->addUniform(this->m_projectionUniform);
    this->m_shaderProgram->addVertexAttribute(
      this->m_positionVertexAttribute, vesVertexAttributeKeys::Position);
    this->m_shaderProgram->addVertexAttribute(
      this->m_normalVertexAttribute, vesVertexAttributeKeys::Normal);
    this->m_shaderProgram->addVertexAttribute(
      this->m_colorVertexAttribute, vesVertexAttributeKeys::Color);
    this->m_shaderProgram->addVertexAttribute(
      this->m_textureCoodinateAttribute,
      vesVertexAttributeKeys::TextureCoordinate);
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

  void createShaderSourceForNonTexturedPlane(std::string &vertShaderText,
                                             std::string &fragShaderText)
  {
    const std::string vertexShaderSource =
      "uniform highp mat4 modelViewMatrix;\n \
       uniform highp mat4 projectionMatrix;\n \
       attribute highp vec4 vertexPosition;\n \
       attribute mediump vec4 vertexColor;\n \
       varying mediump vec4 varColor;\n \
       void main()\n \
       {\n \
         gl_Position = vertexPosition;\n \
         varColor = vertexColor;\n \
       }";

    const std::string fragmentShaderSource =
      "varying mediump vec4 varColor;\n \
       void main()\n \
       {\n \
         gl_FragColor = varColor;\n \
       }";

    vertShaderText = vertexShaderSource;
    fragShaderText = fragmentShaderSource;
  }

  void createShaderSourceForTexturedPlane(std::string &vertShaderText,
                                          std::string &fragShaderText)
  {
    const std::string vertexShaderSource =
      "uniform highp mat4 modelViewMatrix;\n \
       uniform highp mat4 projectionMatrix;\n \
       attribute highp vec4 vertexPosition;\n \
       attribute mediump vec3 vertexTextureCoordinate;\n \
       varying mediump vec2 textureCoordinate;\n \
       void main()\n \
       {\n \
         gl_Position = vertexPosition;\n \
         textureCoordinate = vertexTextureCoordinate.xy;\n \
       }";

    const std::string fragmentShaderSource =
       "varying mediump vec2 textureCoordinate;\n \
        uniform highp sampler2D image;\n \
       void main()\n \
       {\n \
          gl_FragColor = texture2D(image, textureCoordinate);\n \
       }";

    vertShaderText = vertexShaderSource;
    fragShaderText = fragmentShaderSource;
  }

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

  if(!this->m_image)
  {
    this->createShaderSourceForNonTexturedPlane(vertShaderText, fragShaderText);
  }
  else
  {
    this->createShaderSourceForTexturedPlane(vertShaderText, fragShaderText);
  }

  this->m_vertexShader->setShaderSource(vertShaderText);
  this->m_fragmentShader->setShaderSource(fragShaderText);
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
  this->m_backgroundMaterial->addAttribute(this->m_shaderProgram);
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
  vesSourceDataP3T3C3f::Ptr sourceData(new vesSourceDataP3T3C3f());

  // Points.
  vesVertexDataP3T3C3f v1;
  v1.m_position = vesVector3f(-1.0f, -1.0f, 0.0f);
  v1.m_color = vesVector3f(bottomColor[0], bottomColor[1], bottomColor[2]);
  v1.m_texureCoordinates = vesVector3f(0.0f, 0.0f, 0.0f);

  vesVertexDataP3T3C3f v2;
  v2.m_position = vesVector3f(1.0f, -1.0f, 0.0f);
  v2.m_color = vesVector3f(bottomColor[0], bottomColor[1], bottomColor[2]);
  v2.m_texureCoordinates = vesVector3f(1.0f, 0.0f, 0.0f);

  vesVertexDataP3T3C3f v3;
  v3.m_position = vesVector3f(1.0f, 1.0f, 0.0f);
  v3.m_color = vesVector3f(topColor[0], topColor[1], topColor[2]);
  v3.m_texureCoordinates = vesVector3f(1.0f, 1.0f, 0.0f);

  vesVertexDataP3T3C3f v4;
  v4.m_position = vesVector3f(-1.0f, 1.0f, 0.0f);
  v4.m_color = vesVector3f(topColor[0], topColor[1], topColor[2]);
  v4.m_texureCoordinates = vesVector3f(0.0f, 1.0f, 0.0f);

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
