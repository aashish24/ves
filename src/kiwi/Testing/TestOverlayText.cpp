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

#include <vesBackground.h>
#include <vesImage.h>
#include <vesKiwiBaseApp.h>
#include <vesKiwiDataLoader.h>
#include <vesKiwiBaselineImageTester.h>
#include <vesKiwiDataConversionTools.h>
#include <vesKiwiPolyDataRepresentation.h>
#include <vesKiwiText2DRepresentation.h>
#include <vesRenderer.h>
#include <vesSetGet.h>
#include <vesTexture.h>

#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

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

class vesTextApp : public vesKiwiBaseApp {
public:

  vesTextApp() : vesKiwiBaseApp()
  {
    this->IsTesting = false;
    this->setBackgroundColor(63/255.0, 96/255.0, 144/255.0);
    this->DataRep = 0x0;
  }

  ~vesTextApp()
  {
    this->unloadData();
  }

  void initTextureMaterial()
  {
    vesSharedPtr<vesMaterial> material
      = this->addMaterial();

    this->addVertexPositionAttribute(material);
    this->addVertexNormalAttribute(material);
    this->addVertexTextureCoordinateAttribute(material);
    this->TextureMaterial = material;
  }

  void unloadData()
  {
    if (this->DataRep) {
      this->DataRep->removeSelfFromRenderer(this->renderer());
      delete this->DataRep;
      this->DataRep = 0;
    }
  }

  void loadData()
  {
    this->unloadData();

    vesKiwiText2DRepresentation* rep = new vesKiwiText2DRepresentation();
    rep->initializeWithMaterial(this->TextureMaterial);
    rep->setText("VES");
    rep->addSelfToRenderer(this->renderer());
    this->DataRep = rep;

    this->resetView();
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

  bool doTesting()
  {
    const double threshold = 10.0;
    const std::string testName = "Overlay Text";

    vesKiwiBaselineImageTester baselineTester;
    baselineTester.setApp(this);
    baselineTester.setBaselineImageDirectory(this->dataDirectory());
    return baselineTester.performTest(testName, threshold);
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
    std::cout << "Init rendering " << std::endl;
    this->initTextureMaterial();
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

  vesSharedPtr<vesImage> Image;
  vesSharedPtr<vesTexture> Texture;
  vtkSmartPointer<vtkLookupTable> LookupTable;
  vesSharedPtr<vesMaterial> TextureMaterial;
  vesKiwiText2DRepresentation* DataRep;
};


}; // end namespace
//----------------------------------------------------------------------------

int
main(int argc, char *argv[])
{
  vesTextApp app;

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
  app.loadData();
  app.resizeView(testDriver.width(), testDriver.height());
  app.resetView();

  testDriver.render();

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
