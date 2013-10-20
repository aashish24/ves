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
#include <vesImage.h>
#include <vesKiwiBaseApp.h>
#include <vesKiwiDataLoader.h>
#include <vesKiwiDataConversionTools.h>
#include <vesKiwiPolyDataRepresentation.h>
#include <vesKiwiTestHelper.h>
#include <vesRenderer.h>
#include <vesSetGet.h>
#include <vesShaderProgram.h>
#include <vesTexture.h>
#include <vesUniform.h>
#include <vesBuiltinShaders.h>

#include <vtkImageData.h>
#include <vtkPNGReader.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>


class vesTexturedBackgroundApp : public vesKiwiBaseApp {
public:

  vesTypeMacro(vesTexturedBackgroundApp);

  vesTexturedBackgroundApp() : vesKiwiBaseApp()
  {
    this->DataRep = 0x0;
  }

  ~vesTexturedBackgroundApp()
  {
    this->unloadData();
  }

  void initShader(const std::string& vertexSource, const std::string fragmentSource)
  {
    vesSharedPtr<vesShaderProgram> shaderProgram
      = this->addShaderProgram(vertexSource, fragmentSource);
    this->addModelViewMatrixUniform(shaderProgram);
    this->addProjectionMatrixUniform(shaderProgram);
    this->addNormalMatrixUniform(shaderProgram);
    this->addVertexPositionAttribute(shaderProgram);
    this->addVertexNormalAttribute(shaderProgram);
    this->addVertexColorAttribute(shaderProgram);
    this->ShaderProgram = shaderProgram;
  }

  void setBackgroundImage(const std::string &filename)
  {
    vtkSmartPointer<vtkPNGReader> reader = vtkSmartPointer<vtkPNGReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();

    vesImage::Ptr image = vesKiwiDataConversionTools::ConvertImage(reader->GetOutput());
    this->renderer()->background()->setImage(image);
  }

  void unloadData()
  {
    if (this->DataRep) {
      this->DataRep->removeSelfFromRenderer(this->renderer());
      delete this->DataRep;
      this->DataRep = 0;
    }
  }

  void loadData(const std::string &filename, const std::string &imageFilename)
  {
    this->unloadData();

    vesKiwiDataLoader loader;
    vtkSmartPointer<vtkPolyData> polyData =
      vtkPolyData::SafeDownCast(loader.loadDataset(filename));
    assert(polyData.GetPointer());

    vesKiwiPolyDataRepresentation* rep = new vesKiwiPolyDataRepresentation();
    rep->initializeWithShader(this->ShaderProgram);
    rep->setPolyData(polyData);
    rep->addSelfToRenderer(this->renderer());
    this->DataRep = rep;

    this->setBackgroundImage(imageFilename);
  }

  vesSharedPtr<vesShaderProgram> ShaderProgram;
  vesKiwiPolyDataRepresentation* DataRep;
};


class MyTestHelper : public vesKiwiTestHelper {
public:

  MyTestHelper()
  {
    mTestApp = vesTexturedBackgroundApp::Ptr(new vesTexturedBackgroundApp);
    this->setApp(mTestApp);
  }

  void loadData()
  {
    std::string filename = this->sourceDirectory() +
      std::string("/Apps/iOS/Kiwi/Kiwi/Data/bunny.vtp");

    const std::string imageFilename = this->sourceDirectory() +
      std::string("/Apps/iOS/Kiwi/Kiwi/Data/kiwi_opaque.png");

    mTestApp->loadData(filename, imageFilename);
    mTestApp->resetView(false);
  }

  void initApp()
  {
    mTestApp->initShader(
      vesBuiltinShaders::vesShader_vert(),
      vesBuiltinShaders::vesShader_frag());
  }

  bool initTesting()
  {
    this->initApp();
    this->loadData();
    return true;
  }

  bool doTesting()
  {
    return this->performBaselineImageTest("Textured Background");
  }

  vesTexturedBackgroundApp::Ptr mTestApp;
};


int main(int argc, char *argv[])
{
  MyTestHelper helper;
  return vesKiwiTestHelper::main(argc, argv, helper);
}
