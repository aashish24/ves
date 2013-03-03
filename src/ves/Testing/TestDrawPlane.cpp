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

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <vesActor.h>
#include <vesCamera.h>
#include <vesMapper.h>
#include <vesMaterial.h>
#include <vesModelViewUniform.h>
#include <vesProjectionUniform.h>
#include <vesRenderer.h>
#include <vesShader.h>
#include <vesShaderProgram.h>
#include <vesUniform.h>
#include <vesVertexAttribute.h>
#include <vesVertexAttributeKeys.h>
#include <vesViewport.h>

#include <vesTestHelper.h>


class vesTestDrawPlane {
public:

  vesTestDrawPlane() :
    m_modelViewUniform(new vesModelViewUniform()),
    m_projectionUniform(new vesProjectionUniform()),
    m_positionVertexAttribute(new vesPositionVertexAttribute()),
    m_normalVertexAttribute(new vesNormalVertexAttribute()),
    m_colorVertexAttribute(new vesColorVertexAttribute()),
    m_vertexShader(new vesShader(vesShader::Vertex)),
    m_fragmentShader(new vesShader(vesShader::Fragment)),
    m_shaderProgram(vesShaderProgram::Ptr(new vesShaderProgram())),
    m_material(vesMaterial::Ptr(new vesMaterial())),
    m_mapper(vesMapper::Ptr(new vesMapper())),
    m_actor(vesActor::Ptr(new vesActor())),
    m_renderer(vesRenderer::Ptr(new vesRenderer()))
  {
  }

  ~vesTestDrawPlane()
  {
  }

  vesRenderer::Ptr renderer() {
    return m_renderer;
  }

  void init()
  {
    const std::string vertexShaderSource =
      "uniform highp mat4 modelViewMatrix;\n \
       uniform highp mat4 projectionMatrix;\n \
       attribute highp vec4 vertexPosition;\n \
       attribute mediump vec4 vertexColor;\n \
       varying mediump vec4 varColor;\n \
       void main()\n \
       {\n \
         gl_Position = projectionMatrix * modelViewMatrix * vertexPosition;\n \
         varColor = vertexColor;\n \
       }";

    const std::string fragmentShaderSource =
      "varying mediump vec4 varColor;\n \
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

    this->m_material->addAttribute(this->m_shaderProgram);
    this->m_mapper->setGeometryData(this->createPlane());
    this->m_mapper->setColor(1.0, 1.0, 0.0, 1.0);
    this->m_actor->setMapper(this->m_mapper);
    this->m_actor->setMaterial(this->m_material);
    this->m_renderer->addActor(this->m_actor);

    this->m_renderer->camera()->setParallelProjection(false);
    this->m_renderer->setBackgroundColor(0.0, 0.0, 0.0);
  }

