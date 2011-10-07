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
#include "vesKiwiDataRepresentation.h"

#include "vesCamera.h"
#include "vesGMTL.h"
#include "vesRenderer.h"
#include "vesShader.h"
#include "vesShaderProgram.h"
#include "vesTriangleData.h"
#include "vesUniform.h"

#include <vtkDataSet.h>

#include <cassert>
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>

// \todo: Move it to proper locations.
#include "vesVertexAttribute.h"
#include "vesModelViewUniform.h"
#include "vesProjectionUniform.h"
#include "vesNormalMatrixUniform.h"


//----------------------------------------------------------------------------
class vesKiwiViewerApp::vesInternal
{
public:

  vesInternal()
  {
    this->Renderer      = 0;
    this->ShaderProgram = 0;
  }

  ~vesInternal()
  {
    delete this->Renderer;
    delete this->ShaderProgram;

    delete this->ModelViewUniform;
    delete this->ProjectionUnifom;
    delete this->NormalMatrixUniform;
    delete this->PositionVertexAttribute;
    delete this->NormalVertexAttribute;
    delete this->ColorVertexAttribute;
  }


  vesRenderer*      Renderer;
  vesShaderProgram* ShaderProgram;

  std::vector<vesKiwiDataRepresentation*> DataRepresentations;

  vesKiwiDataLoader DataLoader;

  std::string VertexShaderSource;
  std::string FragmentShaderSource;

  std::vector<std::string> BuiltinDatasetNames;
  std::vector<std::string> BuiltinDatasetFilenames;

  std::string CurrentShadingModel;
  std::vector<std::string> BuiltinShadingModels;

  vesModelViewUniform         *ModelViewUniform;
  vesProjectionUniform        *ProjectionUnifom;
  vesNormalMatrixUniform      *NormalMatrixUniform;

  vesPositionVertexAttribute  *PositionVertexAttribute;
  vesNormalVertexAttribute    *NormalVertexAttribute;
  vesColorVertexAttribute     *ColorVertexAttribute;
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
  this->removeAllDataRepresentations();
  delete this->Internal;
}

