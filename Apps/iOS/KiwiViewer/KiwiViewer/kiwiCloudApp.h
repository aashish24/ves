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
#ifndef __kiwiCloudApp_h
#define __kiwiCloudApp_h

#include <iostream>
#include <stdio.h>

#include <vesKiwiViewerApp.h>
#include <vesRenderer.h>
#include <vesBackground.h>
#include <vesCamera.h>
#include <vesKiwiDataLoader.h>
#include <vesKiwiFPSCounter.h>
#include <vesKiwiPolyDataRepresentation.h>
#include <vesKiwiImageWidgetRepresentation.h>
#include <vesKiwiPVRemoteRepresentation.h>

#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkSphereSource.h>
#include <vtkPolyData.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>

#include <vtkErrorCode.h>

#include <vesKiwiOptions.h>
#ifdef VES_USE_PCL
#  include <vtkPCDReader.h>
#endif

#include <iostream>
#include <iomanip>

#include <TargetConditionals.h>
#define TARGET_HAS_PCL TARGET_OS_IPHONE


class kiwiApp : public vesKiwiViewerApp
{
public:

  vesTypeMacro(kiwiApp);

  kiwiApp()
  {
  }

  ~kiwiApp()
  {
  }

  virtual std::string leftText()
  {
    return std::string();
  }

  virtual std::string rightText()
  {
    return std::string();
  }

  virtual std::vector<std::string> actions()
  {
    return std::vector<std::string>();
  }

  virtual bool onAction(const std::string& action)
  {
    vesNotUsed(action);
    return true;
  }

  virtual void setCameraProjectionToParallel()
  {
    this->renderer()->camera()->setParallelProjection(true);
  }

  virtual void setCameraProjectionToPerspective()
  {
    this->renderer()->camera()->setParallelProjection(false);
  }

  virtual void willRender()
  {
    vesKiwiViewerApp::willRender();
    this->mFPSCounter.update();
  }

  vesKiwiFPSCounter& fpsCounter()
  {
    return this->mFPSCounter;
  }

protected:

  vesKiwiFPSCounter mFPSCounter;

};


class kiwiCloudApp : public kiwiApp
{
public:

  vesTypeMacro(kiwiCloudApp);

  kiwiCloudApp()
  {
    this->Image2DMode = false;
    this->PlaneIndex = 0;
    this->ScrollSliceMode = false;
    this->MaxParallelScale = 0.0;
    this->RollCounter[0] = 0;
    this->RollCounter[1] = 0;
    this->RollCounter[2] = 0;
  }

  bool loadDatasetWithCustomBehavior(const std::string& filename)
  {
  // PCL is only supported on the iOS device, not the simulator.
  // You may change the definition of TARGET_HAS_PCL above if you
  // have built a PCL framework targeted to the iOS simulator
  // instead.
    if (vesKiwiDataLoader::hasEnding(filename, "pcd")) {
#if defined(VES_USE_PCL) && TARGET_HAS_PCL
      vtkNew<vtkPCDReader> pcdReader;
      pcdReader->SetFileName(filename.c_str());
      pcdReader->Update();
      if (pcdReader->GetErrorCode() != vtkErrorCode::NoError) {
        this->setErrorMessage("Could not read file", "Could not read the file specified");
        return false;
      }

      vesKiwiPolyDataRepresentation::Ptr rep = this->addPolyDataRepresentation(pcdReader->GetOutput(), this->shaderProgram());

      vtkMatrix4x4* mat = pcdReader->GetTransform()->GetMatrix();

      vesVector3f viewDirection(-mat->GetElement (0, 2),
                                -mat->GetElement (1, 2),
                                -mat->GetElement (2, 2));

      vesVector3f viewUp(mat->GetElement (0, 1),
                         mat->GetElement (1, 1),
                         mat->GetElement (2, 1));

      this->setDefaultCameraParameters(viewDirection, viewUp);


      return true;
#else // no PCL:
      this->setErrorMessage("Unsupported", "PCL support has not been included for this platform.");
      return false;
#endif
    }
    return kiwiApp::loadDatasetWithCustomBehavior(filename);
  }

