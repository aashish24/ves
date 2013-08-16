#include <vesKiwiViewerApp.h>
#include <vesKiwiTestHelper.h>
#include <vesKiwiTransition.h>
#include <vesKiwiScalarTransition.h>
#include <vesKiwiEasing.h>

#include <iostream>
#include <string>

#include <unistd.h>

class EasingTestHelper : public vesKiwiTestHelper
{
public:
  EasingTestHelper()
    {
    mKiwiApp = vesKiwiViewerApp::Ptr(new vesKiwiViewerApp);
    this->setApp(mKiwiApp);
    }

  bool initTesting()
    {
    this->mValue = 1.;
    return true;
    }

  bool doTesting()
    {
    bool ok = true;

    //typedef vesKiwiTwoStageEasing<vesKiwiQuadraticInOutEasing,vesKiwiBounceOutEasing> Tween;
    //Tween::Ptr easy = Tween::Ptr(new Tween())->setSplitPoint(0.75)->setSplitValue(0.3);
    //typedef vesKiwiBounceOutEasing Tween;
    //Tween::Ptr easy = Tween::create(new Tween());

    this->mKiwiApp->addTransition(
      vesKiwiScalarTransition<double>::create(&this->mValue)
        ->takeInitialValue()
        ->setFinalValue(9.0)
        //->setEasing(vesKiwiBounceOutEasing::create())
        ->setEasing(
          vesKiwiTwoStageEasing<vesKiwiPolynomialInOutEasing<5,5>,vesKiwiBounceOutEasing>::create()
            ->setSplitPoint(0.25)
            ->setSplitValue(0.3))
        ->setDuration(0.5));
    //usleep(1.5 * 0.9 * 1000000);
    for (int i = 0; i < 60; ++i)
      {
      render();
      cout << " " << this->mValue << "\n";
      usleep(0.01 * 1000000);
      }

    return ok;
    }

  void render()
    {
    this->vesKiwiTestHelper::render();
    }

protected:
  vesKiwiViewerApp::Ptr mKiwiApp;
  double mValue;
};

int main(int argc, char* argv[])
{
  EasingTestHelper helper;
  return vesKiwiTestHelper::main(argc, argv, helper);
}
