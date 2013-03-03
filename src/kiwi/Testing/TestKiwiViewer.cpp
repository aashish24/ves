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

  void handleKeyboard(unsigned char key, int x, int y)
  {
    if (key == 'n') {
      mCurrentDataset = (mCurrentDataset + 1) % mKiwiApp->numberOfBuiltinDatasets();
      this->loadData(mCurrentDataset);
    }
    else {
      vesKiwiTestHelper::handleKeyboard(key, x, y);
    }
  }

  void loadData(int index)
  {
    mCurrentDataset = index;
    std::string dataRoot = this->sourceDirectory() + "/Apps/iOS/Kiwi/Kiwi/Data/";
    std::string filename = dataRoot + mKiwiApp->builtinDatasetFilename(index);
    mKiwiApp->loadDataset(filename);
    this->resetView();
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

  int mCurrentDataset;
  vesKiwiViewerApp::Ptr mKiwiApp;
};


int main(int argc, char *argv[])
{
  MyTestHelper helper;
  return vesKiwiTestHelper::main(argc, argv, helper);
}
