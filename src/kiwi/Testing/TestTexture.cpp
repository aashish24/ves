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
#include <vesKiwiPolyDataRepresentation.h>
#include <vesKiwiTestHelper.h>
#include <vesRenderer.h>
#include <vesSetGet.h>
#include <vesShaderProgram.h>
#include <vesTexture.h>
#include <vesUniform.h>
#include <vesBuiltinShaders.h>

#include <vtkLookupTable.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>


class vesTextureApp : public vesKiwiBaseApp {
public:

  vesTypeMacro(vesTextureApp)

  vesTextureApp() : vesKiwiBaseApp()
  {
    this->setBackgroundColor(63/255.0, 96/255.0, 144/255.0);

    this->DataRep = 0x0;
  }

  ~vesTextureApp()
  {
    this->unloadData();
  }

  void initTextureShader(const std::string& vertexSource, const std::string fragmentSource)
  {
    vesSharedPtr<vesShaderProgram> shaderProgram
      = this->addShaderProgram(vertexSource, fragmentSource);
    this->addModelViewMatrixUniform(shaderProgram);
    this->addProjectionMatrixUniform(shaderProgram);
    this->addVertexPositionAttribute(shaderProgram);
    this->addVertexTextureCoordinateAttribute(shaderProgram);
    this->TextureShader = shaderProgram;
  }

  void create1DTexture()
  {
    this->Image = vesImage::Ptr(new vesImage());
    this->LookupTable = vtkSmartPointer<vtkLookupTable>::New();
    this->LookupTable->Build();

    /// RGBA
    int numberOfColorComponents = 4;
    this->Image->setData(this->LookupTable->GetPointer(0),
                         this->LookupTable->GetNumberOfTableValues()
                         * numberOfColorComponents);
    this->Image->setWidth(this->LookupTable->GetNumberOfTableValues());
    this->Image->setHeight(1);
    this->Image->setPixelDataType(vesColorDataType::UnsignedByte);
    this->Image->setPixelFormat(vesColorDataType::RGBA);

    this->Texture = vesSharedPtr<vesTexture>(new vesTexture());
    this->Texture->setImage(this->Image);
    this->DataRep->setTexture(this->Texture);
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
    vtkSmartPointer<vtkPolyData> polyData =
      vtkPolyData::SafeDownCast(loader.loadDataset(filename));
    assert(polyData.GetPointer());

    vesKiwiPolyDataRepresentation* rep = new vesKiwiPolyDataRepresentation();
    rep->initializeWithShader(this->TextureShader);
    rep->setPolyData(polyData);
    rep->addSelfToRenderer(this->renderer());
    this->DataRep = rep;

    this->create1DTexture();
  }

  vesSharedPtr<vesImage> Image;
  vesSharedPtr<vesTexture> Texture;
  vtkSmartPointer<vtkLookupTable> LookupTable;
  vesSharedPtr<vesShaderProgram> TextureShader;
  vesKiwiPolyDataRepresentation* DataRep;
};


class MyTestHelper : public vesKiwiTestHelper {
public:

  MyTestHelper()
  {
    mTestApp = vesTextureApp::Ptr(new vesTextureApp);
    this->setApp(mTestApp);
  }

  void loadData()
  {
    std::string filename = this->sourceDirectory() +
      std::string("/Apps/iOS/Kiwi/Kiwi/Data/plane.vtp");

    mTestApp->loadData(filename);
    mTestApp->resetView(false);
  }

  void initApp()
  {
    mTestApp->initTextureShader(
      vesBuiltinShaders::vesTestTexture_vert(),
      vesBuiltinShaders::vesTestTexture_frag());
  }

  bool initTesting()
  {
    this->initApp();
    this->loadData();
    return true;
  }

  bool doTesting()
  {
    return this->performBaselineImageTest("Textured Plane");
  }

  vesTextureApp::Ptr mTestApp;
};


int main(int argc, char *argv[])
{
  MyTestHelper helper;
  return vesKiwiTestHelper::main(argc, argv, helper);
}
