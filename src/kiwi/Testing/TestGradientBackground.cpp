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

#include <vesBackground.h>
#include <vesKiwiBaseApp.h>
#include <vesKiwiDataLoader.h>
#include <vesKiwiSceneTransitions.h>
#include <vesKiwiPolyDataRepresentation.h>
#include <vesKiwiTestHelper.h>
#include <vesRenderer.h>
#include <vesSetGet.h>
#include <vesShaderProgram.h>
#include <vesUniform.h>
#include <vesBuiltinShaders.h>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>


class vesGradientBackgroundApp : public vesKiwiBaseApp {
public:

  vesTypeMacro(vesGradientBackgroundApp);

  vesGradientBackgroundApp()
  {
    this->renderer()->background()->setGradientColor(
      vesVector3f(0.0f/255.0f, 0.0f/255.0f, 2.0f/255.0f),
      vesVector3f(55.0f/255.0f, 55.0f/255.0f, 113.0f/255.0f));
  }

  ~vesGradientBackgroundApp()
  {
    this->unloadData();
  }

  void initClipShader(const std::string& vertexSource, const std::string fragmentSource)
  {
    vesSharedPtr<vesShaderProgram> shaderProgram
      = this->addShaderProgram(vertexSource, fragmentSource);
    this->addModelViewMatrixUniform(shaderProgram);
    this->addProjectionMatrixUniform(shaderProgram);
    this->addNormalMatrixUniform(shaderProgram);
    this->addVertexPositionAttribute(shaderProgram);
    this->addVertexNormalAttribute(shaderProgram);
    this->addVertexColorAttribute(shaderProgram);
    this->addVertexTextureCoordinateAttribute(shaderProgram);
    this->ClipShader = shaderProgram;

    this->ClipUniform = vesSharedPtr<vesUniform>(
      new vesUniform("clipPlaneEquation", vesVector4f(-1.0f, 0.0f, 0.0f, 0.0f)));
    this->ClipShader->addUniform(this->ClipUniform);
  }

  void unloadData()
  {
    if (this->DataRep) {
      this->DataRep->removeSelfFromRenderer(this->renderer());
      vesKiwiPolyDataRepresentation::Ptr null;
      this->DataRep = null;
    }
  }

  vesKiwiPolyDataRepresentation::Ptr loadData(const std::string& filename)
  {
    this->unloadData();

    vesKiwiDataLoader loader;
    vtkSmartPointer<vtkPolyData> polyData = vtkPolyData::SafeDownCast(loader.loadDataset(filename));
    assert(polyData.GetPointer());

    vesKiwiPolyDataRepresentation::Ptr rep =
      vesKiwiPolyDataRepresentation::Ptr(
        new vesKiwiPolyDataRepresentation());
    rep->initializeWithShader(this->ClipShader);
    rep->setPolyData(polyData);
    rep->addSelfToRenderer(this->renderer());
    this->DataRep = rep;
    return rep;
  }

  vesSharedPtr<vesUniform> ClipUniform;
  vesSharedPtr<vesShaderProgram> ClipShader;
  vesSharedPtr<vesKiwiPolyDataRepresentation> DataRep;
};


class MyTestHelper : public vesKiwiTestHelper {
public:

  MyTestHelper()
  {
    mTestApp = vesGradientBackgroundApp::Ptr(new vesGradientBackgroundApp);
    this->setApp(mTestApp);
  }

  void loadData()
  {
    std::string filename = this->sourceDirectory() +
      std::string("/Apps/iOS/Kiwi/Kiwi/Data/bunny.vtp");

    this->mRep =
    mTestApp->loadData(filename);
    mTestApp->resetView(false);
    if (!isTesting())
      {
      this->hop();
      }
  }

  void hop()
    {
      mTestApp->addTransition(
        vesActorTranslationTransition::create(mRep->actor())
          ->setInitialValue(vesVector3f(0., 0.2, 0.))
          ->setFinalValue(vesVector3f(0., 0.0, 0.))
          ->setEasing(vesKiwiBounceOutEasing::create())
          ->setDuration(0.75)
          ->followedBy(
            vesKiwiPolyDataColorTransition::create(mRep)
            ->setFinalValue(vesVector4f(1., 1., 0.5, 1.))
            ->setEasing(vesKiwiQuadraticInOutEasing::create())
            ->setDuration(0.75)
            ->followedBy(
              vesKiwiPolyDataColorTransition::create(mRep)
              ->setFinalValue(vesVector4f(1., 0.8, 1., 1.))
              ->setEasing(vesKiwiQuadraticInOutEasing::create())
              ->setDuration(0.75)
              ->followedBy(
                vesKiwiPolyDataColorTransition::create(mRep)
                ->setFinalValue(vesVector4f(1., 1., 1., 1.))
                ->setEasing(vesKiwiQuadraticInOutEasing::create())
                ->setDuration(0.75)
              )
            )
            ->alsoStart(
              vesActorTranslationTransition::create(mRep->actor())
              ->setFinalValue(vesVector3f(0., 0.2, 0.))
              ->setEasing(vesKiwiQuadraticOutEasing::create())
              ->setDuration(0.5)
              ->followedBy(
                vesActorTranslationTransition::create(mRep->actor())
                ->setFinalValue(vesVector3f(0., 0.0, 0.))
                ->setEasing(vesKiwiBounceOutEasing::create())
                ->setDuration(0.75)
                ->followedBy(
                  vesActorTranslationTransition::create(mRep->actor())
                  ->setFinalValue(vesVector3f(0., 0.2, 0.))
                  ->setEasing(vesKiwiQuadraticOutEasing::create())
                  ->setDuration(0.5)
                  ->followedBy(
                    vesActorTranslationTransition::create(mRep->actor())
                    ->setFinalValue(vesVector3f(0., 0.0, 0.))
                    ->setEasing(vesKiwiBounceOutEasing::create())
                    ->setDuration(0.75)
                  )
                )
              )
            )
          )
        );
    }

  void handleKeyboard(unsigned char key, int x, int y)
  {
    if (key == 'h') {
      this->hop();
    } else {
      vesKiwiTestHelper::handleKeyboard(key, x, y);
    }
  }

  void initApp()
  {
    mTestApp->initClipShader(
      vesBuiltinShaders::vesClipPlane_vert(),
      vesBuiltinShaders::vesClipPlane_frag());
  }

  bool initTesting()
  {
    this->initApp();
    this->loadData();
    return true;
  }

  bool doTesting()
  {
    return this->performBaselineImageTest("Gradient Background");
  }

  vesGradientBackgroundApp::Ptr mTestApp;
  vesKiwiPolyDataRepresentation::Ptr mRep;
};


int main(int argc, char *argv[])
{
  MyTestHelper helper;
  return vesKiwiTestHelper::main(argc, argv, helper);
}
