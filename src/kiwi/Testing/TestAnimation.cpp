#include <vesImage.h>
#include <vesKiwiCameraTransition.h>
#include <vesKiwiDataLoader.h>
#include <vesKiwiPolyDataRepresentation.h>
#include <vesKiwiTestHelper.h>
#include <vesKiwiTransition.h>
#include <vesKiwiViewerApp.h>
#include <vesRenderer.h>
#include <vesSetGet.h>
#include <vesShaderProgram.h>
#include <vesBuiltinShaders.h>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <unistd.h> // for usleep

class AnimationTestHelper : public vesKiwiTestHelper
{
public:

  AnimationTestHelper()
    {
    mTestApp = vesKiwiViewerApp::Ptr(new vesKiwiViewerApp);
    this->mDuration = 0.;
    this->setApp(mTestApp);
    }

  virtual ~AnimationTestHelper()
    {
    this->unloadData();
    }

  void unloadData()
    {
    if (this->DataRep)
      {
      this->DataRep->removeSelfFromRenderer(mTestApp->renderer());
      vesKiwiPolyDataRepresentation::Ptr null;
      this->DataRep = null;
      }
    }

  void loadData()
    {
    std::string filename = this->sourceDirectory() +
      std::string("/Apps/iOS/Kiwi/Kiwi/Data/plane.vtp");

    this->unloadData();

    vesKiwiDataLoader loader;
    vtkSmartPointer<vtkPolyData> polyData =
      vtkPolyData::SafeDownCast(loader.loadDataset(filename));
    assert(polyData.GetPointer());

    vesKiwiPolyDataRepresentation::Ptr rep =
      vesKiwiPolyDataRepresentation::Ptr(
        new vesKiwiPolyDataRepresentation());
    rep->initializeWithShader(mTestApp->shaderProgram());
    rep->setPolyData(polyData);
    rep->addSelfToRenderer(mTestApp->renderer());
    this->DataRep = rep;

    mTestApp->resetView(false);
  }

  void initApp()
    {
    // do stuff that needs to happen before scene geometry is loaded
    mTestApp->setCameraRotationInertiaIsEnabled(true);
    }

  double addTransitions()
    {
    double totalDuration = 0.;

    vesVector3f eye(3., -3., -3.);
    vesVector3f aim(0., 0., 0.);
    vesVector3f up(0., 1., 0.);
    mTestApp->addTransition(
      vesKiwiCameraTransition::create(mTestApp->camera())
        ->takeInitialFrame()
        ->setFinalFrame(eye, aim, up)
        ->setDuration(0.5));
    totalDuration += 0.5;
    // Now render until animation complete... (how???)

    return totalDuration;
    }

  bool initTesting()
    {
    this->initApp();
    this->loadData();
    this->mDuration += this->addTransitions();

    return true;
    }

  bool doTesting()
    {
    usleep(1.5 * this->mDuration * 1000000);
    mTestApp->render();
    return this->performBaselineImageTest("Animation");
    }

  vesKiwiPolyDataRepresentation::Ptr DataRep;
  vesKiwiViewerApp::Ptr mTestApp;
  double mDuration;
};

int main(int argc, char *argv[])
{
  AnimationTestHelper helper;
  return vesKiwiTestHelper::main(argc, argv, helper);
}
