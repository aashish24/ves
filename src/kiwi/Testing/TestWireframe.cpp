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

#include <vesActor.h>
#include <vesKiwiBaseApp.h>
#include <vesKiwiDataLoader.h>
#include <vesKiwiPolyDataRepresentation.h>
#include <vesKiwiTestHelper.h>
#include <vesMapper.h>
#include <vesShaderProgram.h>
#include <vesUniform.h>
#include <vesBuiltinShaders.h>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>


class vesWireframeApp : public vesKiwiBaseApp {
public:

  vesTypeMacro(vesWireframeApp);

  vesWireframeApp() : vesKiwiBaseApp()
  {
    this->setBackgroundColor(63/255.0, 96/255.0, 144/255.0);
    this->DataRep = 0;
  }

  ~vesWireframeApp()
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
      delete this->DataRep;
      this->DataRep = 0;
    }
  }

  void loadData(const std::string& filename)
  {
    this->unloadData();

    vesKiwiDataLoader loader;
    vtkSmartPointer<vtkPolyData> polyData
      = vtkPolyData::SafeDownCast(loader.loadDataset(filename));
    assert(polyData.GetPointer());

    vesKiwiPolyDataRepresentation* rep = new vesKiwiPolyDataRepresentation();
    rep->initializeWithShader(this->ClipShader);
    rep->setPolyData(polyData);
    rep->addSelfToRenderer(this->renderer());
    rep->actor()->mapper()->enableWireframe(true);
    this->DataRep = rep;
  }


  vesSharedPtr<vesUniform> ClipUniform;
  vesSharedPtr<vesShaderProgram> ClipShader;
  vesKiwiPolyDataRepresentation* DataRep;
};


class MyTestHelper : public vesKiwiTestHelper {
public:

  MyTestHelper()
  {
    mTestApp = vesWireframeApp::Ptr(new vesWireframeApp);
    this->setApp(mTestApp);
  }

  void loadData()
  {
    std::string filename = this->sourceDirectory() +
      std::string("/Apps/iOS/Kiwi/Kiwi/Data/bunny.vtp");

    mTestApp->loadData(filename);
    mTestApp->resetView(false);
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

  void resetView(bool vtkNotUsed(withTransition))
  {
    mTestApp->setCameraPosition(vesVector3f(-0.25, 0.25, 0.25));
    mTestApp->setCameraFocalPoint(vesVector3f(0.0, 0.1, 0.0));
  }

  bool doTesting()
  {
    return this->performBaselineImageTest("Wireframe Standford Bunny");
  }

  vesWireframeApp::Ptr mTestApp;
};


int main(int argc, char *argv[])
{
  MyTestHelper helper;
  return vesKiwiTestHelper::main(argc, argv, helper);
}