  void printError()
  {
      printf("Error: %s\n", this->loadDatasetErrorTitle().c_str());
      printf("Message: %s\n", this->loadDatasetErrorMessage().c_str());
  }

  bool is2DImageMode() {
    return this->Image2DMode;//(this->imageRep() && this->camera()->parallelProjection());
  }

  bool is2DImageZoomedOut()
  {
    return (this->is2DImageMode() && (this->camera()->parallelScale() >= this->MaxParallelScale));
  }

  void start2DImageView()
  {
    if (this->imageRep()) {
      this->Image2DMode = true;
      this->haltCameraRotationInertia();
      this->camera()->setParallelProjection(true);
      this->setCameraRotationInertiaIsEnabled(false);
      this->imageRep()->setInteractionIsEnabled(false);
      this->imageRep()->setOutlineVisible(false);
      this->setBackgroundColor(0,0,0);
    }
  }

  void onCamera3D()
  {
    this->Image2DMode = false;
    this->camera()->setParallelProjection(false);
    this->setCameraRotationInertiaIsEnabled(true);
    this->setDefaultBackgroundColor();

    vesKiwiImageWidgetRepresentation::Ptr imageRep = this->imageRep();
    if (imageRep) {
      imageRep->setInteractionIsEnabled(true);
      imageRep->setPlaneVisibility(0, true);
      imageRep->setPlaneVisibility(1, true);
      imageRep->setPlaneVisibility(2, true);
      imageRep->setOutlineVisible(true);
      imageRep->refreshTextures();
      this->resetView(true);
    }
  }

  void setViewDirectionAndParallelScale(vesVector3f viewDirection, vesVector3f viewUp, double width, double height)
  {
    this->start2DImageView();
    this->resetView(viewDirection, viewUp, true);
    double aspectRatio = static_cast<double>(this->viewWidth())/this->viewHeight();
    double parallelScale = std::max(width/aspectRatio, height) / 2.0;
    this->camera()->setParallelScale(parallelScale);
    this->MaxParallelScale = parallelScale;
  }

  void reset2DView()
  {
    if (this->PlaneIndex == 0) {
      this->onCameraX();
    }
    else if (this->PlaneIndex == 1) {
      this->onCameraY();
    }
    else if (this->PlaneIndex == 2) {
      this->onCameraZ();
    }
  }

  void onCameraRoll()
  {
    int& rollCounter = this->RollCounter[this->PlaneIndex];
    rollCounter++;
    rollCounter = rollCounter % 4;
    this->reset2DView();
  }
  
  void onCameraX()
  {
    vesKiwiImageWidgetRepresentation::Ptr imageRep = this->imageRep();
    imageRep->setPlaneVisibility(0, true);
    imageRep->setPlaneVisibility(1, false);
    imageRep->setPlaneVisibility(2, false);
    imageRep->refreshTextures();
    this->PlaneIndex = 0;

    double* bounds = imageRep->imageData()->GetBounds();
    double width = bounds[3] - bounds[2];
    double height = bounds[5] - bounds[4];
    
    vesVector3f viewDirection(1,0,0);
    vesVector3f viewUp;
    switch (this->RollCounter[this->PlaneIndex]) {
      case 0: viewUp = vesVector3f(0,0,1); break;
      case 1: viewUp = vesVector3f(0,1,0); std::swap(width, height); break;
      case 2: viewUp = vesVector3f(0,0,-1); break;
      case 3: viewUp = vesVector3f(0,-1,0); std::swap(width, height); break;
    }


    this->setViewDirectionAndParallelScale(viewDirection, viewUp, width, height);
  }

