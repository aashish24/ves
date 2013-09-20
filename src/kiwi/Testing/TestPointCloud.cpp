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
#include <vesKiwiPolyDataRepresentation.h>
#include <vesKiwiTestHelper.h>
#include <vesUniform.h>
#include <vesGeometryData.h>
#include <vesShaderProgram.h>
#include <vesTexture.h>
#include <vesCamera.h>
#include <vesVertexAttribute.h>
#include <vesVertexAttributeKeys.h>
#include <vesBuiltinShaders.h>

#include <vtkXMLPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>



class vesPointApp : public vesKiwiBaseApp {
public:

  vesTypeMacro(vesPointApp);

  vesPointApp() : vesKiwiBaseApp()
  {
    this->setBackgroundColor(0.0, 0.0, 0.0);
  }

  ~vesPointApp()
  {
    this->unloadData();
  }

  void resetView(bool withTransition)
  {
    this->vesKiwiBaseApp::resetView(withTransition);

    // move the camera for a better default view of the cturtle.vtp dataset
    this->camera()->elevation(180);
    this->camera()->roll(180);
  }

  void initShader(const std::string& vertexSource, const std::string fragmentSource)
  {
    this->m_shader = this->addShaderProgram(vertexSource, fragmentSource);
    this->addModelViewMatrixUniform(m_shader);
    this->addProjectionMatrixUniform(m_shader);
    this->addVertexPositionAttribute(m_shader);
    this->addNormalMatrixUniform(m_shader);
    this->addVertexNormalAttribute(m_shader);
    this->addVertexColorAttribute(m_shader);
  }

  void unloadData()
  {
    if (this->m_dataRep) {
      this->m_dataRep->removeSelfFromRenderer(this->renderer());
      this->m_dataRep.reset();
    }
  }

  void loadData(const std::string& filename)
  {
    this->unloadData();

    vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();
    vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();
    assert(polyData->GetNumberOfPoints());

    vesKiwiPolyDataRepresentation::Ptr rep = vesKiwiPolyDataRepresentation::Ptr(new vesKiwiPolyDataRepresentation());
    rep->initializeWithShader(this->m_shader);
    rep->setPolyData(polyData);
    rep->addSelfToRenderer(this->renderer());
    this->m_dataRep = rep;
  }

  vesSharedPtr<vesShaderProgram> m_shader;
  vesSharedPtr<vesGeometryData> m_data;
  vesSharedPtr<vesKiwiPolyDataRepresentation> m_dataRep;

};


class MyTestHelper : public vesKiwiTestHelper {
public:

  MyTestHelper()
  {
    mTestApp = vesPointApp::Ptr(new vesPointApp);
    this->setApp(mTestApp);
  }

  void loadData()
  {
    std::string filename = this->sourceDirectory() +
      std::string("/Apps/iOS/Kiwi/Kiwi/Data/cturtle.vtp");

    mTestApp->loadData(filename);
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
    return this->performBaselineImageTest("PointCloud");
  }

  vesPointApp::Ptr mTestApp;
};


int main(int argc, char *argv[])
{
  MyTestHelper helper;
  return vesKiwiTestHelper::main(argc, argv, helper);
}
