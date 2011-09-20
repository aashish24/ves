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

#include "vesKiwiViewerApp.h"
#include "vesKiwiDataLoader.h"

#include "vesActor.h"
#include "vesCamera.h"
#include "vesGMTL.h"
#include "vesMapper.h"
#include "vesRenderer.h"
#include "vesShader.h"
#include "vesShaderProgram.h"
#include "vesTriangleData.h"
#include "vesUniform.h"

#include <cassert>
#include <cmath>

#include <algorithm>
#include <string>
#include <vector>

//----------------------------------------------------------------------------
class vesKiwiViewerApp::vesInternal
{
public:

  vesInternal()
  {
    this->Actor         = 0;
    this->Mapper        = 0;
    this->Renderer      = 0;
    this->ShaderProgram = 0;
  }

  ~vesInternal()
  {
    delete this->Actor;
    delete this->Mapper->triangleData();
    delete this->Mapper;
    delete this->Renderer;
    delete this->ShaderProgram;
  }

  vesActor*         Actor;
  vesMapper*        Mapper;
  vesRenderer*      Renderer;
  vesShaderProgram* ShaderProgram;

  vesKiwiDataLoader DataLoader;

  std::string VertexShaderSource;
  std::string FragmentShaderSource;

  std::vector<std::string> BuiltinDatasetNames;
  std::vector<std::string> BuiltinDatasetFilenames;

  std::string CurrentShadingModel;
  std::vector<std::string> BuiltinShadingModels;
};

//----------------------------------------------------------------------------
vesKiwiViewerApp::vesKiwiViewerApp()
{
  this->Internal = new vesInternal();

  this->addBuiltinDataset("Sphere", "current.stl");
  this->addBuiltinDataset("Utah Teapot", "teapot.vtp");
  this->addBuiltinDataset("Stanford Bunny", "bunny.vtp");
  this->addBuiltinDataset("NLM Visible Woman Hand", "visible-woman-hand.vtp");
  this->addBuiltinDataset("NAMIC Knee Atlas", "AppendedKneeData.vtp");
  this->addBuiltinDataset("Mount St. Helens", "MountStHelen.vtp");
  this->addBuiltinDataset("Space Shuttle", "shuttle.vtp");
  this->addBuiltinDataset("Buckyball", "Buckyball.vtp");
  this->addBuiltinDataset("Motor", "motor.g");
  this->addBuiltinDataset("Caffeine", "caffeine.pdb");

  this->addBuiltinShadingModel("Gouraud");
  this->addBuiltinShadingModel("Blinn-Phong");
  this->addBuiltinShadingModel("Toon");
}

//----------------------------------------------------------------------------
vesKiwiViewerApp::~vesKiwiViewerApp()
{
  delete this->Internal;

  delete this->m_modelViewProjectionUniform;
  delete this->m_normalMatrixUniform;
  delete this->m_lightDirectionUniform;
  delete this->m_opacityUniform;
  delete this->m_enableDiffuseUniform;
  delete this->m_useGouraudShaderUniform;
  delete this->m_useBlinnPhongShaderUniform;
  delete this->m_useToonShaderUniform;
}

//----------------------------------------------------------------------------
vesActor* vesKiwiViewerApp::actor() const
{
  return this->Internal->Actor;
}

//----------------------------------------------------------------------------
vesCamera* vesKiwiViewerApp::camera() const
{
  assert(this->Internal->Renderer);
  return this->Internal->Renderer->GetCamera();
}

//----------------------------------------------------------------------------
vesMapper* vesKiwiViewerApp::mapper() const
{
  return this->Internal->Mapper;
}

//----------------------------------------------------------------------------
vesRenderer* vesKiwiViewerApp::renderer() const
{
  return this->Internal->Renderer;
}