  void onCameraY()
  {
    vesKiwiImageWidgetRepresentation::Ptr imageRep = this->imageRep();
    imageRep->setPlaneVisibility(0, false);
    imageRep->setPlaneVisibility(1, true);
    imageRep->setPlaneVisibility(2, false);
    imageRep->refreshTextures();
    this->PlaneIndex = 1;

    double* bounds = imageRep->imageData()->GetBounds();
    double width = bounds[1] - bounds[0];
    double height = bounds[5] - bounds[4];

    vesVector3f viewDirection(0,1,0);
    vesVector3f viewUp;
    switch (this->RollCounter[this->PlaneIndex]) {
      case 0: viewUp = vesVector3f(0,0,1); break;
      case 1: viewUp = vesVector3f(-1,0,0); std::swap(width, height); break;
      case 2: viewUp = vesVector3f(0,0,-1); break;
      case 3: viewUp = vesVector3f(1,0,0); std::swap(width, height); break;
    }

    this->setViewDirectionAndParallelScale(viewDirection, viewUp, width, height);
  }

  void onCameraZ()
  {
    vesKiwiImageWidgetRepresentation::Ptr imageRep = this->imageRep();
    imageRep->setPlaneVisibility(0, false);
    imageRep->setPlaneVisibility(1, false);
    imageRep->setPlaneVisibility(2, true);
    imageRep->refreshTextures();
    this->PlaneIndex = 2;

    double* bounds = imageRep->imageData()->GetBounds();
    double width = bounds[1] - bounds[0];
    double height = bounds[3] - bounds[2];

    vesVector3f viewDirection(0,0,1);
    vesVector3f viewUp(0,1,0);
    switch (this->RollCounter[this->PlaneIndex]) {
      case 0: viewUp = vesVector3f(0,1,0); break;
      case 1: viewUp = vesVector3f(1,0,0); std::swap(width, height); break;
      case 2: viewUp = vesVector3f(0,-1,0); break;
      case 3: viewUp = vesVector3f(-1,0,0); std::swap(width, height); break;
    }

    this->setViewDirectionAndParallelScale(viewDirection, viewUp, width, height);
  }

  std::string leftText()
  {
    if (!this->is2DImageMode()) {
      return std::string();
    }
    
    vesKiwiImageWidgetRepresentation::Ptr imageRep = this->imageRep();
    vtkImageData* imageData = imageRep->imageData();

    int planeIndex = this->PlaneIndex;
    int sliceIndex = imageRep->sliceIndex(planeIndex);
    int numberOfSlices = imageRep->numberOfSlices(planeIndex);

    int imageDims[2];
    int dimensions[3];
    imageData->GetDimensions(dimensions);

    if (planeIndex == 0) {
      imageDims[0] = dimensions[1];
      imageDims[1] = dimensions[2];
    }
    else if (planeIndex == 1) {
      imageDims[0] = dimensions[0];
      imageDims[1] = dimensions[2];
    }
    else if (planeIndex == 2) {
      imageDims[0] = dimensions[0];
      imageDims[1] = dimensions[1];
    }

    std::stringstream str;

    str << std::fixed << setprecision(2)
        << "Slice: " << sliceIndex+1 << "/" << numberOfSlices << " (" << imageDims[0] << "x" << imageDims[1] << ")\n"
        << "Window: " << imageRep->window() << "\n"
        << "Level: " << imageRep->level() << "\n";
    return str.str();
  }

  std::string rightText()
  {
    if (!this->is2DImageMode()) {
      return std::string();
    }

    vesKiwiImageWidgetRepresentation::Ptr imageRep = this->imageRep();
    vtkImageData* imageData = imageRep->imageData();

    int dimensions[3];
    double scalarRange[2];
    imageData->GetDimensions(dimensions);
    imageData->GetScalarRange(scalarRange);

    std::stringstream str;

    str << std::fixed << setprecision(2)
        << "Scalar Type: " << imageData->GetScalarTypeAsString() << "\n"
        << "Scalar Range: [" << scalarRange[0] << ", " << scalarRange[1] << "]\n"
        << "Dimensions: [" << dimensions[0] << ", " << dimensions[1] << ", " << dimensions[2] << "]\n";
    return str.str();
  }

