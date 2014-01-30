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

#include <vesKiwiViewerApp.h>
#include <vesKiwiTestHelper.h>
#include <vesKiwiFPSCounter.h>
#include <vesKiwiPolyDataRepresentation.h>

class MyTestHelper : public vesKiwiTestHelper {
public:

  MyTestHelper()
  {
    mCurrentDataset = -1;
    mKiwiApp = vesKiwiViewerApp::Ptr(new vesKiwiViewerApp);
    this->setApp(mKiwiApp);
  }

  void resetView(bool withTransition)
  {
    if (mCurrentDataset >= 0) {
      mKiwiApp->applyBuiltinDatasetCameraParameters(mCurrentDataset, withTransition);
    }
    else {
      mKiwiApp->resetView(withTransition);
    }
  }

  vesKiwiPolyDataRepresentation::Ptr polyDataRep()
  {
    const std::vector<vesKiwiDataRepresentation::Ptr>& reps = this->mKiwiApp->dataRepresentations();
    for (size_t i = 0; i < reps.size(); ++i) {
      vesKiwiPolyDataRepresentation::Ptr rep = vesDynamicPtrCast<vesKiwiPolyDataRepresentation>(reps[i]);
      if (rep) {
        return rep;
      }
    }
    return vesKiwiPolyDataRepresentation::Ptr();
  }

  bool handleKeyboardRep(unsigned char key)
  {
    vesKiwiPolyDataRepresentation::Ptr rep = this->polyDataRep();
    if (!rep) {
      return false;
    }

    if (key >= '1' && key <= '9') {
      int selectedIndex = key - '1';
      std::vector<std::string> colorModes = rep->colorModes();
      if (selectedIndex >= 0 && selectedIndex < colorModes.size()) {
        rep->setColorMode(colorModes[selectedIndex]);
      }
    }
    if (key == 'o') {
      rep->setPointSize(rep->pointSize() - 1);
    }
    else if (key == 'p') {
      rep->setPointSize( rep->pointSize() + 1);
    }
    else if (key == 'k') {
      rep->setLineWidth(rep->lineWidth() - 1);
    }
    else if (key == 'l') {
      rep->setLineWidth(rep->lineWidth() + 1);
    }
    else if (key == 'u') {
      rep->setOpacity(rep->opacity() - 0.1);
    }
    else if (key == 'i') {
      rep->setOpacity(rep->opacity() + 0.1);
    }
    else if (key == 'a') {
      rep->surfaceOn();
    }
    else if (key == 's') {
      rep->surfaceWithEdgesOn();
    }
    else if (key == 'd') {
      rep->wireframeOn();
    }
    else if (key == 'f') {
      rep->pointsOn();
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
    else if (this->handleKeyboardRep(key)) {

    }
    else {
      vesKiwiTestHelper::handleKeyboard(key, x, y);
    }
  }

  void loadData(const std::string& filename)
  {
    mKiwiApp->resetScene();
    if (!mKiwiApp->loadDataset(filename)) {
      std::cout << "load data error: " << mKiwiApp->loadDatasetErrorTitle()
                << ": " << mKiwiApp->loadDatasetErrorMessage() << std::endl;
    }
    this->resetView(false);
  }

  void loadData(int index)
  {
    mCurrentDataset = index;
    std::string dataRoot = this->sourceDirectory() + "/Apps/iOS/Kiwi/Kiwi/Data/";
    std::string filename = dataRoot + mKiwiApp->builtinDatasetFilename(index);
    this->loadData(filename);
  }

  bool initTesting()
  {
    this->loadData(mKiwiApp->defaultBuiltinDatasetIndex());
    return true;
  }

  bool doTesting()
  {
    bool allTestsPassed = true;

    for (int i = 0; i < mKiwiApp->numberOfBuiltinDatasets(); ++i) {

      this->loadData(i);
      mKiwiApp->render();

      std::string testName = mKiwiApp->builtinDatasetName(i);
      if (!this->performBaselineImageTest(testName)) {
        allTestsPassed = false;
      }
    }

    return allTestsPassed;
  }

  void render()
  {
    this->vesKiwiTestHelper::render();

    mFPSCounter.update();
  }

  int mCurrentDataset;
  vesKiwiViewerApp::Ptr mKiwiApp;

  vesKiwiFPSCounter mFPSCounter;
};


int main(int argc, char *argv[])
{
  MyTestHelper helper;
  return vesKiwiTestHelper::main(argc, argv, helper);
}
