#include <vesKiwiViewerApp.h>
#include <vesKiwiTestHelper.h>
#include <vesKiwiEasing.h>

#include <iostream>
#include <string>

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
    return true;
    }

  template<typename EasingPtr>
  bool testEasing(const std::string& name, EasingPtr e)
    {
    std::cout << "%% " << name << " %% can it overshoot? " << e->doesOvershoot() << "\n";
    std::cout << "x = [\n";
    for (int i = 0; i <= 40; ++ i)
      {
      double x = i / 40.0;
      std::cout << "  " << x << " " << e->evaluate(x) << "\n";
      }
    std::cout
      << "];\n"
      << "plot(x(:,1),x(:,2));\n"
      << "pause;\n"
      ;
    if (
      fabs(e->evaluate(0.)) > 1e-8 ||
      fabs(1. - e->evaluate(1.)) > 1e-8)
      {
      // Fail because we should always start at 0 and end at 1.
      // (At least for all the test data).
      return false;
      }
    return true;
    }

  bool doTesting()
    {
    // For now, just evaluate easings and ensure their initial/final
    // values are correct. In the long term, an image test using some
    // transitions would be nice.
    bool ok = true;

    ok &= testEasing("vesKiwiLinearInOutEasing", vesKiwiLinearInOutEasing::create());
    ok &= testEasing("vesKiwiQuadraticInEasing", vesKiwiQuadraticInEasing::create());
    ok &= testEasing("vesKiwiQuadraticOutEasing", vesKiwiQuadraticOutEasing::create());
    ok &= testEasing("vesKiwiQuadraticInOutEasing", vesKiwiQuadraticInOutEasing::create());
    ok &= testEasing("vesKiwiCubicInEasing", vesKiwiCubicInEasing::create());
    ok &= testEasing("vesKiwiCubicOutEasing", vesKiwiCubicOutEasing::create());
    ok &= testEasing("vesKiwiCubicInOutEasing", vesKiwiCubicInOutEasing::create());
    ok &= testEasing("vesKiwiQuarticInEasing", vesKiwiQuarticInEasing::create());
    ok &= testEasing("vesKiwiQuarticOutEasing", vesKiwiQuarticOutEasing::create());
    ok &= testEasing("vesKiwiQuarticInOutEasing", vesKiwiQuarticInOutEasing::create());
    ok &= testEasing("vesKiwiQuinticInEasing", vesKiwiQuinticInEasing::create());
    ok &= testEasing("vesKiwiQuinticOutEasing", vesKiwiQuinticOutEasing::create());
    ok &= testEasing("vesKiwiQuinticInOutEasing", vesKiwiQuinticInOutEasing::create());
    ok &= testEasing("vesKiwiCubicBezierEasing", vesKiwiCubicBezierEasing::create());
    ok &= testEasing("vesKiwiBackInOutEasing", vesKiwiBackInOutEasing::create());
    ok &= testEasing("vesKiwiBounceInEasing", vesKiwiBounceInEasing::create());
    ok &= testEasing("vesKiwiBounceOutEasing", vesKiwiBounceOutEasing::create());
    ok &= testEasing("vesKiwiBounceInOutEasing", vesKiwiBounceInOutEasing::create());
    ok &= testEasing("vesKiwiBounceInQuadraticInOutEasing",
      vesKiwiTwoStageEasing<vesKiwiBounceInEasing,vesKiwiQuadraticInOutEasing>::create()
        ->setSplitPoint(0.75)
        ->setSplitValue(0.3));

    typedef vesKiwiTwoStageEasing<
      vesKiwiTwoStageEasing<vesKiwiLinearInOutEasing,vesKiwiLinearInOutEasing>,
      vesKiwiTwoStageEasing<vesKiwiLinearInOutEasing,vesKiwiLinearInOutEasing>
        > MultiStageEasing;
    MultiStageEasing::Ptr multiStage = MultiStageEasing::create();
    ok &= testEasing("vesKiwiFourStageLinearMM", multiStage);
    multiStage->easeIn()->setSplitPoint(0.25)->setSplitValue(0.75);
    multiStage->easeOut()->setSplitPoint(0.25)->setSplitValue(0.75);
    multiStage->setSplitPoint(0.1);
    ok &= testEasing("vesKiwiFourStageLinearLM", multiStage);
    multiStage->setSplitValue(0.75);
    ok &= testEasing("vesKiwiFourStageLinearLL", multiStage);
    return ok;
    }

  void render()
    {
    this->vesKiwiTestHelper::render();
    }

protected:
  vesKiwiViewerApp::Ptr mKiwiApp;
};

int main(int argc, char* argv[])
{
  EasingTestHelper helper;
  return vesKiwiTestHelper::main(argc, argv, helper);
}
