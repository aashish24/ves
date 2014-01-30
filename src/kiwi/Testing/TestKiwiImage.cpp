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

#include <vesKiwiViewerApp.h>
#include <vesKiwiBaselineImageTester.h>
#include <vesBuiltinShaders.h>
#include <vesKiwiTestHelper.h>

#include <vesKiwiImageWidgetRepresentation.h>
#include <vesCamera.h>
#include <vtkImageData.h>

class MyTestHelper : public vesKiwiTestHelper {
public:

  MyTestHelper()
  {
    mCurrentDataset = 0;
    mKiwiApp = vesKiwiViewerApp::Ptr(new vesKiwiViewerApp);
    this->setApp(mKiwiApp);
  }

  void resetView()
  {
    mKiwiApp->applyBuiltinDatasetCameraParameters(mCurrentDataset);
  }

  void start2DImageView()
  {
    mKiwiApp->haltCameraRotationInertia();
    mKiwiApp->camera()->setParallelProjection(true);
    mKiwiApp->setCameraRotationInertiaIsEnabled(false);
    this->imageRep()->setWindowLevelInteractionEnabled(true);
  }

  vesKiwiImageWidgetRepresentation::Ptr imageRep() const
  {
    if (!mKiwiApp->dataRepresentations().size()) {
      return vesKiwiImageWidgetRepresentation::Ptr();
    }
    return vesDynamicPtrCast<vesKiwiImageWidgetRepresentation>(mKiwiApp->dataRepresentations()[0]);
  }

  void start3DImageView()
  {
    vesKiwiImageWidgetRepresentation::Ptr imageRep = this->imageRep();
    imageRep->setWindowLevelInteractionEnabled(false);
    imageRep->setPlaneVisibility(0, true);
    imageRep->setPlaneVisibility(1, true);
    imageRep->setPlaneVisibility(2, true);
    mKiwiApp->camera()->setParallelProjection(false);
    mKiwiApp->setCameraRotationInertiaIsEnabled(true);

    vesVector3f viewDirection(-0.65814518, -0.65814518,  0.36563621);
    vesVector3f viewUp(-0.25127214, -0.25127214, -0.93473238);
    mKiwiApp->resetView(viewDirection, viewUp, false);
  }

  void setViewDirectionAndParallelScale(vesVector3f viewDirection, vesVector3f viewUp, double width, double height)
  {
    this->start2DImageView();
    mKiwiApp->resetView(viewDirection, viewUp, false);
    vesCamera::Ptr camera = mKiwiApp->camera();
    double aspectRatio = static_cast<double>(mKiwiApp->viewWidth())/mKiwiApp->viewHeight();
    double parallelScale = std::max(width/aspectRatio, height) / 2.0;
    camera->setParallelScale(parallelScale);
  }

  void onCameraX()
  {
    vesKiwiImageWidgetRepresentation::Ptr imageRep = this->imageRep();
    imageRep->setPlaneVisibility(0, true);
    imageRep->setPlaneVisibility(1, false);
    imageRep->setPlaneVisibility(2, false);
    vesVector3f viewDirection(-1,0,0);
    vesVector3f viewUp(0,0,-1);
    double* bounds = imageRep->imageData()->GetBounds();
    double width = bounds[3] - bounds[2];
    double height = bounds[5] - bounds[4];
    this->setViewDirectionAndParallelScale(viewDirection, viewUp, width, height);
  }

  void onCameraY()
  {
    vesKiwiImageWidgetRepresentation::Ptr imageRep = this->imageRep();
    imageRep->setPlaneVisibility(0, false);
    imageRep->setPlaneVisibility(1, true);
    imageRep->setPlaneVisibility(2, false);
    vesVector3f viewDirection(0,-1,0);
    vesVector3f viewUp(0,0,-1);
    double* bounds = imageRep->imageData()->GetBounds();
    double width = bounds[1] - bounds[0];
    double height = bounds[5] - bounds[4];
    this->setViewDirectionAndParallelScale(viewDirection, viewUp, width, height);
  }

