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

#include <vesConfigure.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <vesKiwiViewerApp.h>
#include <vesKiwiBaselineImageTester.h>
#include <vesSetGet.h>

#ifdef VES_QNX
  #include "vesKiwiQNXTestDriver.h"
  #include <stdio.h>
  #include <stdlib.h>
  #include <errno.h>
  #include <sys/neutrino.h>
  #include <sys/syspage.h>
#elif defined (VES_HOST)
  #include "vesKiwiX11TestDriver.h"
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
  #include <X11/keysym.h>
#endif

//----------------------------------------------------------------------------
namespace {

class vesKiwiViewerAppTest : public vesKiwiViewerApp {
public:

  vesKiwiViewerAppTest() : vesKiwiViewerApp()
  {
    this->IsTesting = false;
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

  void loadData(int index)
  {
    std::string dataRoot = this->sourceDirectory() + "/Apps/iOS/Kiwi/Kiwi/Data/";
    std::string filename = dataRoot + this->builtinDatasetFilename(index);
    this->loadDataset(filename);
    this->applyBuiltinDatasetCameraParameters(index);
  }

  bool doTesting()
  {
    const double threshold = 10.0;
    bool allTestsPassed = true;

    vesKiwiBaselineImageTester baselineTester;
    baselineTester.setApp(this);
    baselineTester.setBaselineImageDirectory(this->dataDirectory());

    // This loads each builtin dataset, renders it, and saves a screenshot

    // Note, this loop renders but does not bother to swap buffers
    for (int i = 0; i < this->numberOfBuiltinDatasets(); ++i) {
      loadData(i);

      // call the info methods, this helps coverage, though we're not testing the return values
      this->numberOfModelFacets();
      this->numberOfModelVertices();
      this->numberOfModelLines();

      this->render();
      std::string testName = this->builtinDatasetName(i);

      if (!baselineTester.performTest(testName, threshold)) {
        allTestsPassed = false;
      }
    }

    return allTestsPassed;
  }

  std::string getFileContents(const std::string& filename)
  {
    std::ifstream file(filename.c_str());
    std::stringstream buffer;
    if (file) {
      buffer << file.rdbuf();
      file.close();
    }
    return buffer.str();
  }

  void initRendering()
  {
    this->initGL();
  }

  bool initTest(int argc, char* argv[])
  {
    if (argc < 2) {
      printf("Usage: %s <path to VES source directory> [path to testing data directory]\n", argv[0]);
      return false;
    }

    this->setSourceDirectory(argv[1]);

    if (argc == 3) {
      this->setDataDirectory(argv[2]);
      this->setTesting(true);
    }
    return true;
  }

  void finalizeTest()
  {
  }

  std::string       SourceDirectory;
  std::string       DataDirectory;
  bool              IsTesting;
};

}; // end namespace

int
main(int argc, char *argv[])
{
  vesKiwiViewerAppTest app;

  if (!app.initTest(argc, argv)) {
    return -1;
    fprintf(stderr, "Looping\n");
  }

#ifdef VES_QNX
  vesKiwiQNXTestDriver testDriver(&app);
#elif defined (VES_HOST)
  vesKiwiX11TestDriver testDriver(&app);
#endif

  testDriver.init();

  app.initRendering();
  app.resizeView(testDriver.width(), testDriver.height());
  app.resetView();

  for (int i = 0; i < app.numberOfBuiltinDatasets(); ++i) {
    app.loadData(i);
    testDriver.render();
    sleep(1);
  }

  // begin the event loop if not in testing mode
  bool testPassed = true;
  if (!app.isTesting()) {
    testDriver.start();
  }
  else {
    testPassed = app.doTesting();
  }

  app.finalizeTest();
  testDriver.finalize();

  return testPassed ? 0 : 1;
}