  vesSharedPtr<vesGeometryData> createPlane()
  {
    vesGeometryData::Ptr geometryData (new vesGeometryData());
    vesSourceDataP3N3C3f::Ptr sourceData(new vesSourceDataP3N3C3f());

    vesVector4f topLeftColor = vesVector4f(1.0f, 0.0f, 0.0f, 1.0f);
    vesVector4f bottomRightColor = vesVector4f(0.0f, 0.0f, 1.0f, 1.0f);
    vesVector4f color = vesVector4f(0.0f, 1.0f, 0.0f, 1.0f);

    // Points.
    vesVertexDataP3N3C3f v1;
    v1.m_position = vesVector3f(-1.0f, -1.0f, 0.0f);
    v1.m_normal = vesVector3f(0.0f, 0.0f, 1.0f);
    v1.m_color = vesVector3f(color[0], color[1], color[2]);

    vesVertexDataP3N3C3f v2;
    v2.m_position = vesVector3f(1.0f, -1.0f, 0.0f);
    v2.m_normal = vesVector3f(0.0f, 0.0f, 1.0f);
    v2.m_color = vesVector3f(bottomRightColor[0], bottomRightColor[1], bottomRightColor[2]);

    vesVertexDataP3N3C3f v3;
    v3.m_position = vesVector3f(1.0f, 1.0f, 0.0f);
    v3.m_normal = vesVector3f(0.0f, 0.0f, 1.0f);
    v3.m_color = vesVector3f(color[0], color[1], color[2]);

    vesVertexDataP3N3C3f v4;
    v4.m_position = vesVector3f(-1.0f, 1.0f, 0.0f);
    v4.m_normal = vesVector3f(0.0f, 0.0f, 1.0f);
    v4.m_color = vesVector3f(topLeftColor[0], topLeftColor[1], topLeftColor[2]);

    sourceData->pushBack(v1);
    sourceData->pushBack(v2);
    sourceData->pushBack(v3);
    sourceData->pushBack(v4);

    // Triangle cells.
    vesPrimitive::Ptr triangles (new vesPrimitive());
    vesSharedPtr< vesIndices<unsigned short> > indices (new vesIndices<unsigned short>());
    indices->pushBackIndices(0, 3, 2);
    indices->pushBackIndices(1, 0, 2);
    triangles->setVesIndices(indices);
    triangles->setPrimitiveType(vesPrimitiveRenderType::Triangles);
    triangles->setIndexCount(3);
    triangles->setIndicesValueType(vesPrimitiveIndicesValueType::UnsignedShort);

    geometryData->setName("PlaneGeometryData");
    geometryData->addSource(sourceData);
    geometryData->addPrimitive(triangles);
    return geometryData;
  }

  void toggleVisibility()
  {
    this->m_actor->setVisible(!this->m_actor->isVisible());
  }

  void toggleColorVisibility()
  {
    this->m_material->enableVertexColor(!this->m_material->isEnabledVertexColor());
  }


private:

  vesSharedPtr<vesModelViewUniform> m_modelViewUniform;
  vesSharedPtr<vesProjectionUniform> m_projectionUniform;
  vesSharedPtr<vesPositionVertexAttribute> m_positionVertexAttribute;
  vesSharedPtr<vesNormalVertexAttribute> m_normalVertexAttribute;
  vesSharedPtr<vesColorVertexAttribute> m_colorVertexAttribute;
  vesSharedPtr<vesTextureCoordinateVertexAttribute> m_textureCoodinateAttribute;
  vesSharedPtr<vesGeometryData> m_backgroundPlaneData;

  vesShader::Ptr m_vertexShader;
  vesShader::Ptr m_fragmentShader;
  vesShaderProgram::Ptr m_shaderProgram;
  vesMaterial::Ptr m_material;
  vesMapper::Ptr m_mapper;
  vesActor::Ptr m_actor;
  vesRenderer::Ptr m_renderer;

};




class MyTestHelper : public vesTestHelper {
public:

  MyTestHelper()
  {
    this->setRenderer(mApp.renderer());
  }

  void handleKeyboard(unsigned char key, int x, int y)
  {
    if (key == 'v') {
      mApp.toggleVisibility();
    }
    else if (key == 'c') {
      mApp.toggleColorVisibility();
    }
    else {
      vesTestHelper::handleKeyboard(key, x, y);
    }
  }

  vesTestDrawPlane mApp;
};


int main(int argc, char *argv[])
{

  if (argc < 2) {
    printf("Usage: %s <path to VES source directory> [path to testing data directory]\n", argv[0]);
    return 1;
  }


  const int windowWidth = 800;
  const int windowHeight = 600;
  MyTestHelper helper;
  helper.init(&argc, argv, windowWidth, windowHeight, "TestDrawPlane");
  helper.mApp.init();


  int printInfo = 1;
  if (printInfo) {
    printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
    printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
    printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
    printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
  }

  // render once
  helper.resize(windowWidth, windowHeight);
  helper.renderer()->resetCamera();
  helper.render();

  // begin the event loop if not in testing mode
  if (argc < 3) {
    helper.startMainLoop();
  }

  return 0;
}
