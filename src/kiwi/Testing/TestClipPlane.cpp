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

#include <vesKiwiBaseApp.h>
#include <vesKiwiDataLoader.h>
#include <vesKiwiPolyDataRepresentation.h>
#include <vesKiwiTestHelper.h>
#include <vesShaderProgram.h>
#include <vesUniform.h>
#include <vesBuiltinShaders.h>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>


class vesClipApp : public vesKiwiBaseApp {
public:

  vesTypeMacro(vesClipApp);

  vesClipApp() : vesKiwiBaseApp()
  {
    this->setBackgroundColor(63/255.0, 96/255.0, 144/255.0);
    this->DataRep = 0;
  }

  ~vesClipApp()
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
    mClipApp = vesClipApp::Ptr(new vesClipApp);
    this->setApp(mClipApp);
  }

  void loadData()
  {
    std::string filename = this->sourceDirectory() +
      std::string("/Apps/iOS/Kiwi/Kiwi/Data/bunny.vtp");

    mClipApp->loadData(filename);
    mClipApp->resetView(false);
  }

  void initApp()
  {
    mClipApp->initClipShader(vesBuiltinShaders::vesClipPlane_vert(), vesBuiltinShaders::vesClipPlane_frag());
  }

  bool initTesting()
  {
    this->initApp();
    this->loadData();
    return true;
  }

  bool doTesting()
  {
    return this->performBaselineImageTest("Clipped Standford Bunny");
  }

  vesClipApp::Ptr mClipApp;
};


int main(int argc, char *argv[])
{
  MyTestHelper helper;
  return vesKiwiTestHelper::main(argc, argv, helper);
}
