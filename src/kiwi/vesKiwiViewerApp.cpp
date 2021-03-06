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
#include "vesKiwiImagePlaneDataRepresentation.h"
#include "vesKiwiImageWidgetRepresentation.h"
#include "vesKiwiAnimationRepresentation.h"
#include "vesKiwiBrainAtlasRepresentation.h"
#include "vesKiwiText2DRepresentation.h"
#include "vesKiwiPlaneWidget.h"
#include "vesKiwiPolyDataRepresentation.h"

#include "vesCamera.h"
#include "vesColorUniform.h"
#include "vesMath.h"
#include "vesModelViewUniform.h"
#include "vesNormalMatrixUniform.h"
#include "vesProjectionUniform.h"
#include "vesRenderer.h"
#include "vesActor.h"
#include "vesShader.h"
#include "vesShaderProgram.h"
#include "vesTexture.h"
#include "vesUniform.h"
#include "vesVertexAttribute.h"
#include "vesVertexAttributeKeys.h"
#include "vesBuiltinShaders.h"

#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkCellLocator.h>


#include <vtksys/SystemTools.hxx>

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
  }

  ~vesInternal()
  {
    this->DataRepresentations.clear();
    this->BuiltinDatasetNames.clear();
    this->BuiltinDatasetFilenames.clear();
    this->BuiltinShadingModels.clear();
  }

  struct vesShaderProgramData
  {
    vesShaderProgramData(
      const std::string &name, vesSharedPtr<vesShaderProgram> shaderProgram) :
      Name(name), ShaderProgram(shaderProgram)
    {
    }

    std::string Name;
    vesSharedPtr<vesShaderProgram> ShaderProgram;
  };

  bool setShaderProgramOnRepresentations(
    vesSharedPtr<vesShaderProgram> shaderProgram);

  vesSharedPtr<vesShaderProgram> ShaderProgram;
  vesSharedPtr<vesShaderProgram> TextureShader;
  vesSharedPtr<vesShaderProgram> GouraudTextureShader;
  vesSharedPtr<vesShaderProgram> ClipShader;
  vesSharedPtr<vesUniform> ClipUniform;

  std::vector<vesKiwiDataRepresentation*> DataRepresentations;

  vesKiwiDataLoader DataLoader;

  std::vector<std::string> BuiltinDatasetNames;
  std::vector<std::string> BuiltinDatasetFilenames;

  std::string CurrentShadingModel;
  std::vector<vesShaderProgramData> BuiltinShadingModels;
};

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::vesInternal::setShaderProgramOnRepresentations(
  vesSharedPtr<vesShaderProgram> shaderProgram)
{
  bool success = false;

  for (size_t i = 0; i < this->DataRepresentations.size(); ++i) {
    vesKiwiPolyDataRepresentation *polyDataRepresentation =
        dynamic_cast<vesKiwiPolyDataRepresentation*>(this->DataRepresentations[i]);

    vesKiwiImageWidgetRepresentation *imageWidgetRepresentation =
        dynamic_cast<vesKiwiImageWidgetRepresentation*>(this->DataRepresentations[i]);

    if (polyDataRepresentation) {
      polyDataRepresentation->setShaderProgram(shaderProgram);
      success = true;
    }
    else if (imageWidgetRepresentation) {
      imageWidgetRepresentation->setShaderProgram(shaderProgram);
      success = true;
    }
  }

  return success;
}

//----------------------------------------------------------------------------
vesKiwiViewerApp::vesKiwiViewerApp()
{
  this->Internal = new vesInternal();
  this->setDefaultBackgroundColor();

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
  this->addBuiltinDataset("Head", "head.vti");
  this->addBuiltinDataset("KiwiViewer Logo", "kiwi.png");
  this->addBuiltinDataset("ROS Turtle", "cturtle.vtk");

  // These depend on external data, so are commented out for now.
  //this->addBuiltinDataset("SPL-PNL Brain Atlas", "model_info.txt");
  //this->addBuiltinDataset("Can Simulation", "can0000.vtp");

  this->initBlinnPhongShader(
    vesBuiltinShaders::vesBlinnPhong_vert(),
    vesBuiltinShaders::vesBlinnPhong_frag());
  this->initToonShader(
    vesBuiltinShaders::vesToonShader_vert(),
    vesBuiltinShaders::vesToonShader_frag());
  this->initGouraudShader(
    vesBuiltinShaders::vesShader_vert(),
    vesBuiltinShaders::vesShader_frag());
  this->initGouraudTextureShader(
    vesBuiltinShaders::vesGouraudTexture_vert(),
    vesBuiltinShaders::vesGouraudTexture_frag());
  this->initTextureShader(
    vesBuiltinShaders::vesBackgroundTexture_vert(),
    vesBuiltinShaders::vesBackgroundTexture_frag());
  this->initClipShader(
    vesBuiltinShaders::vesClipPlane_vert(),
    vesBuiltinShaders::vesClipPlane_frag());

  this->setShadingModel("Gouraud");
}