  void onCameraZ()
  {
    vesKiwiImageWidgetRepresentation::Ptr imageRep = this->imageRep();
    imageRep->setPlaneVisibility(0, false);
    imageRep->setPlaneVisibility(1, false);
    imageRep->setPlaneVisibility(2, true);
    vesVector3f viewDirection(0,0,1);
    vesVector3f viewUp(0,-1,0);
    double* bounds = imageRep->imageData()->GetBounds();
    double width = bounds[1] - bounds[0];
    double height = bounds[3] - bounds[2];
    this->setViewDirectionAndParallelScale(viewDirection, viewUp, width, height);
  }

  bool handleKeyboardRadiologyDemo(unsigned char key)
  {
    if (key == '1') {
      this->onCameraX();
    }
    else if (key == '2') {
      this->onCameraY();
    }
    else if (key == '3') {
      this->onCameraZ();
    }
    else if (key == '4') {
      this->start3DImageView();
    }
    else {
      return false;
    }

    return true;
  }

  void handleKeyboard(unsigned char key, int x, int y)
  {
    if (key == 'n') {
      mCurrentDataset = (mCurrentDataset + 1) % mKiwiApp->numberOfBuiltinDatasets();
      this->loadData(mCurrentDataset);
    }
    else if (this->handleKeyboardRadiologyDemo(key)) {
      return;
    }
    else {
      vesKiwiTestHelper::handleKeyboard(key, x, y);
    }
  }

  void loadData(int index)
  {
    std::string dataRoot = this->sourceDirectory() + "/Apps/iOS/Kiwi/Kiwi/Data/";
    std::string filename = dataRoot + mKiwiApp->builtinDatasetFilename(index);
    mKiwiApp->loadDataset(filename);
    mKiwiApp->applyBuiltinDatasetCameraParameters(index);
    mCurrentDataset = index;
  }

  bool doTesting()
  {
    const double threshold = 10.0;
    bool allTestsPassed = true;

    vesKiwiBaselineImageTester baselineTester;
    baselineTester.setApp(&*this->app());
    baselineTester.setBaselineImageDirectory(this->dataDirectory());

    // This loads each builtin dataset, renders it, and saves a screenshot

    // Note, this loop renders but does not bother to swap buffers
    for (int i = 0; i < mKiwiApp->numberOfBuiltinDatasets(); ++i) {
      this->loadData(i);

      // Enable the background image for the final image regression test
      if (i == mKiwiApp->numberOfBuiltinDatasets()-1)
        {
        mKiwiApp->setBackgroundTexture(this->sourceDirectory() + "/Apps/iOS/Kiwi/Kiwi/Data/kiwi.png");
        }

      // call the info methods, this helps coverage, though we're not testing the return values
      mKiwiApp->numberOfModelFacets();
      mKiwiApp->numberOfModelVertices();
      mKiwiApp->numberOfModelLines();

      mKiwiApp->render();
      std::string testName = mKiwiApp->builtinDatasetName(i);

      if (!baselineTester.performTest(testName, threshold)) {
        allTestsPassed = false;
      }
    }

    return allTestsPassed;
  }

  int mCurrentDataset;

  vesKiwiViewerApp::Ptr mKiwiApp;
};


int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Usage: %s <path to VES source directory> [path to testing data directory]\n", argv[0]);
    return 1;
  }

  MyTestHelper helper;
  helper.setSourceDirectory(argv[1]);
  if (argc == 3) {
    helper.setDataDirectory(argv[2]);
    helper.setTesting(true);
  }

  const int windowWidth = 800;
  const int windowHeight = 600;
  helper.init(&argc, argv, windowWidth, windowHeight, "TestKiwiViewer");
  helper.mKiwiApp->loadDataset(helper.sourceDirectory() + "/Apps/iOS/Kiwi/Kiwi/Data/head.vti");

  // render once
  helper.app()->resizeView(windowWidth, windowHeight);
  helper.app()->resetView();
  helper.app()->render();

  // begin the event loop if not in testing mode
  bool testPassed = true;
  if (!helper.isTesting()) {
    helper.startMainLoop();
  }
  else {
    testPassed = helper.doTesting();
  }

  return testPassed ? 0 : 1;
}