  virtual std::vector<std::string> actions()
  {
    std::vector<std::string> actionList;

    if (this->imageRep()) {
      actionList.push_back("X");
      actionList.push_back("Y");
      actionList.push_back("Z");
      actionList.push_back("3D");
      if (this->is2DImageMode()) {
        actionList.push_back("\xe2\x86\xbb");
      }
    }
    else if (this->pvRemoteRep()) {

      actionList.push_back("Reset Camera");
      actionList.push_back("Refresh Scene");

    }
    else {
      actionList.push_back("Reset Camera");
    }

    const std::vector<vesKiwiDataRepresentation::Ptr>& reps = this->dataRepresentations();
    for (size_t i = 0; i < reps.size(); ++i) {
      vesKiwiWidgetRepresentation::Ptr widget = vesDynamicPtrCast<vesKiwiWidgetRepresentation>(reps[i]);
      if (widget) {
        std::vector<std::string> widgetActions = widget->actions();
        actionList.insert(actionList.end(), widgetActions.begin(), widgetActions.end());
      }
    }


    return actionList;
  }

  virtual bool onAction(const std::string& action)
  {
    if (action == "X")
      this->onCameraX();
    else if (action == "Y")
      this->onCameraY();
    else if (action == "Z")
      this->onCameraZ();
    else if (action == "3D")
      this->onCamera3D();
    else if (action == "\xe2\x86\xbb")
      this->onCameraRoll();
    else if (action == "Reset Camera")
      this->resetView(true);
    else if (action == "Refresh Scene") {
      this->pvRemoteRep()->requestScene();
    }


    const std::vector<vesKiwiDataRepresentation::Ptr>& reps = this->dataRepresentations();
    for (size_t i = 0; i < reps.size(); ++i) {
      vesKiwiWidgetRepresentation::Ptr widget = vesDynamicPtrCast<vesKiwiWidgetRepresentation>(reps[i]);
      if (widget && widget->handleAction(action)) {
        return true;
      }
    }

    return true;
  }

  virtual void setCameraProjectionToPerspective()
  {
    if (!this->is2DImageMode()) {
      this->renderer()->camera()->setParallelProjection(false);
    }
  }

  virtual void handleTwoTouchRotationGesture(double rotation)
  {
    if (this->is2DImageMode()) {
      return;
    }
    
    this->vesKiwiViewerApp::handleTwoTouchRotationGesture(rotation);
  }
  
  virtual void handleSingleTouchDown(int x, int y)
  {
    if (this->is2DImageMode()) {
      if (x > this->viewWidth() - 50) {
        this->ScrollSliceMode = true;
        this->CurrentSliceIndex = this->imageRep()->sliceIndex(this->PlaneIndex);
      }
    }
    
    this->vesKiwiViewerApp::handleSingleTouchDown(x, y);
  }

  virtual void handleSingleTouchUp()
  {
    this->ScrollSliceMode = false;
    this->vesKiwiViewerApp::handleSingleTouchUp();
  }

  virtual void handleTwoTouchPanGesture(double x0, double y0, double x1, double y1)
  {
    if (this->is2DImageMode()) {
      return;
    }
    this->vesKiwiViewerApp::handleTwoTouchPanGesture(x0, y0, x1, y1);
  }