//----------------------------------------------------------------------------
vesShaderProgram* vesKiwiViewerApp::shaderProgram() const
{
  return this->Internal->ShaderProgram;
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfBuiltinDatasets() const
{
  return static_cast<int>(this->Internal->BuiltinDatasetNames.size());
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::defaultBuiltinDatasetIndex() const
{
  return 6;
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::builtinDatasetName(int index)
{
  assert(index >= 0 && index < this->numberOfBuiltinDatasets());
  return this->Internal->BuiltinDatasetNames[index];
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::builtinDatasetFilename(int index)
{
  assert(index >= 0 && index < this->numberOfBuiltinDatasets());
  return this->Internal->BuiltinDatasetFilenames[index];
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::addBuiltinDataset(const std::string& name, const std::string& filename)
{
  this->Internal->BuiltinDatasetNames.push_back(name);
  this->Internal->BuiltinDatasetFilenames.push_back(filename);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::addBuiltinShadingModel(const std::string &name)
{
  this->Internal->BuiltinShadingModels.push_back(name);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::render()
{
  glClearColor(63/255.0f, 96/255.0f, 144/255.0, 1.0f);

  // \Note: We have to call it every render call since the current
  // implementation is not quite right in VES library.
  this->setShadingModel(this->getCurrentShadingModel());

  this->Internal->Renderer->ResetCameraClippingRange();
  this->Internal->Renderer->Render();
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::resizeView(int width, int height)
{
  this->Internal->Renderer->Resize(width, height, 1.0f);
  glViewport(0, 0, width, height);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::resetView()
{
  // this is just confusing...
  // We want to set the direction to look from and view up
  // then we want to dolly the camera so that the surface takes up
  // a reasonable portion of the screen.
  // the ResetCamera function will overwrite view up
  // so we have to call things in this strange order.

  //
  // set direction to look from
  vesRenderer* renderer = this->Internal->Renderer;
  renderer->GetCamera()->SetViewPlaneNormal(vesVector3f(0.0, 0.0, 1.0));

  // dolly so that scene fits window
  renderer->ResetCamera();

  // The current ResetCamera() method pulls the camera back further than
  // required.  ResetCamera should be fixed.  Until then, perform a dolly
  // with a scale factor of 1.5 (a magic number).
  renderer->GetCamera()->Dolly(1.5);

  // now set the view plane normal
  renderer->GetCamera()->SetViewUp(vesVector3f(0.0, 1.0, 0.0));
  renderer->GetCamera()->OrthogonalizeViewUp();
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleTwoTouchPanGesture(double x0, double y0, double x1, double y1)
{
  // calculate the focal depth so we'll know how far to move
  vesRenderer* ren = this->Internal->Renderer;
  vesCamera* camera = ren->GetCamera();
  vesVector3f viewFocus = camera->GetFocalPoint();
  vesVector3f viewPoint = camera->GetPosition();
  vesVector3f viewFocusDisplay = ren->ComputeWorldToDisplay(viewFocus);
  float focalDepth = viewFocusDisplay[2];

  // map change into world coordinates
  vesVector3f oldPickPoint = ren->ComputeDisplayToWorld(vesVector3f(x0, y0, focalDepth));
  vesVector3f newPickPoint = ren->ComputeDisplayToWorld(vesVector3f(x1, y1, focalDepth));
  vesVector3f motionVector = oldPickPoint - newPickPoint;

  vesVector3f newViewFocus = viewFocus + motionVector;
  vesVector3f newViewPoint = viewPoint + motionVector;
  camera->SetFocalPoint(newViewFocus);
  camera->SetPosition(newViewPoint);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleSingleTouchPanGesture(double deltaX, double deltaY)
{
  //
  // Rotate camera
  // Based on vtkInteractionStyleTrackballCamera::Rotate().
  //
  vesRenderer* ren = this->Internal->Renderer;
  vesCamera *camera = ren->GetCamera();

  double delta_elevation = -20.0 / ren->GetHeight();
  double delta_azimuth = -20.0 / ren->GetWidth();
  double motionFactor = 10.0;

  double rxf = deltaX * delta_azimuth * motionFactor;
  double ryf = deltaY * delta_elevation * motionFactor;

  camera->Azimuth(rxf);
  camera->Elevation(ryf);
  camera->OrthogonalizeViewUp();
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleTwoTouchPinchGesture(double scale)
{
  this->Internal->Renderer->GetCamera()->Dolly(scale);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleTwoTouchRotationGesture(double rotation)
{
  vesCamera* camera = this->Internal->Renderer->GetCamera();
  camera->Roll(rotation * 180.0 / M_PI);
  camera->OrthogonalizeViewUp();
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setVertexShaderSource(const std::string& source)
{
  this->Internal->VertexShaderSource = source;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setFragmentShaderSource(const std::string& source)
{
  this->Internal->FragmentShaderSource = source;
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::getNumberOfShadingModels() const
{
  return static_cast<int>(this->Internal->BuiltinShadingModels.size());
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::getCurrentShadingModel() const
{
  return this->Internal->CurrentShadingModel;
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::getShadingModel(int index) const
{
  std::string empty;
  if(index < 0 ||
     index > static_cast<int>(this->Internal->BuiltinShadingModels.size()))
  {
  return empty;
  }

  return this->Internal->BuiltinShadingModels[index];
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::setShadingModel(const std::string& name)
{
  bool success = false;

  std::vector<std::string>::iterator itr;
  itr = std::find(this->Internal->BuiltinShadingModels.begin(),
                  this->Internal->BuiltinShadingModels.end(),
                  name);

  if(itr != this->Internal->BuiltinShadingModels.end())
  {
    success = true;

    this->Internal->CurrentShadingModel = name;

    if(name.compare("Gouraud") == 0)
    {
      this->m_enableDiffuseUniform->set(1);
      this->m_useGouraudShaderUniform->set(1);
      this->m_useBlinnPhongShaderUniform->set(0);
      this->m_useToonShaderUniform->set(0);
    }
    else if(name.compare("Blinn-Phong") == 0)
    {
      this->m_enableDiffuseUniform->set(1);
      this->m_useGouraudShaderUniform->set(0);
      this->m_useBlinnPhongShaderUniform->set(1);
      this->m_useToonShaderUniform->set(0);
    }
    else // Must be "Toon" shader.
    {
      this->m_enableDiffuseUniform->set(1);
      this->m_useGouraudShaderUniform->set(0);
      this->m_useBlinnPhongShaderUniform->set(0);
      this->m_useToonShaderUniform->set(1);
    }
  }

  return success;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initializeShaderUniforms()
{
  this->m_modelViewProjectionUniform = new vesUniform("modelViewProjectionMatrix",
    vesMatrix4x4f());
  this->m_normalMatrixUniform = new vesUniform("normalMatrix", vesMatrix3x3f());
  this->m_lightDirectionUniform = new vesUniform("lightDirection", vesVector3f());
  this->m_opacityUniform = new vesUniform("opacity", 1.0f);

  this->m_enableDiffuseUniform = new vesUniform("enableDiffuse", 1);
  this->m_useGouraudShaderUniform = new vesUniform("useGouraudShader", 1);
  this->m_useBlinnPhongShaderUniform = new vesUniform("useBlinnPhongShader", 0);
  this->m_useToonShaderUniform = new vesUniform("useToonShader", 0);
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initializeShaderProgram()
{
  this->Internal->ShaderProgram = new vesShaderProgram();

  this->Internal->ShaderProgram->addShader(
    new vesShader(vesShader::Vertex, this->Internal->VertexShaderSource));

  this->Internal->ShaderProgram->addShader(
    new vesShader(vesShader::Fragment, this->Internal->FragmentShaderSource));

  this->initializeShaderUniforms();

  this->Internal->ShaderProgram->addUniform(this->m_modelViewProjectionUniform);
  this->Internal->ShaderProgram->addUniform(this->m_normalMatrixUniform);
  this->Internal->ShaderProgram->addUniform(this->m_lightDirectionUniform);
  this->Internal->ShaderProgram->addUniform(this->m_opacityUniform);

  this->Internal->ShaderProgram->addUniform(this->m_enableDiffuseUniform);
  this->Internal->ShaderProgram->addUniform(this->m_useGouraudShaderUniform);
  this->Internal->ShaderProgram->addUniform(this->m_useBlinnPhongShaderUniform);
  this->Internal->ShaderProgram->addUniform(this->m_useToonShaderUniform);

  // Set default shading model.
  this->setShadingModel(this->getShadingModel(0));

  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initializeRendering()
{
  this->Internal->Renderer = new vesRenderer();
  this->Internal->Mapper = new vesMapper();
  this->Internal->Mapper->setTriangleData(new vesTriangleData);
  this->Internal->Actor = new vesActor(this->Internal->Mapper);
  this->Internal->Actor->setColor(0.8, 0.8, 0.8, 1.0);
  this->Internal->Renderer->AddActor(this->Internal->Actor);

  this->Internal->Actor->appearance()->addAttribute(this->Internal->ShaderProgram);
  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadDataset(const std::string& filename)
{
  vesTriangleData* newData = this->Internal->DataLoader.loadDataset(filename);
  if (!newData) {
    return false;
  }

  delete this->Internal->Mapper->triangleData();
  this->Internal->Mapper->setTriangleData(newData);
  return true;
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::loadDatasetErrorTitle() const
{
  return this->Internal->DataLoader.errorTitle();
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::loadDatasetErrorMessage() const
{
  return this->Internal->DataLoader.errorMessage();
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfModelFacets() const
{
  return static_cast<int>(this->Internal->Mapper->triangleData()->GetTriangles().size());
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfModelVertices() const
{
  return static_cast<int>(this->Internal->Mapper->triangleData()->GetPoints().size());
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfModelLines() const
{
  return static_cast<int>(this->Internal->Mapper->triangleData()->GetLines().size());
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::viewWidth() const
{
  return this->Internal->Renderer->GetWidth();
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::viewHeight() const
{
  return this->Internal->Renderer->GetHeight();
}
