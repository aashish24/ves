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
#include <vesKiwiTestHelper.h>

#include <vesKiwiPolyDataRepresentation.h>
#include <vesPVWebDataSet.h>


class vesKiwiPVWebApp : public vesKiwiViewerApp {
public:

  vesTypeMacro(vesKiwiPVWebApp);
  typedef vesKiwiViewerApp Superclass;

  vesKiwiPVWebApp() : vesKiwiViewerApp()
  {
  }

  ~vesKiwiPVWebApp()
  {
  }

};



class MyTestHelper : public vesKiwiTestHelper {
public:

  MyTestHelper()
  {
    mKiwiApp = vesKiwiPVWebApp::Ptr(new vesKiwiPVWebApp);
    this->setApp(mKiwiApp);
  }


  void handleKeyboard(unsigned char key, int x, int y)
  {
    vesKiwiTestHelper::handleKeyboard(key, x, y);
  }


  vesKiwiPVWebApp::Ptr mKiwiApp;
};


void testPVWeb(vesKiwiPVWebApp::Ptr app)
{
  app->doPVWebTest("http://paraviewweb.kitware.com", "");
}



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
  helper.init(&argc, argv, windowWidth, windowHeight, "TestPVWeb");

  testPVWeb(helper.mKiwiApp);

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
    //testPassed = helper.doTesting();
  }

  return testPassed ? 0 : 1;
}