//----------------------------------------------------------------------------
vesKiwiViewerApp::~vesKiwiViewerApp()
{
  this->removeAllDataRepresentations();
  delete this->Internal;
}

//----------------------------------------------------------------------------
const vesSharedPtr<vesShaderProgram> vesKiwiViewerApp::shaderProgram() const
{
  return this->Internal->ShaderProgram;
}

//----------------------------------------------------------------------------
vesSharedPtr<vesShaderProgram> vesKiwiViewerApp::shaderProgram()
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
void vesKiwiViewerApp::addBuiltinShadingModel(
  const std::string &name, vesSharedPtr<vesShaderProgram> shaderProgram)
{
  assert(shaderProgram);

  if (!shaderProgram) {
    return;
  }

  for(size_t i=0; i < this->Internal->BuiltinShadingModels.size(); ++i) {
    if (this->Internal->BuiltinShadingModels[i].Name == name) {
      this->deleteShaderProgram(this->Internal->BuiltinShadingModels[i].ShaderProgram);
      this->Internal->BuiltinShadingModels[i].ShaderProgram = shaderProgram;
      return;
    }
  }

  this->Internal->BuiltinShadingModels.push_back(
    vesInternal::vesShaderProgramData(name, shaderProgram));
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::willRender()
{
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    this->Internal->DataRepresentations[i]->willRender(this->renderer());
  }
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleSingleTouchPanGesture(double deltaX, double deltaY)
{
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    vesKiwiWidgetRepresentation* rep = dynamic_cast<vesKiwiWidgetRepresentation*>(this->Internal->DataRepresentations[i]);
    if (rep) {
      if (rep->handleSingleTouchPanGesture(deltaX, deltaY)) {
        return;
      }
    }
  }

  this->Superclass::handleSingleTouchPanGesture(deltaX, deltaY);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleDoubleTap()
{
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    vesKiwiWidgetRepresentation* rep = dynamic_cast<vesKiwiWidgetRepresentation*>(this->Internal->DataRepresentations[i]);
    if (rep) {
      if (rep->handleDoubleTap()) {
        return;
      }
    }
  }
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleSingleTouchUp()
{
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    vesKiwiWidgetRepresentation* rep = dynamic_cast<vesKiwiWidgetRepresentation*>(this->Internal->DataRepresentations[i]);
    if (rep) {
      if (rep->handleSingleTouchUp()) {
        return;
      }
    }
  }
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleSingleTouchTap(int displayX, int displayY)
{
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    vesKiwiWidgetRepresentation* rep = dynamic_cast<vesKiwiWidgetRepresentation*>(this->Internal->DataRepresentations[i]);
    if (rep) {
      if (rep->handleSingleTouchTap(displayX, displayY)) {
        return;
      }
    }
  }
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleSingleTouchDown(int displayX, int displayY)
{
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    vesKiwiWidgetRepresentation* rep = dynamic_cast<vesKiwiWidgetRepresentation*>(this->Internal->DataRepresentations[i]);
    if (rep) {
      if (rep->handleSingleTouchDown(displayX, displayY)) {
        return;
      }
    }
  }
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::widgetInteractionIsActive() const
{
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    vesKiwiWidgetRepresentation* rep = dynamic_cast<vesKiwiWidgetRepresentation*>(this->Internal->DataRepresentations[i]);
    if (rep) {
      if (rep->interactionIsActive()) {
        return true;
      }
    }
  }
  return false;
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

  return this->Internal->BuiltinShadingModels[index].Name;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::setShadingModel(const std::string& name)
{
  bool success = true;

  for(size_t i=0; i < this->Internal->BuiltinShadingModels.size(); ++i) {
    // Check if we have a match.
    if (this->Internal->BuiltinShadingModels[i].Name == name) {
      // Check if we have a valid shader program before we switch.
      if (this->Internal->BuiltinShadingModels[i].ShaderProgram) {
        this->Internal->ShaderProgram =
          this->Internal->BuiltinShadingModels[i].ShaderProgram;

        return this->Internal->setShaderProgramOnRepresentations(
          this->Internal->ShaderProgram);
      }
    }
  }

  return !success;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initGouraudShader(const std::string& vertexSource, const std::string& fragmentSource)
{
  vesSharedPtr<vesShaderProgram> shaderProgram
    = this->addShaderProgram(vertexSource, fragmentSource);
  this->addModelViewMatrixUniform(shaderProgram);
  this->addProjectionMatrixUniform(shaderProgram);
  this->addNormalMatrixUniform(shaderProgram);
  this->addVertexPositionAttribute(shaderProgram);
  this->addVertexNormalAttribute(shaderProgram);
  this->addVertexColorAttribute(shaderProgram);
  this->Internal->ShaderProgram = shaderProgram;

  this->addBuiltinShadingModel("Gouraud", shaderProgram);

  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initBlinnPhongShader(const std::string& vertexSource,
                                            const std::string& fragmentSource)
{
  vesSharedPtr<vesShaderProgram> shaderProgram
    = this->addShaderProgram(vertexSource, fragmentSource);
  this->addModelViewMatrixUniform(shaderProgram);
  this->addProjectionMatrixUniform(shaderProgram);
  this->addNormalMatrixUniform(shaderProgram);
  this->addVertexPositionAttribute(shaderProgram);
  this->addVertexNormalAttribute(shaderProgram);
  this->addVertexColorAttribute(shaderProgram);
  this->Internal->ShaderProgram = shaderProgram;

  this->addBuiltinShadingModel("BlinnPhong", shaderProgram);

  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initToonShader(const std::string& vertexSource,
                                      const std::string& fragmentSource)
{
  vesSharedPtr<vesShaderProgram> shaderProgram
    = this->addShaderProgram(vertexSource, fragmentSource);
  this->addModelViewMatrixUniform(shaderProgram);
  this->addProjectionMatrixUniform(shaderProgram);
  this->addNormalMatrixUniform(shaderProgram);
  this->addVertexPositionAttribute(shaderProgram);
  this->addVertexNormalAttribute(shaderProgram);

  this->Internal->ShaderProgram = shaderProgram;

  this->addBuiltinShadingModel("Toon", shaderProgram);

  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initTextureShader(const std::string& vertexSource,
                                         const std::string& fragmentSource)
{
  vesSharedPtr<vesShaderProgram> shaderProgram
    = this->addShaderProgram(vertexSource, fragmentSource);
  this->addModelViewMatrixUniform(shaderProgram);
  this->addProjectionMatrixUniform(shaderProgram);
  this->addVertexPositionAttribute(shaderProgram);
  this->addVertexTextureCoordinateAttribute(shaderProgram);
  this->Internal->TextureShader = shaderProgram;
  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initGouraudTextureShader(const std::string& vertexSource, const std::string& fragmentSource)
{
  vesShaderProgram::Ptr shaderProgram = this->addShaderProgram(vertexSource, fragmentSource);
  this->addModelViewMatrixUniform(shaderProgram);
  this->addProjectionMatrixUniform(shaderProgram);
  this->addNormalMatrixUniform(shaderProgram);
  this->addVertexPositionAttribute(shaderProgram);
  this->addVertexNormalAttribute(shaderProgram);
  this->addVertexTextureCoordinateAttribute(shaderProgram);
  this->Internal->GouraudTextureShader = shaderProgram;
  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initClipShader(const std::string& vertexSource, const std::string& fragmentSource)
{
  vesShaderProgram::Ptr shaderProgram = this->addShaderProgram(vertexSource, fragmentSource);
  this->addModelViewMatrixUniform(shaderProgram);
  this->addProjectionMatrixUniform(shaderProgram);
  this->addNormalMatrixUniform(shaderProgram);
  this->addVertexPositionAttribute(shaderProgram);
  this->addVertexNormalAttribute(shaderProgram);
  this->addVertexColorAttribute(shaderProgram);
  this->addVertexTextureCoordinateAttribute(shaderProgram);
  this->Internal->ClipShader = shaderProgram;

  this->Internal->ClipUniform = vesUniform::Ptr(new vesUniform("clipPlaneEquation", vesVector4f(1.0f, 0.0f, 0.0f, 0.0f)));
  this->Internal->ClipShader->addUniform(this->Internal->ClipUniform);
  return true;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::removeAllDataRepresentations()
{
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    vesKiwiDataRepresentation* rep = this->Internal->DataRepresentations[i];
    rep->removeSelfFromRenderer(this->renderer());
    delete rep;
  }
  this->Internal->DataRepresentations.clear();
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::addRepresentationsForDataSet(vtkDataSet* dataSet)
{

  if (vtkPolyData::SafeDownCast(dataSet)) {
    this->addPolyDataRepresentation(vtkPolyData::SafeDownCast(dataSet), this->shaderProgram());
  }
  else if (vtkImageData::SafeDownCast(dataSet)) {

    vtkImageData* image = vtkImageData::SafeDownCast(dataSet);

    if (image->GetDataDimension() == 3) {

      vesKiwiImageWidgetRepresentation* rep = new vesKiwiImageWidgetRepresentation();
      rep->initializeWithShader(this->shaderProgram(), this->Internal->TextureShader);
      rep->setImageData(image);
      rep->addSelfToRenderer(this->renderer());
      this->Internal->DataRepresentations.push_back(rep);

    }
    else {

      vesKiwiImagePlaneDataRepresentation* rep = new vesKiwiImagePlaneDataRepresentation();
      rep->initializeWithShader(this->Internal->TextureShader);
      rep->setImageData(image);
      rep->addSelfToRenderer(this->renderer());
      this->Internal->DataRepresentations.push_back(rep);

    }
  }
}

//----------------------------------------------------------------------------
vesKiwiPolyDataRepresentation* vesKiwiViewerApp::addPolyDataRepresentation(
  vtkPolyData* polyData, vesSharedPtr<vesShaderProgram> program)
{
  vesKiwiPolyDataRepresentation* rep = new vesKiwiPolyDataRepresentation();
  rep->initializeWithShader(program);
  rep->setPolyData(polyData);
  rep->addSelfToRenderer(this->renderer());
  this->Internal->DataRepresentations.push_back(rep);
  return rep;
}

//----------------------------------------------------------------------------
vesKiwiText2DRepresentation* vesKiwiViewerApp::addTextRepresentation(const std::string& text)
{
  vesKiwiText2DRepresentation* rep = new vesKiwiText2DRepresentation();
  rep->initializeWithShader(this->Internal->TextureShader);
  rep->setText(text);
  rep->addSelfToRenderer(this->renderer());
  this->Internal->DataRepresentations.push_back(rep);
  return rep;
}

//----------------------------------------------------------------------------
vesKiwiPlaneWidget* vesKiwiViewerApp::addPlaneWidget()
{
  vesKiwiPlaneWidget* rep = new vesKiwiPlaneWidget();
  rep->initializeWithShader(this->shaderProgram(), this->Internal->ClipUniform);
  rep->addSelfToRenderer(this->renderer());
  this->Internal->DataRepresentations.push_back(rep);
  return rep;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setBackgroundTexture(const std::string& filename)
{
  vesNotUsed(filename);
  //vtkSmartPointer<vtkImageData> image =
  // vtkImageData::SafeDownCast(this->Internal->DataLoader.loadDataset(filename));
  //vesTexture* backgroundTexture = this->newTextureFromImage(image);
  //this->renderer()->SetBackground(backgroundTexture);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setDefaultBackgroundColor()
{
  this->setBackgroundColor(63/255.0, 96/255.0, 144/255.0);
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadBrainAtlas(const std::string& filename)
{
  vesKiwiBrainAtlasRepresentation* rep = new vesKiwiBrainAtlasRepresentation();
  rep->initializeWithShader(this->shaderProgram(), this->Internal->TextureShader, this->Internal->ClipShader);
  rep->loadData(filename);
  rep->addSelfToRenderer(this->renderer());
  this->Internal->DataRepresentations.push_back(rep);

  //std::string dataDir = vtksys::SystemTools::GetFilenamePath(filename);
  //vtkSmartPointer<vtkDataSet> imageData = this->Internal->DataLoader.loadDataset(dataDir + "/brain_atlas_mri.mha");
  //if (imageData) {
  //  this->addRepresentationsForDataSet(imageData);
  //}

  this->addPlaneWidget();
  this->setBackgroundColor(0.0, 0.0, 0.0);
  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadCanSimulation(const std::string& filename)
{
  vesKiwiAnimationRepresentation* rep = new vesKiwiAnimationRepresentation();
  rep->initializeWithShader(this->shaderProgram(), this->Internal->TextureShader, this->Internal->GouraudTextureShader);
  rep->loadData(filename);
  rep->addSelfToRenderer(this->renderer());
  this->Internal->DataRepresentations.push_back(rep);
  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadDatasetWithCustomBehavior(const std::string& filename)
{
  // These demos are currently disabled because they depend on external data 
  //if (vtksys::SystemTools::GetFilenameName(filename) == "model_info.txt") {
  //  return loadBrainAtlas(filename);
  //}
  //else if (vtksys::SystemTools::GetFilenameName(filename) == "can0000.vtp") {
  //  return loadCanSimulation(filename);
  //}

  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadDataset(const std::string& filename)
{
  this->removeAllDataRepresentations();
  this->setDefaultBackgroundColor();

  // this is a hook that can be used to load certain datasets using custom logic
  if (this->loadDatasetWithCustomBehavior(filename)) {
    return true;
  }

  vtkSmartPointer<vtkDataSet> dataSet = this->Internal->DataLoader.loadDataset(filename);
  if (!dataSet) {
    return false;
  }

  this->addRepresentationsForDataSet(dataSet);
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
    count += this->Internal->DataRepresentations[i]->numberOfFacets();
  }
  return count;
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfModelVertices() const
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    count += this->Internal->DataRepresentations[i]->numberOfVertices();
  }
  return count;
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfModelLines() const
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    count += this->Internal->DataRepresentations[i]->numberOfLines();
  }
  return count;
}
