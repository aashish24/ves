#include <vesKiwiViewerApp.h>
#include <vesKiwiTestHelper.h>
#include <vesKiwiTransition.h>
#include <vesKiwiScalarTransition.h>
#include <vesKiwiEasing.h>

#include <iostream>
#include <string>

#include <unistd.h>

class TransitionTestHelper : public vesKiwiTestHelper
{
public:
  TransitionTestHelper()
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
    // Verify that the transitioned value starts at
    // the proper initial value (1) and ends at the
    // proper final value (11).
    bool ok = (this->mValue == 1.);

    this->mKiwiApp->addTransition(
      vesKiwiScalarTransition<double>::create(&this->mValue)
        ->takeInitialValue()
        ->setFinalValue(11.0)
        ->setEasing(
          vesKiwiTwoStageEasing<vesKiwiPolynomialInOutEasing<5,5>,vesKiwiBounceOutEasing>::create()
            ->setSplitPoint(0.25)
            ->setSplitValue(0.3))
        ->setDuration(0.5));

    for (int i = 0; i < 60; ++i)
      {
      render();
      cout << " " << this->mValue << "\n";
      usleep(0.01 * 1000000);
      }
    // Note that 60 * 0.01 * 1000000 usec = 0.6 s, which is longer than
    // the duration of the transition (0.5 s), so the transition should
    // easily have ended by now. Verify it.
    ok &= this->mValue == 11.0;

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
  TransitionTestHelper helper;
  return vesKiwiTestHelper::main(argc, argv, helper);
}
