
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
  vesInternal()
  {
    this->m_backgroundActor = new vesActor();
    this->m_backgroundMapper = new vesMapper();
    this->m_backgroundMaterial = new vesMaterial();
    this->m_shaderProgram = new vesShaderProgram();
    this->m_vertexShader = new vesShader(vesShader::Vertex);
    this->m_fragmentShader = new vesShader(vesShader::Fragment);
    this->m_modelViewUniform = new vesModelViewUniform();
    this->m_projectionUniform = new vesProjectionUniform();
    this->m_positionVertexAttribute = new vesPositionVertexAttribute();
    this->m_normalVertexAttribute = new vesNormalVertexAttribute();
    this->m_colorVertexAttribute = new vesColorVertexAttribute();
    this->m_textureCoodinateAttribute = new vesTextureCoordinateVertexAttribute();
    this->m_backgroundPlaneData = 0x0;
    this->m_depth = new vesDepth();
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
    delete this->m_backgroundActor; this->m_backgroundActor = 0x0;
    delete this->m_backgroundMapper; this->m_backgroundMapper = 0x0;
    delete this->m_backgroundMaterial; this->m_backgroundMaterial = 0x0;
    delete this->m_shaderProgram; this->m_shaderProgram = 0x0;
    delete this->m_modelViewUniform; this->m_modelViewUniform = 0x0;
    delete this->m_projectionUniform; this->m_projectionUniform = 0x0;
    delete this->m_positionVertexAttribute; this->m_positionVertexAttribute = 0x0;
    delete this->m_normalVertexAttribute; this->m_normalVertexAttribute = 0x0;
    delete this->m_colorVertexAttribute; this->m_colorVertexAttribute = 0x0;
    delete this->m_textureCoodinateAttribute; this->m_textureCoodinateAttribute = 0x0;
    delete this->m_backgroundPlaneData; this->m_backgroundPlaneData = 0x0;
    delete this->m_depth; this->m_depth = 0x0;
  }

  void deleteBackground();
  void createBackground(vesBackground *background, const vesVector4f &topColor,
                        const vesVector4f &bottomColor);
  void createBackground(const vesVector4f &topColor,
                        const vesVector4f &bottomColor);
  vesTriangleData* createBackgroundPlane(const vesVector4f &topColor,
                                         const vesVector4f &bottomColor);

  vesActor *m_backgroundActor;
  vesMapper *m_backgroundMapper;
  vesMaterial *m_backgroundMaterial;
  vesShaderProgram *m_shaderProgram;
  vesShader* m_vertexShader;
  vesShader* m_fragmentShader;
  vesModelViewUniform *m_modelViewUniform;
  vesProjectionUniform *m_projectionUniform;
  vesPositionVertexAttribute *m_positionVertexAttribute;
  vesNormalVertexAttribute *m_normalVertexAttribute;
  vesColorVertexAttribute *m_colorVertexAttribute;
  vesTextureCoordinateVertexAttribute *m_textureCoodinateAttribute;
  vesTriangleData *m_backgroundPlaneData;
  vesDepth *m_depth;

  vesVector4f m_topColor;
  vesVector4f m_bottomColor;
};


void vesBackground::vesInternal::deleteBackground()
{
  delete this->m_backgroundActor; this->m_backgroundActor = 0x0;
  delete this->m_backgroundMapper; this->m_backgroundMapper = 0x0;
  delete this->m_backgroundPlaneData; this->m_backgroundPlaneData = 0x0;
}


void vesBackground::vesInternal::createBackground(const vesVector4f &topColor,
  const vesVector4f &bottomColor)
{
  this->m_backgroundActor = new vesActor();
  this->m_backgroundMapper = new vesMapper();

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


vesTriangleData* vesBackground::vesInternal::createBackgroundPlane(
  const vesVector4f &topColor, const vesVector4f &bottomColor)
{
  vesTriangleData *backgroundPlaneData = new vesTriangleData();

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