  virtual void handleSingleTouchPanGesture(double deltaX, double deltaY)
  {
    if (this->is2DImageMode()) {

      if (this->ScrollSliceMode) {


        int numberOfSlices = this->imageRep()->numberOfSlices(this->PlaneIndex);
        
        deltaY = -deltaY;
        deltaY = deltaY/this->viewHeight();

        double scrollDistanceInSlices = numberOfSlices*deltaY;

        this->CurrentSliceIndex += scrollDistanceInSlices;
        if (this->CurrentSliceIndex < 0) {
          this->CurrentSliceIndex = 0;
        }
        else if (this->CurrentSliceIndex > numberOfSlices-1) {
          this->CurrentSliceIndex = numberOfSlices-1;
        }

        this->imageRep()->setScrollSlice(this->PlaneIndex);
        this->imageRep()->scheduleSetSliceIndex(this->PlaneIndex, static_cast<int>(this->CurrentSliceIndex));
        return;
      }
      else if (this->is2DImageZoomedOut()) {

        double scalarRange[2];
        this->imageRep()->imageData()->GetScalarRange(scalarRange);

        double range = scalarRange[1] - scalarRange[0];
        deltaY = -deltaY;
        deltaX = deltaX/this->viewWidth();
        deltaY = deltaY/this->viewHeight();

        deltaX = deltaX*(range/1);
        deltaY = deltaY*(range/1);

        this->imageRep()->setWindowLevel(this->imageRep()->window()+deltaX, this->imageRep()->level()+deltaY);
        return;
      }
      else {
        this->vesKiwiViewerApp::handleTwoTouchPanGesture(0,0,deltaX,-deltaY);
        return;
      }
    }
    
    this->vesKiwiViewerApp::handleSingleTouchPanGesture(deltaX, deltaY);
  }

  virtual void handleTwoTouchPinchGesture(double scale)
  {
    this->vesKiwiViewerApp::handleTwoTouchPinchGesture(scale);

    // check if we're in 2d image view mode
    if (this->is2DImageMode()) {

      if (this->is2DImageZoomedOut()) {
        this->camera()->setParallelScale(this->MaxParallelScale);
        this->reset2DView();
      }
    }
  
  }

  vesKiwiImageWidgetRepresentation::Ptr imageRep() const
  {
    if (this->dataRepresentations().size() != 1) {
      return vesKiwiImageWidgetRepresentation::Ptr();
    }
    return vesDynamicPtrCast<vesKiwiImageWidgetRepresentation>(this->dataRepresentations()[0]);
  }

  virtual void setDefaultBackgroundColor()
  {
    //vesVector3f topColor(0.2, 0.2, 0.2);
    //vesVector3f bottomColor(0, 0, 0);
    //this->renderer()->background()->setGradientColor(bottomColor, topColor);
  }

protected:


  bool Image2DMode;
  bool ScrollSliceMode;
  int PlaneIndex;
  int RollCounter[3];
  float MaxParallelScale;
  double CurrentSliceIndex;

};





#include <vesKiwiStreamingDataRepresentation.h>
#include <vesKiwiText2DRepresentation.h>


class vesKiwiPointCloudApp : public kiwiCloudApp {
public:

  vesTypeMacro(vesKiwiPointCloudApp);
  typedef vesKiwiViewerApp Superclass;

  vesKiwiPointCloudApp()
  {
  }

  ~vesKiwiPointCloudApp()
  {
    this->unloadData();
  }

  void unloadData()
  {
    if (mDataRep) {
      mDataRep->removeSelfFromRenderer(this->renderer());
      mDataRep.reset();
    }
  }

  void willRender()
  {
    this->Superclass::willRender();
    this->mDataRep->willRender(this->renderer());
  }

  bool connect(const std::string& host, int port)
  {
    vesKiwiStreamingDataRepresentation::Ptr rep = vesKiwiStreamingDataRepresentation::Ptr(new vesKiwiStreamingDataRepresentation);
    if (!rep->connectToServer(host, port)) {
      this->setErrorMessage("Connection failed", "Could not connect to server.");
      return false;
    }

    mDataRep = rep;
    mDataRep->initializeWithShader(this->shaderProgram());
    mDataRep->addSelfToRenderer(this->renderer());
    this->resetView(false);
    return true;
  }

  virtual void setDefaultBackgroundColor()
  {
    vesVector3f topColor(0.2, 0.2, 0.2);
    vesVector3f bottomColor(0, 0, 0);
    this->renderer()->background()->setGradientColor(bottomColor, topColor);
  }

  vesKiwiStreamingDataRepresentation::Ptr mDataRep;
};


#endif /* defined(__CloudAppTab__kiwiCloudApp__) */
