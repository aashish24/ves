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
#include "vesKiwiCameraSpinner.h"
#include "vesKiwiDataConversionTools.h"
#include "vesKiwiDataLoader.h"
#include "vesKiwiDataRepresentation.h"
#include "vesKiwiImagePlaneDataRepresentation.h"
#include "vesKiwiImageWidgetRepresentation.h"
#include "vesKiwiAnimationRepresentation.h"
#include "vesKiwiBrainAtlasRepresentation.h"
#include "vesKiwiText2DRepresentation.h"
#include "vesKiwiPlaneWidget.h"
#include "vesKiwiPolyDataRepresentation.h"
#include "vesKiwiSceneRepresentation.h"
#include "vesKiwiWidgetInteractionDelegate.h"

#include "vesBackground.h"
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
#include "vesOpenGLSupport.h"
#include "vesBuiltinShaders.h"

#include "vesKiwiOptions.h"
#ifdef VES_USE_CURL
#  include "vesKiwiCurlDownloader.h"
#  include "vesKiwiPVRemoteRepresentation.h"
#  include "vesPVWebClient.h"
#  include "vesPVWebDataSet.h"
#endif
#ifdef VES_USE_LIBARCHIVE
#  include "vesKiwiArchiveUtils.h"
#endif


#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>


#include <vtksys/SystemTools.hxx>

#include <cassert>
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>

using vesDynamicPtrCast;

//----------------------------------------------------------------------------
class vesKiwiViewerApp::vesInternal
{
public:

