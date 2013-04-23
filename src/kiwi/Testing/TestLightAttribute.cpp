#include <vesConfigure.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <vesActor.h>
#include <vesBackground.h>
#include <vesCamera.h>
#include <vesGeometryData.h>
#include <vesLight.h>
#include <vesMapper.h>
#include <vesMaterial.h>
#include <vesRenderer.h>
#include <vesVertexAttribute.h>
#include <vesVertexAttributeKeys.h>
#include <vesViewport.h>

#include <vesKiwiBaseApp.h>
#include <vesKiwiBaselineImageTester.h>
#include <vesKiwiDataLoader.h>
#include <vesKiwiPolyDataRepresentation.h>

#include <vtkPolyData.h>

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


class vesLightAttributeApp : public vesKiwiBaseApp {
public:

  vesLightAttributeApp()
  {
    this->DataRep1 = 0;
    this->DataRep2 = 0;
    this->DataRep3 = 0;
    this->IsTesting = false;
    this->setBackgroundColor(63/255.0, 96/255.0, 144/255.0);
  }

  ~vesLightAttributeApp()
  {
    this->unloadData();
  }

  /// Material3 uses lighting
  void initMaterial1()
  {
    vesSharedPtr<vesMaterial> material
      = this->addMaterial();
    this->addVertexPositionAttribute(material);
    this->addVertexNormalAttribute(material);
    this->addVertexColorAttribute(material);
    this->addVertexTextureCoordinateAttribute(material);
    this->Material1 = material;
  }

  /// Material3 disables lighting
  void initMaterial2()
  {
    vesSharedPtr<vesMaterial> material
      = this->addMaterial();
    this->addVertexPositionAttribute(material);
    this->addVertexNormalAttribute(material);
    this->addVertexColorAttribute(material);
    this->addVertexTextureCoordinateAttribute(material);
    this->Material2 = material;

    // Disable lighting for this material
    vesLight::Ptr light = vesSharedPtr<vesLight>(new vesLight());
    light->disable();
    this->Material2->addAttribute(light);
  }

  /// Material3
  void initMaterial3()
  {
    vesSharedPtr<vesMaterial> material
      = this->addMaterial();
    this->addVertexPositionAttribute(material);
    this->addVertexNormalAttribute(material);
    this->addVertexColorAttribute(material);
    this->addVertexTextureCoordinateAttribute(material);
    this->Material3 = material;
  }

  void unloadData()
  {
    if (this->DataRep1) {
      this->DataRep1->removeSelfFromRenderer(this->renderer());
      delete this->DataRep1;
      this->DataRep1 = 0;
    }

    if (this->DataRep2) {
      this->DataRep2->removeSelfFromRenderer(this->renderer());
      delete this->DataRep2;
      this->DataRep2 = 0;
    }

    if (this->DataRep3) {
      this->DataRep3->removeSelfFromRenderer(this->renderer());
      delete this->DataRep3;
      this->DataRep3 = 0;
    }
  }

  void loadData(const std::string& filename)
  {
    this->unloadData();

    vesKiwiDataLoader loader;
    vtkSmartPointer<vtkPolyData> polyData
      = vtkPolyData::SafeDownCast(loader.loadDataset(filename));
    assert(polyData.GetPointer());

    vesKiwiPolyDataRepresentation* rep1 = new vesKiwiPolyDataRepresentation();
    rep1->initializeWithMaterial(this->Material1);
    rep1->setPolyData(polyData);
    rep1->addSelfToRenderer(this->renderer());
    rep1->actor()->setTranslation(vesVector3f(-0.1, 0.0, 0.0));
    this->DataRep1 = rep1;

    vesKiwiPolyDataRepresentation* rep2 = new vesKiwiPolyDataRepresentation();
    rep2->initializeWithMaterial(this->Material2);
    rep2->setPolyData(polyData);
    rep2->addSelfToRenderer(this->renderer());
    rep2->actor()->mapper()->enableWireframe(true);
    rep2->actor()->setTranslation(vesVector3f(0.1, 0.0, 0.0));
    rep2->setColor(0.1, 0.6, 0.4, 0.4);
    this->DataRep2 = rep2;

    vesKiwiPolyDataRepresentation* rep3 = new vesKiwiPolyDataRepresentation();
    rep3->initializeWithMaterial(this->Material3);
    rep3->setPolyData(polyData);
    rep3->addSelfToRenderer(this->renderer());
    rep3->actor()->setTranslation(vesVector3f(0.3, 0.0, 0.0));
    this->DataRep3 = rep3;
  }

  std::string sourceDirectory() {
    return this->SourceDirectory;
  }

  void setSourceDirectory(std::string dir) {
    this->SourceDirectory = dir;
  }

  bool isTesting() {
    return this->IsTesting;
  }

  void setTesting(bool testing) {
    this->IsTesting = testing;
  }

  void loadData()
  {
    std::string filename = this->sourceDirectory() +
      std::string("/Apps/iOS/Kiwi/Kiwi/Data/bunny.vtp");

    this->loadData(filename);
    this->resetView();
  }

  void loadDefaultData()
  {
    this->loadData();
  }

  bool doTesting()
  {
    const double threshold = 10.0;
    const std::string testName = "Light Attribute";

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
    this->initMaterial1();
    this->initMaterial2();
    this->initMaterial3();
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

  std::string dataDirectory() {
    return this->DataDirectory;
  }

  void setDataDirectory(std::string dir) {
    this->DataDirectory = dir;
  }


  std::string       SourceDirectory;
  std::string       DataDirectory;
  bool              IsTesting;

  vesSharedPtr<vesMaterial> Material1;
  vesSharedPtr<vesMaterial> Material2;
  vesSharedPtr<vesMaterial> Material3;

  vesKiwiPolyDataRepresentation* DataRep1;
  vesKiwiPolyDataRepresentation* DataRep2;
  vesKiwiPolyDataRepresentation* DataRep3;
};

}; // end namespace


int
main(int argc, char *argv[])
{
  vesLightAttributeApp app;

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
  app.loadDefaultData();
  app.resizeView(testDriver.width(), testDriver.height());
  app.resetView();
  app.setCameraPosition(vesVector3f(-0.45, 0.25, 0.35));
  app.setCameraFocalPoint(vesVector3f(0.0, 0.1, 0.0));

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
