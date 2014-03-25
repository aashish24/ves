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
#include <vtkTriangle.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>

#include "vesKiwiDataConversionTools.h"

class vesCellDataApp : public vesKiwiBaseApp {
public:

  vesTypeMacro(vesCellDataApp);

  vesCellDataApp() : vesKiwiBaseApp()
  {
    this->setBackgroundColor(63/255.0, 96/255.0, 144/255.0);
    this->DataRep = 0;
  }

  ~vesCellDataApp()
  {
    this->unloadData();
  }

  void initDefaultShader(const std::string& vertexSource, const std::string fragmentSource)
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
    this->DefaultShader = shaderProgram;
  }

  void unloadData()
  {
    if (this->DataRep) {
      this->DataRep->removeSelfFromRenderer(this->renderer());
      delete this->DataRep;
      this->DataRep = 0;
    }
  }

  void loadData()
  {
     // Setup points
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 1.0, 0.0);

    // Setup scalars
    vtkSmartPointer<vtkDoubleArray> scalars = vtkSmartPointer<vtkDoubleArray>::New();
    scalars->SetNumberOfComponents(1);
    scalars->SetName("ScalarsArray");
    scalars->InsertNextTuple1(0.0);
    scalars->InsertNextTuple1(1.0);

    vtkSmartPointer<vtkCellArray> triangles =vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkTriangle> triangle1 = vtkSmartPointer<vtkTriangle>::New();
    triangle1->GetPointIds()->SetId(0, 0);
    triangle1->GetPointIds()->SetId(1, 1);
    triangle1->GetPointIds()->SetId(2, 2);
    triangles->InsertNextCell(triangle1);
    vtkSmartPointer<vtkTriangle> triangle2 = vtkSmartPointer<vtkTriangle>::New();
    triangle2->GetPointIds()->SetId(0, 2);
    triangle2->GetPointIds()->SetId(1, 1);
    triangle2->GetPointIds()->SetId(2, 3);
    triangles->InsertNextCell(triangle2);

    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);
    polydata->SetPolys(triangles);
    polydata->GetCellData()->SetScalars(scalars);
    ////////////

    vesKiwiPolyDataRepresentation* rep = new vesKiwiPolyDataRepresentation();
    rep->initializeWithShader(this->DefaultShader);
    rep->setPolyData(polydata, true);
    rep->colorByScalars("ScalarsArray");
    rep->addSelfToRenderer(this->renderer());
    this->DataRep = rep;
  }

  vesSharedPtr<vesUniform> ClipUniform;
  vesSharedPtr<vesShaderProgram> DefaultShader;
  vesKiwiPolyDataRepresentation* DataRep;
};


class MyTestHelper : public vesKiwiTestHelper {
public:

  MyTestHelper()
  {
    mCellDataApp = vesCellDataApp::Ptr(new vesCellDataApp);
    this->setApp(mCellDataApp);
  }

  void loadData()
  {
    mCellDataApp->loadData();
    mCellDataApp->resetView(false);
  }

  void initApp()
  {
    mCellDataApp->initDefaultShader(vesBuiltinShaders::vesShader_vert(),
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
    return this->performBaselineImageTest("Cell Data");
  }

  vesCellDataApp::Ptr mCellDataApp;
};


int main(int argc, char *argv[])
{
  MyTestHelper helper;
  return vesKiwiTestHelper::main(argc, argv, helper);
}