//----------------------------------------------------------------------------
vesCamera* vesKiwiViewerApp::camera() const
{
  assert(this->Internal->Renderer);
  return this->Internal->Renderer->camera();
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
  // \Note: We have to call it every render call since the current
  // implementation is not quite right in VES library.
  this->setShadingModel(this->getCurrentShadingModel());

  this->Internal->Renderer->resetCameraClippingRange();
  this->Internal->Renderer->render();
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::resizeView(int width, int height)
{
  this->Internal->Renderer->resize(width, height, 1.0f);
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

  renderer->camera()->SetViewPlaneNormal(vesVector3f(0.0, 0.0, 1.0));

  // dolly so that scene fits window
  renderer->resetCamera();

  // The current ResetCamera() method pulls the camera back further than
  // required.  ResetCamera should be fixed.  Until then, perform a dolly
  // with a scale factor of 1.5 (a magic number).
  renderer->camera()->Dolly(1.5);

  // now set the view plane normal
  renderer->camera()->SetViewUp(vesVector3f(0.0, 1.0, 0.0));
  renderer->camera()->OrthogonalizeViewUp();
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleTwoTouchPanGesture(double x0, double y0, double x1, double y1)
{
  // calculate the focal depth so we'll know how far to move
  vesRenderer* ren = this->Internal->Renderer;
  vesCamera* camera = ren->camera();
  vesVector3f viewFocus = camera->GetFocalPoint();
  vesVector3f viewPoint = camera->GetPosition();
  vesVector3f viewFocusDisplay = ren->computeWorldToDisplay(viewFocus);
  float focalDepth = viewFocusDisplay[2];

  // map change into world coordinates
  vesVector3f oldPickPoint = ren->computeDisplayToWorld(vesVector3f(x0, y0, focalDepth));
  vesVector3f newPickPoint = ren->computeDisplayToWorld(vesVector3f(x1, y1, focalDepth));
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
  vesCamera *camera = ren->camera();

  double delta_elevation = -20.0 / ren->height();
  double delta_azimuth   = -20.0 / ren->width();

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
  this->Internal->Renderer->camera()->Dolly(scale);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleTwoTouchRotationGesture(double rotation)
{
  vesCamera* camera = this->Internal->Renderer->camera();
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
  bool success = true;
  return success;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initializeShaderUniforms()
{
  // Do nothing.
  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initializeShaderProgram()
{
  std::cout << "vesKiwiViewerApp::initializeShaderProgram " << std::endl;

  this->Internal->ShaderProgram = new vesShaderProgram();

  this->Internal->ShaderProgram->addShader(
    new vesShader(vesShader::Vertex, this->Internal->VertexShaderSource));

  this->Internal->ShaderProgram->addShader(
    new vesShader(vesShader::Fragment, this->Internal->FragmentShaderSource));


  // \todo: Delete this during destructions.
  this->Internal->ModelViewUniform = new vesModelViewUniform();
  this->Internal->ProjectionUnifom = new vesProjectionUniform();
  this->Internal->NormalMatrixUniform = new vesNormalMatrixUniform();

  this->Internal->PositionVertexAttribute = new vesPositionVertexAttribute();
  this->Internal->NormalVertexAttribute   = new vesNormalVertexAttribute();
  this->Internal->ColorVertexAttribute    = new vesColorVertexAttribute();

  this->Internal->ShaderProgram->addUniform(this->Internal->ModelViewUniform);
  this->Internal->ShaderProgram->addUniform(this->Internal->ProjectionUnifom);
  this->Internal->ShaderProgram->addUniform(this->Internal->NormalMatrixUniform);

  this->Internal->ShaderProgram->addVertexAttribute(this->Internal->PositionVertexAttribute, 0);
  this->Internal->ShaderProgram->addVertexAttribute(this->Internal->NormalVertexAttribute, 1);
  this->Internal->ShaderProgram->addVertexAttribute(this->Internal->ColorVertexAttribute, 3);

  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initializeRendering()
{
  assert(this->Internal->ShaderProgram);

  this->Internal->Renderer = new vesRenderer();

  return true;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::removeAllDataRepresentations()
{
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    this->Internal->DataRepresentations[i]->removeSelfFromRenderer(this->Internal->Renderer);
    delete this->Internal->DataRepresentations[i];
  }

  this->Internal->DataRepresentations.clear();
}

//----------------------------------------------------------------------------
vesKiwiDataRepresentation* vesKiwiViewerApp::addNewDataRepresentation(vtkDataSet* dataSet)
{
  vesKiwiDataRepresentation* rep = new vesKiwiDataRepresentation();
  rep->initializeWithShader(this->Internal->ShaderProgram);
  rep->setDataSet(dataSet);
  rep->addSelfToRenderer(this->Internal->Renderer);
  this->Internal->DataRepresentations.push_back(rep);
  return rep;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadDataset(const std::string& filename)
{
  vtkSmartPointer<vtkDataSet> dataSet = this->Internal->DataLoader.loadDataset(filename);
  if (!dataSet) {
    return false;
  }

  this->removeAllDataRepresentations();
  this->addNewDataRepresentation(dataSet);
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
  int count = 0;
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    count += static_cast<int>(this->Internal->DataRepresentations[i]->triangleData()->GetTriangles().size());
  }
  return count;
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfModelVertices() const
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    count += static_cast<int>(this->Internal->DataRepresentations[i]->triangleData()->GetPoints().size());
  }
  return count;

}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfModelLines() const
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    count += static_cast<int>(this->Internal->DataRepresentations[i]->triangleData()->GetLines().size());
  }
  return count;
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::viewWidth() const
{
  return this->Internal->Renderer->width();
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::viewHeight() const
{
  return this->Internal->Renderer->height();
}
