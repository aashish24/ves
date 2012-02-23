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

#if defined(__APPLE__)
  #include <GLUT/glut.h>
#else
  // \NOTE It is important that we include header in this order
  #include <vesGL.h>
  #include <GL/glut.h>
#endif

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

#ifdef Bool
#undef Bool
#endif

//----------------------------------------------------------------------------
namespace {

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

  std::string sourceDirectory() {
    return this->SourceDirectory;
  }

  void setSourceDirectory(std::string dir) {
    this->SourceDirectory = dir;
  }

  std::string dataDirectory() {
    return this->DataDirectory;
  }

  void setDataDirectory(std::string dir) {
    this->DataDirectory = dir;
  }

  bool isTesting() {
    return this->IsTesting;
  }

  void setTesting(bool testing) {
    this->IsTesting = testing;
  }

  void init()
  {
    const std::string vertexShaderSource =
      "uniform mat4 modelViewMatrix;\n \
       uniform mat4 projectionMatrix;\n \
       attribute vec4 vertexPosition;\n \
       attribute vec4 vertexColor;\n \
       varying vec4 varColor;\n \
       void main()\n \
       {\n \
         gl_Position = projectionMatrix * modelViewMatrix * vertexPosition;\n \
         varColor = vertexColor;\n \
       }";

    const std::string fragmentShaderSource =
      "varying vec4 varColor;\n \
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
    this->m_actor->setMapper(this->m_mapper);
    this->m_actor->setMaterial(this->m_material);
    this->m_renderer->addActor(this->m_actor);

    this->m_renderer->camera()->setParallelProjection(false);
  }

  vesSharedPtr<vesGeometryData> createPlane()
  {
    vesGeometryData::Ptr geometryData (new vesGeometryData());
    vesSourceDataP3N3C3f::Ptr sourceData(new vesSourceDataP3N3C3f());

    vesVector4f topLeftColor = vesVector4f(0.5f, 0.0f, 0.0f, 1.0f);
    vesVector4f bottomRightColor = vesVector4f(0.0f, 0.0f, 0.5f, 1.0f);
    vesVector4f color = vesVector4f(0.5f, 0.5f, 0.5f, 1.0f);


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
    triangles->pushBackIndices(0, 3, 2);
    triangles->pushBackIndices(1, 0, 2);
    triangles->setPrimitiveType(vesPrimitiveRenderType::Triangles);
    triangles->setIndexCount(3);

    geometryData->setName("PlaneGeometryData");
    geometryData->addSource(sourceData);
    geometryData->addPrimitive(triangles);
    return geometryData;
  }

  void render()
  {
    this->m_renderer->render();
  }

  void resizeView(int winWidth, int winHeight)
  {
    this->m_renderer->resize(winWidth, winHeight, 1.0f);
  }

  void resetView()
  {
    // dolly so that scene fits window
    this->m_renderer->resetCamera();
  }


private:

  std::string       SourceDirectory;
  std::string       DataDirectory;
  bool              IsTesting;

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

//----------------------------------------------------------------------------
vesTestDrawPlane* testDrawPlane;

//----------------------------------------------------------------------------
bool DoTesting()
{
  testDrawPlane->render();

  return true;
}

//----------------------------------------------------------------------------
void InitRendering()
{
  testDrawPlane->init();
}

//----------------------------------------------------------------------------
bool InitTest(int argc, char* argv[])
{
  if (argc < 2) {
    printf("Usage: %s <path to VES source directory> [path to testing data directory]\n", argv[0]);
    return false;
  }

  testDrawPlane = new vesTestDrawPlane();
  testDrawPlane->setSourceDirectory(argv[1]);

  if (argc == 3) {
    testDrawPlane->setDataDirectory(argv[2]);
    testDrawPlane->setTesting(true);
  }
  return true;
}

//----------------------------------------------------------------------------
void FinalizeTest()
{
  delete testDrawPlane;
}

}; // end namespace
//----------------------------------------------------------------------------


void display(void) {
  testDrawPlane->render();
  glutSwapBuffers();
}

void reshape(int width, int height) {
  testDrawPlane->resizeView(width, height);
}

void idle(void) {
  glutPostRedisplay();
}


int main(int argc, char *argv[])
{

  if (!InitTest(argc, argv)) {
    return -1;
  }

  const int winWidth = 800;
  const int winHeight = 600;

  //a basic set up...
  glutInit(&argc, &argv[0]);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(winWidth, winHeight);

  //create the window, the argument is the title
  glutCreateWindow("KiwiViewer");

  //pass the callbacks
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);


  InitRendering();
  testDrawPlane->resizeView(winWidth, winHeight);
  testDrawPlane->resetView();

  // render once
  testDrawPlane->render();
  glutSwapBuffers();

  // begin the event loop if not in testing mode
  bool testPassed = true;
  if (!testDrawPlane->isTesting()) {
    glutMainLoop();
  }
  else {
    testPassed = DoTesting();
  }

  FinalizeTest();


  return testPassed ? 0 : 1;

}