  vesInternal()
  {
    this->IsAnimating = false;
    this->CameraRotationInertiaIsEnabled = true;
    this->CameraSpinner = vesKiwiCameraSpinner::Ptr(new vesKiwiCameraSpinner);
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

  struct vesCameraParameters
  {
    vesCameraParameters()
    {
      this->setParameters(vesVector3f(0.0, 0.0, -1.0), vesVector3f(0.0, 1.0, 0.0));
    }

    void setParameters(const vesVector3f& viewDirection, const vesVector3f& viewUp)
    {
      this->ViewDirection = viewDirection;
      this->ViewUp = viewUp;
    }

    vesVector3f ViewDirection;
    vesVector3f ViewUp;
  };

  bool IsAnimating;
  bool CameraRotationInertiaIsEnabled;
  std::string ErrorTitle;
  std::string ErrorMessage;

  vesSharedPtr<vesShaderProgram> ShaderProgram;
  vesSharedPtr<vesShaderProgram> TextureShader;
  vesSharedPtr<vesShaderProgram> GouraudTextureShader;
  vesSharedPtr<vesShaderProgram> ClipShader;
  vesSharedPtr<vesShaderProgram> WireframeShader;
  vesSharedPtr<vesShaderProgram> SurfaceWithEdgesShader;
  vesSharedPtr<vesUniform> ClipUniform;

  std::vector<vesKiwiDataRepresentation::Ptr> DataRepresentations;

  vesKiwiCameraSpinner::Ptr CameraSpinner;
  vesKiwiDataLoader DataLoader;

  std::vector<std::string> BuiltinDatasetNames;
  std::vector<std::string> BuiltinDatasetFilenames;
  std::vector<vesCameraParameters> BuiltinDatasetCameraParameters;

  vesCameraParameters DefaultCameraParameters;

  std::vector<vesShaderProgramData> BuiltinShadingModels;
};

//----------------------------------------------------------------------------
vesKiwiViewerApp::vesKiwiViewerApp()
{
  this->Internal = new vesInternal();
  this->Internal->CameraSpinner->setInteractor(this->cameraInteractor());
  this->resetScene();

  this->addBuiltinDataset("Utah Teapot", "teapot.vtp");
  this->addBuiltinDataset("Stanford Bunny", "bunny.vtp");
  this->addBuiltinDataset("NLM Visible Woman Hand", "visible-woman-hand.vtp");
  this->addBuiltinDataset("NA-MIC Knee Atlas", "AppendedKneeData.vtp");

  this->addBuiltinDataset("ROS C Turtle", "cturtle.vtp");
  this->Internal->BuiltinDatasetCameraParameters.back().setParameters(
    vesVector3f(0.,0.,1.), vesVector3f(0.,-1.,0.));

  this->addBuiltinDataset("Mount St. Helens", "MountStHelen.vtp");
  this->addBuiltinDataset("Space Shuttle", "shuttle.vtp");

  //http://visibleearth.nasa.gov/view.php?id=57730
  this->addBuiltinDataset("NASA Blue Marble", "nasa-blue-marble.kiwi");
  this->Internal->BuiltinDatasetCameraParameters.back().setParameters(
    vesVector3f(1.,0.,0.), vesVector3f(0.,0.,1.));

  this->addBuiltinDataset("Buckyball", "Buckyball.vtp");
  this->addBuiltinDataset("Caffeine", "caffeine.pdb");

  this->addBuiltinDataset("Head CT Image", "head.vti");
  this->Internal->BuiltinDatasetCameraParameters.back().setParameters(
    vesVector3f(1.,0.,0.), vesVector3f(0.,0.,-1.));

  this->addBuiltinDataset("KiwiViewer Logo", "kiwi.png");


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
  this->initWireframeShader(
    vesBuiltinShaders::vesWireframeShader_vert(),
    vesBuiltinShaders::vesWireframeShader_frag());
  this->initSurfaceWithEdgesShader(
    vesBuiltinShaders::vesWireframeShader_vert(),
    vesBuiltinShaders::vesWireframeShader_frag());
  this->initTextureShader(
    vesBuiltinShaders::vesBackgroundTexture_vert(),
    vesBuiltinShaders::vesBackgroundTexture_frag());
  this->initClipShader(
    vesBuiltinShaders::vesClipPlane_vert(),
    vesBuiltinShaders::vesClipPlane_frag());
}

//----------------------------------------------------------------------------
vesKiwiViewerApp::~vesKiwiViewerApp()
{
  this->removeAllDataRepresentations();
  delete this->Internal;
}


//----------------------------------------------------------------------------
void vesKiwiViewerApp::addManagedDataRepresentation(vesKiwiDataRepresentation::Ptr rep)
{
  this->Internal->DataRepresentations.push_back(rep);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::initGL()
{
  this->vesKiwiBaseApp::initGL();
  this->Internal->DataLoader.setErrorOnMoreThan65kVertices(!this->glSupport()->isSupportedIndexUnsignedInt());
  #ifdef VES_USE_DESKTOP_GL
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  #endif
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::checkForPVWebError(vesSharedPtr<vesPVWebClient> client)
{
#ifdef VES_USE_CURL
  this->setErrorMessage(client->errorTitle(), client->errorMessage());
  return !client->errorMessage().empty();
#else
  this->setErrorMessage("Unsupported", "ParaViewWeb requires cURL support, which was not included.");
  return false;
#endif
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadPVWebDataSet(vesSharedPtr<vesPVWebDataSet> dataset)
{
#ifdef VES_USE_CURL
  if (!dataset)
    return false;

  if (dataset->m_layer != 0)
    return false;

  if (dataset->m_numberOfVerts == 0)
    return false;

  vesGeometryData::Ptr geometryData = vesKiwiDataConversionTools::ConvertPVWebData(dataset);
  vesKiwiPolyDataRepresentation::Ptr rep = vesKiwiPolyDataRepresentation::Ptr(new vesKiwiPolyDataRepresentation());
  rep->initializeWithShader(this->shaderProgram());
  rep->setWireframeShader(this->Internal->WireframeShader);
  rep->setSurfaceWithEdgesShader(this->Internal->SurfaceWithEdgesShader);
  rep->mapper()->setGeometryData(geometryData);
  rep->assignColorsInternal();
  if (dataset->m_transparency) {
    rep->setOpacity(0.4);
  }

  vtkNew<vtkTransform> transform;
  const float* matrix = dataset->matrix();
  double* matrixElements = (*transform->GetMatrix())[0];
  for (int i = 0; i < 16; ++i) {
    matrixElements[i] = matrix[i];
  }
  rep->setTransformOnActor(rep->actor(), transform.GetPointer());

  rep->addSelfToRenderer(this->renderer());
  this->addManagedDataRepresentation(rep);
  return true;
#else
  return false;
#endif // VES_USE_CURL
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadPVWebDataSet(const std::string& filename)
{
#ifdef VES_USE_CURL
  vesPVWebDataSet::Ptr dataset = vesPVWebDataSet::loadDataSetFromFile(filename);
  if (!dataset) {
    return false;
  }

  return this->loadPVWebDataSet(dataset);
#else
  return false;
#endif // VES_USE_CURL
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::doPVWebTest(const std::string& host, const std::string& sessionId)
{
#ifdef VES_USE_CURL
  this->resetScene();

  vesPVWebClient::Ptr client(new vesPVWebClient);
  client->setHost(host);

  if (sessionId.empty()) {

    if (!client->createVisualization()) {
      this->checkForPVWebError(client);
      return false;
    }

    client->configureOff();

    if (client->createView()) {
      client->executeCommand("Sphere");
      client->executeCommand("Show");
      client->executeCommand("Render");
      client->executeCommand("ResetCamera");
      client->configureOn();
      client->executeCommand("Render");
      client->pollSceneMetaData();
      client->downloadObjects();
    }

    client->endVisualization();

  }
  else {

    client->setSessionId(sessionId);

    if (client->createView()) {
      client->configureOn();
      client->executeCommand("Render");
      client->executeCommand("GetProxy");
      client->pollSceneMetaData();
      client->downloadObjects();
    }
  }

  if (this->checkForPVWebError(client)) {
    return false;
  }

  for (size_t i = 0; i < client->datasets().size(); ++i) {
    vesPVWebDataSet::Ptr dataset = client->datasets()[i];
    this->loadPVWebDataSet(dataset);
  }

  this->resetView();

  return true;
#else
  return false;
#endif // VES_USE_CURL
}

//----------------------------------------------------------------------------
vesSharedPtr<vesKiwiPVRemoteRepresentation> vesKiwiViewerApp::pvRemoteRep()
{
#ifdef VES_USE_CURL
  if (this->Internal->DataRepresentations.size()) {
    return dynamic_pointer_cast<vesKiwiPVRemoteRepresentation>(this->Internal->DataRepresentations[0]);
  }
#endif // VES_USE_CURL
  return vesSharedPtr<vesKiwiPVRemoteRepresentation>();
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::doPVRemote(const std::string& host, int port)
{
#ifdef VES_USE_CURL
  this->resetScene();

  vesKiwiPVRemoteRepresentation::Ptr rep = vesKiwiPVRemoteRepresentation::Ptr(new vesKiwiPVRemoteRepresentation);
  if (!rep->connectToServer(host, port)) {

    std::stringstream errorMessage;
    errorMessage << "Failed to connect to " << host << ":" << port;
    this->setErrorMessage("Connection failed", errorMessage.str());
    return false;
  }

  rep->initializeWithShader(this->shaderProgram());
  rep->addSelfToRenderer(this->renderer());

  this->Internal->DataRepresentations.push_back(rep);

  this->setBackgroundColor(0.0, 0.0, 0.0);
  this->resetView();

  return true;
#else
  return false;
#endif // VES_USE_CURL
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::downloadFile(const std::string& url, const std::string& downloadDir)
{
#ifdef VES_USE_CURL
  vesKiwiCurlDownloader downloader;
  std::string result = downloader.downloadUrlToDirectory(url, downloadDir);
  if (!result.size()) {
    this->setErrorMessage(downloader.errorTitle(), downloader.errorMessage());
  }

  return result;
#else
  this->setErrorMessage("Unsupported", "Kiwi was not built with cURL support.");
  return this->Internal->ErrorMessage;
#endif // VES_USE_CURL
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
const std::vector<vesKiwiDataRepresentation::Ptr>& vesKiwiViewerApp::dataRepresentations() const
{
  return this->Internal->DataRepresentations;
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfBuiltinDatasets() const
{
  return static_cast<int>(this->Internal->BuiltinDatasetNames.size());
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::defaultBuiltinDatasetIndex() const
{
  return 0;
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
  this->Internal->BuiltinDatasetCameraParameters.push_back(vesInternal::vesCameraParameters());
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::applyBuiltinDatasetCameraParameters(
  int index, bool withTransition)
{
  this->Internal->CameraSpinner->disable();
  this->Superclass::resetView(
    this->Internal->BuiltinDatasetCameraParameters[index].ViewDirection,
    this->Internal->BuiltinDatasetCameraParameters[index].ViewUp,
    withTransition);
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
bool vesKiwiViewerApp::isAnimating() const
{
  return this->Internal->IsAnimating;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setAnimating(bool animating)
{
  this->Internal->IsAnimating = animating;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::willRender()
{
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    this->Internal->DataRepresentations[i]->willRender(this->renderer());
  }
  this->Internal->CameraSpinner->updateSpin();
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleSingleTouchPanGesture(double deltaX, double deltaY)
{
  this->Internal->CameraSpinner->disable();
  this->Internal->CameraSpinner->handlePanGesture(vesVector2d(deltaX, deltaY));

  if (vesKiwiWidgetInteractionDelegate::handleSingleTouchPanGesture(deltaX, deltaY, this->Internal->DataRepresentations)) {
    return;
  }

  this->Superclass::handleSingleTouchPanGesture(deltaX, deltaY);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleSingleTouchUp()
{

  if (!this->widgetInteractionIsActive()
      && this->Internal->CameraRotationInertiaIsEnabled
      && this->Internal->CameraSpinner->currentMagnitude() > 0.0) {
    this->Internal->CameraSpinner->enable();
  }
  else {
    this->Internal->CameraSpinner->disable();
  }

  vesKiwiWidgetInteractionDelegate::handleSingleTouchUp(this->Internal->DataRepresentations);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleSingleTouchTap(int displayX, int displayY)
{
  this->Internal->CameraSpinner->disable();
  vesKiwiWidgetInteractionDelegate::handleSingleTouchTap(displayX, displayY, this->Internal->DataRepresentations);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleSingleTouchDown(int displayX, int displayY)
{
  this->Internal->CameraSpinner->disable();
  vesKiwiWidgetInteractionDelegate::handleSingleTouchDown(displayX, displayY, this->Internal->DataRepresentations);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleTwoTouchPanGesture(double x0, double y0, double x1, double y1)
{
  this->Internal->CameraSpinner->disable();

  if (vesKiwiWidgetInteractionDelegate::handleTwoTouchPanGesture(x0, y0, x1, y1, this->Internal->DataRepresentations)) {
    return;
  }

  this->Superclass::handleTwoTouchPanGesture(x0, y0, x1, y1);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleTwoTouchPinchGesture(double scale)
{
  this->Internal->CameraSpinner->disable();

  if (vesKiwiWidgetInteractionDelegate::handleTwoTouchPinchGesture(scale, this->Internal->DataRepresentations)) {
    return;
  }

  this->Superclass::handleTwoTouchPinchGesture(scale);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleTwoTouchRotationGesture(double rotation)
{
  this->Internal->CameraSpinner->disable();

  if (vesKiwiWidgetInteractionDelegate::handleTwoTouchRotationGesture(rotation, this->Internal->DataRepresentations)) {
    return;
  }

  this->Superclass::handleTwoTouchRotationGesture(rotation);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleDoubleTap(int displayX, int displayY)
{
  this->Internal->CameraSpinner->disable();
  vesKiwiWidgetInteractionDelegate::handleDoubleTap(displayX, displayY, this->Internal->DataRepresentations);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleLongPress(int displayX, int displayY)
{
  this->Internal->CameraSpinner->disable();
  vesKiwiWidgetInteractionDelegate::handleLongPress(displayX, displayY, this->Internal->DataRepresentations);
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::widgetInteractionIsActive() const
{
  return vesKiwiWidgetInteractionDelegate::interactionIsActive(this->Internal->DataRepresentations);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::resetView(bool withTransition)
{
  this->Internal->CameraSpinner->disable();

  //std::cout << "view direction:" << this->Internal->DefaultCameraParameters.ViewDirection << std::endl;
  //std::cout << "view up:" << this->Internal->DefaultCameraParameters.ViewUp << std::endl;

  //this->camera()->setPosition(vesVector3f(0,0,-10));
  //this->camera()->setFocalPoint(vesVector3f(0,0,0));
  //this->camera()->setViewUp(vesVector3f(0,1,0));

  this->Superclass::resetView(this->Internal->DefaultCameraParameters.ViewDirection,
                              this->Internal->DefaultCameraParameters.ViewUp, withTransition);
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::getNumberOfShadingModels() const
{
  return static_cast<int>(this->Internal->BuiltinShadingModels.size());
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
vesSharedPtr<vesShaderProgram> vesKiwiViewerApp::shaderProgram(const std::string& name) const
{
  for(size_t i=0; i < this->Internal->BuiltinShadingModels.size(); ++i) {
    if (this->Internal->BuiltinShadingModels[i].Name == name) {
      return this->Internal->BuiltinShadingModels[i].ShaderProgram;
    }
  }
  return vesShaderProgram::Ptr();
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
bool vesKiwiViewerApp::initWireframeShader(const std::string& vertexSource, const std::string& fragmentSource)
{
  vesSharedPtr<vesShaderProgram> shaderProgram
    = this->addShaderProgram(vertexSource, fragmentSource);
  this->addModelViewMatrixUniform(shaderProgram);
  this->addProjectionMatrixUniform(shaderProgram);
  this->addNormalMatrixUniform(shaderProgram);
  this->addVertexPositionAttribute(shaderProgram);
  this->addVertexNormalAttribute(shaderProgram);
  this->addVertexColorAttribute(shaderProgram);
  this->Internal->WireframeShader = shaderProgram;

  vesVertexAttribute::Ptr p1Attribute = vesGenericVertexAttribute::Ptr(new vesGenericVertexAttribute("tri_p1"));
  vesVertexAttribute::Ptr p2Attribute = vesGenericVertexAttribute::Ptr(new vesGenericVertexAttribute("tri_p2"));
  vesVertexAttribute::Ptr indsAttribute = vesGenericVertexAttribute::Ptr(new vesGenericVertexAttribute("tri_point_index"));
  shaderProgram->addVertexAttribute(p1Attribute, 10);
  shaderProgram->addVertexAttribute(p2Attribute, 11);
  shaderProgram->addVertexAttribute(indsAttribute, 12);

  this->addBuiltinShadingModel("Wireframe", shaderProgram);

  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initSurfaceWithEdgesShader(const std::string& vertexSource, const std::string& fragmentSource)
{
  std::string customFragmentSource = "#define SURFACE_WITH_EDGES\n" + fragmentSource;

  vesSharedPtr<vesShaderProgram> shaderProgram
    = this->addShaderProgram(vertexSource, customFragmentSource);
  this->addModelViewMatrixUniform(shaderProgram);
  this->addProjectionMatrixUniform(shaderProgram);
  this->addNormalMatrixUniform(shaderProgram);
  this->addVertexPositionAttribute(shaderProgram);
  this->addVertexNormalAttribute(shaderProgram);
  this->addVertexColorAttribute(shaderProgram);
  this->Internal->SurfaceWithEdgesShader = shaderProgram;

  vesVertexAttribute::Ptr p1Attribute = vesGenericVertexAttribute::Ptr(new vesGenericVertexAttribute("tri_p1"));
  vesVertexAttribute::Ptr p2Attribute = vesGenericVertexAttribute::Ptr(new vesGenericVertexAttribute("tri_p2"));
  vesVertexAttribute::Ptr indsAttribute = vesGenericVertexAttribute::Ptr(new vesGenericVertexAttribute("tri_point_index"));
  shaderProgram->addVertexAttribute(p1Attribute, 10);
  shaderProgram->addVertexAttribute(p2Attribute, 11);
  shaderProgram->addVertexAttribute(indsAttribute, 12);

  this->addBuiltinShadingModel("SurfaceWithEdges", shaderProgram);

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
  this->addBuiltinShadingModel("Texture", shaderProgram);
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
  this->addBuiltinShadingModel("GouraudTexture", shaderProgram);
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
void vesKiwiViewerApp::resetScene()
{
  this->resetErrorMessage();
  this->removeAllDataRepresentations();
  this->setDefaultBackgroundColor();
  this->setAnimating(false);
  this->Internal->CameraSpinner->disable();

  this->Internal->DefaultCameraParameters = vesInternal::vesCameraParameters();
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::removeAllDataRepresentations()
{
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    this->Internal->DataRepresentations[i]->removeSelfFromRenderer(this->renderer());
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

      vesKiwiImageWidgetRepresentation::Ptr rep = vesKiwiImageWidgetRepresentation::Ptr(new vesKiwiImageWidgetRepresentation());
      rep->initializeWithShader(this->shaderProgram(), this->Internal->TextureShader);
      rep->setImageData(image);
      rep->addSelfToRenderer(this->renderer());
      this->Internal->DataRepresentations.push_back(rep);

    }
    else {

      vesKiwiImagePlaneDataRepresentation::Ptr rep = vesKiwiImagePlaneDataRepresentation::Ptr(new vesKiwiImagePlaneDataRepresentation());
      rep->initializeWithShader(this->Internal->TextureShader);
      rep->setSurfaceShader(this->shaderProgram());
      rep->setTextureSurfaceShader(this->Internal->TextureShader);
      rep->setWireframeShader(this->Internal->WireframeShader);
      rep->setSurfaceWithEdgesShader(this->Internal->SurfaceWithEdgesShader);
      rep->setImageData(image);
      rep->addSelfToRenderer(this->renderer());
      this->Internal->DataRepresentations.push_back(rep);

    }
  }
}

//----------------------------------------------------------------------------
vesKiwiPolyDataRepresentation::Ptr vesKiwiViewerApp::addPolyDataRepresentation(
  vtkPolyData* polyData, vesSharedPtr<vesShaderProgram> program)
{
  vesKiwiPolyDataRepresentation::Ptr rep = vesKiwiPolyDataRepresentation::Ptr(new vesKiwiPolyDataRepresentation());
  rep->initializeWithShader(program);
  rep->setWireframeShader(this->Internal->WireframeShader);
  rep->setSurfaceWithEdgesShader(this->Internal->SurfaceWithEdgesShader);
  rep->setPolyData(polyData);
  rep->addSelfToRenderer(this->renderer());
  this->Internal->DataRepresentations.push_back(rep);
  return rep;
}

//----------------------------------------------------------------------------
vesKiwiText2DRepresentation::Ptr vesKiwiViewerApp::addTextRepresentation(const std::string& text)
{
  vesKiwiText2DRepresentation::Ptr rep = vesKiwiText2DRepresentation::Ptr(new vesKiwiText2DRepresentation());
  rep->initializeWithShader(this->Internal->TextureShader);
  rep->setText(text);
  rep->addSelfToRenderer(this->renderer());
  this->Internal->DataRepresentations.push_back(rep);
  return rep;
}

//----------------------------------------------------------------------------
vesKiwiPlaneWidget::Ptr vesKiwiViewerApp::addPlaneWidget()
{
  vesKiwiPlaneWidget::Ptr rep = vesKiwiPlaneWidget::Ptr(new vesKiwiPlaneWidget());
  rep->initializeWithShader(this->shaderProgram(), this->Internal->ClipUniform);
  rep->addSelfToRenderer(this->renderer());
  this->Internal->DataRepresentations.push_back(rep);
  return rep;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setBackgroundTexture(const std::string& filename)
{
  vtkSmartPointer<vtkImageData> vtkimage = vtkImageData::SafeDownCast(this->Internal->DataLoader.loadDataset(filename));
  if (vtkimage) {
    vesImage::Ptr image = vesKiwiDataConversionTools::ConvertImage(vtkimage);
    if (image) {
      this->renderer()->background()->setImage(image);
    }
  }
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setDefaultBackgroundColor()
{
  this->setBackgroundColor(63/255.0, 96/255.0, 144/255.0);
  //this->setBackgroundColor(0,0,0);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::checkForAdditionalData(const std::string& dirname)
{
  vesNotUsed(dirname);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setDefaultCameraParameters(vesVector3f viewDirection, vesVector3f viewUp)
{
  viewDirection.normalize();
  viewUp.normalize();
  this->Internal->DefaultCameraParameters.setParameters(viewDirection, viewUp);
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadBrainAtlas(const std::string& filename)
{
  vesKiwiBrainAtlasRepresentation::Ptr rep = vesKiwiBrainAtlasRepresentation::Ptr(new vesKiwiBrainAtlasRepresentation());
  rep->initializeWithShader(this->shaderProgram(), this->Internal->TextureShader, this->Internal->ClipShader);
  rep->loadData(filename);
  rep->addSelfToRenderer(this->renderer());
  this->Internal->DataRepresentations.push_back(rep);

  vesKiwiPlaneWidget::Ptr planeWidget = this->addPlaneWidget();
  rep->setClipPlane(planeWidget->plane());

  this->Internal->DefaultCameraParameters.setParameters(
    vesVector3f(-1,0,0),
    vesVector3f(0,0,1));

  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadTexturedMesh(const std::string& meshFile, const std::string& imageFile)
{
  vtkSmartPointer<vtkPolyData> polyData = vtkPolyData::SafeDownCast(this->Internal->DataLoader.loadDataset(meshFile));
  if (!polyData) {
    this->handleLoadDatasetError();
    return false;
  }

  vtkSmartPointer<vtkImageData> image = vtkImageData::SafeDownCast(this->Internal->DataLoader.loadDataset(imageFile));
  if (!image) {
    this->handleLoadDatasetError();
    return false;
  }

  vesKiwiPolyDataRepresentation::Ptr rep = this->addPolyDataRepresentation(polyData, this->Internal->GouraudTextureShader);
  vtkSmartPointer<vtkUnsignedCharArray> pixels = vtkUnsignedCharArray::SafeDownCast(image->GetPointData()->GetScalars());
  if (!pixels) {
    this->setErrorMessage("Error Loading File", "Could not convert image file to texture data.");
    return false;
  }

  int width = image->GetDimensions()[0];
  int height = image->GetDimensions()[1];

  vesTexture::Ptr texture = vesTexture::Ptr(new vesTexture());
  vesKiwiDataConversionTools::SetTextureData(pixels, texture, width, height);
  rep->setTexture(texture);

  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadKiwiScene(const std::string& sceneFile)
{
  vesKiwiSceneRepresentation::Ptr rep = vesKiwiSceneRepresentation::Ptr(new vesKiwiSceneRepresentation);

  rep->setShaders(this->shaderProgram(),
      this->Internal->WireframeShader,
      this->Internal->SurfaceWithEdgesShader,
      this->Internal->GouraudTextureShader,
      this->Internal->TextureShader,
      this->Internal->ClipShader);

  bool result = rep->loadScene(sceneFile, &this->Internal->DataLoader);

  if (!result) {
    this->setErrorMessage(rep->errorTitle(), rep->errorMessage());
  }

  const std::vector<vesKiwiDataRepresentation::Ptr>& sceneReps = rep->dataRepresentations();
  for (size_t i = 0; i < sceneReps.size(); ++i) {
    sceneReps[i]->addSelfToRenderer(this->renderer());
    this->addManagedDataRepresentation(sceneReps[i]);
  }

  if (rep->hasCameraSettings()) {
    this->Internal->DefaultCameraParameters.setParameters(
      rep->cameraFocalPoint() - rep->cameraPosition(),
      rep->cameraViewUp());

    this->Internal->DefaultCameraParameters.ViewDirection.normalize();
    this->Internal->DefaultCameraParameters.ViewUp.normalize();
  }

  if (rep->hasBackgroundSettings()) {
    this->renderer()->background()->setImage(vesImage::Ptr());
    if (rep->backgroundImage().empty()) {
      this->renderer()->background()->setGradientColor(rep->backgroundColor(), rep->backgroundColor2());
    }
    else {
      this->setBackgroundTexture(rep->backgroundImage());
    }
  }

  return result;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadArchive(const std::string& archiveFile)
{
  std::string baseDir = vtksys::SystemTools::GetFilenamePath(archiveFile);

#ifdef VES_USE_LIBARCHIVE
  vesKiwiArchiveUtils archiveLoader;

  bool result = archiveLoader.extractArchive(archiveFile, baseDir);
  if (!result) {
    this->setErrorMessage(archiveLoader.errorTitle(), archiveLoader.errorMessage());
    return false;
  }

  const std::vector<std::string>& entries = archiveLoader.entries();

  //printf("have %lu extracted entries\n", entries.size());

  // load .kiwi file if it exists
  for (size_t i = 0; i < entries.size(); ++i) {
    //printf("extracted entry:  %s\n", entries[i].c_str());
    if (vtksys::SystemTools::GetFilenameLastExtension(entries[i]) == ".kiwi") {
      return this->loadDataset(entries[i]);
    }
  }

  // try to load all entries that are not directories
  for (size_t i = 0; i < entries.size(); ++i) {
    if (!vtksys::SystemTools::FileIsDirectory(entries[i].c_str())) {
      this->loadDataset(entries[i]);
    }
  }
#else
  this->setErrorMessage("Unsupported", "Kiwi not built with libarchive support.");
#endif // VES_USE_LIBARCHIVE

  return this->Internal->ErrorMessage.empty();
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadDatasetWithCustomBehavior(const std::string& filename)
{
  if (vtksys::SystemTools::GetFilenameName(filename) == "spl_pnl_brain_atlas.kiwi") {
    return loadBrainAtlas(filename);
  }
  else if (vtksys::SystemTools::GetFilenameLastExtension(filename) == ".kiwi") {
    return loadKiwiScene(filename);
  }
  else if (vtksys::SystemTools::GetFilenameLastExtension(filename) == ".pvwebgl") {
    return loadPVWebDataSet(filename);
  }
  else if (vtksys::SystemTools::GetFilenameLastExtension(filename) == ".zip"
           || vtksys::SystemTools::GetFilenameLastExtension(filename) == ".gz"
           || vtksys::SystemTools::GetFilenameLastExtension(filename) == ".tgz"
           || vtksys::SystemTools::GetFilenameLastExtension(filename) == ".bz2") {
    return loadArchive(filename);
  }

  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadDataset(const std::string& filename)
{
  // this is a hook that can be used to load certain datasets using custom logic
  if (this->loadDatasetWithCustomBehavior(filename)) {
    return true;
  }
  else if (!this->Internal->ErrorMessage.empty()) {
    return false;
  }

  vtkSmartPointer<vtkDataSet> dataSet = this->Internal->DataLoader.loadDataset(filename);
  if (!dataSet) {
    this->handleLoadDatasetError();
    return false;
  }

  this->addRepresentationsForDataSet(dataSet);
  return true;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setErrorMessage(const std::string& errorTitle, const std::string& errorMessage)
{
  if (this->Internal->ErrorMessage.empty()) {
    this->Internal->ErrorTitle = errorTitle;
    this->Internal->ErrorMessage = errorMessage;
  }
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::resetErrorMessage()
{
  this->Internal->ErrorTitle.clear();
  this->Internal->ErrorMessage.clear();
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleLoadDatasetError()
{
  this->setErrorMessage(this->Internal->DataLoader.errorTitle(),
                        this->Internal->DataLoader.errorMessage());
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::loadDatasetErrorTitle() const
{
  return this->Internal->ErrorTitle;
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::loadDatasetErrorMessage() const
{
  return this->Internal->ErrorMessage;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setCameraRotationInertiaIsEnabled(bool enabled)
{
  this->Internal->CameraRotationInertiaIsEnabled = enabled;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::cameraRotationInertiaIsEnabled() const
{
  return this->Internal->CameraRotationInertiaIsEnabled;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::haltCameraRotationInertia()
{
  this->Internal->CameraSpinner->disable();
}

//----------------------------------------------------------------------------
vesKiwiCameraSpinner::Ptr vesKiwiViewerApp::cameraSpinner() const
{
  return this->Internal->CameraSpinner;
}

namespace {

// Return a vector containing the geometry data for every
// visible actor in the renderer scene.
std::vector<vesGeometryData::Ptr> collectGeometryData(vesRenderer::Ptr renderer)
{
  std::vector<vesGeometryData::Ptr> geometryData;
  std::vector<vesSharedPtr<vesActor> > actors = renderer->sceneActors();
  for (size_t i = 0; i < actors.size(); ++i) {
    if (actors[i]->isVisible() && actors[i]->mapper() && actors[i]->mapper()->geometryData()) {
      geometryData.push_back(actors[i]->mapper()->geometryData());
    }
  }
  return geometryData;
}

}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfModelFacets() const
{
  int count = 0;
  std::vector<vesGeometryData::Ptr> geometryData = collectGeometryData(this->renderer());

  for (size_t i = 0; i < geometryData.size(); ++i) {
    vesPrimitive::Ptr tris = geometryData[i]->triangles();
    count += tris ? static_cast<int>(tris->size())/3 : 0;
  }

  return count;
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfModelVertices() const
{
  int count = 0;
  std::vector<vesGeometryData::Ptr> geometryData = collectGeometryData(this->renderer());

  for (size_t i = 0; i < geometryData.size(); ++i) {
    vesSourceData::Ptr points = geometryData[i]->sourceData(vesVertexAttributeKeys::Position);
    count += points ? static_cast<int>(points->sizeOfArray()) : 0;
  }

  return count;
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfModelLines() const
{
  int count = 0;
  std::vector<vesGeometryData::Ptr> geometryData = collectGeometryData(this->renderer());

  for (size_t i = 0; i < geometryData.size(); ++i) {
    vesPrimitive::Ptr lines = geometryData[i]->lines();
    count += lines ? static_cast<int>(lines->size()/2) : 0;
  }

  return count;
}
