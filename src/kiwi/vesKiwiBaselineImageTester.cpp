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

#include "vesKiwiBaselineImageTester.h"
#include "vesGL.h"
#include "vesGLTypes.h"
#include "vesKiwiBaseApp.h"

#include <vtkErrorCode.h>
#include <vtkImageData.h>
#include <vtkImageDifference.h>
#include <vtkImageShiftScale.h>
#include <vtkNew.h>
#include <vtkPNGReader.h>
#include <vtkPNGWriter.h>

#include <iostream>

//----------------------------------------------------------------------------
vesKiwiBaselineImageTester::vesKiwiBaselineImageTester() : mApp(0)
{
}

//----------------------------------------------------------------------------
vesKiwiBaselineImageTester::~vesKiwiBaselineImageTester()
{
}

//----------------------------------------------------------------------------
vesKiwiBaseApp* vesKiwiBaselineImageTester::app()
{
  return this->mApp;
}

//----------------------------------------------------------------------------
void vesKiwiBaselineImageTester::setApp(vesKiwiBaseApp* app)
{
  this->mApp = app;
}

//----------------------------------------------------------------------------
const std::string& vesKiwiBaselineImageTester::baselineImageDirectory()
{
  return this->mBaselineImageDirectory;
}

//----------------------------------------------------------------------------
void vesKiwiBaselineImageTester::setBaselineImageDirectory(const std::string& baselineImageDirectory)
{
  this->mBaselineImageDirectory = baselineImageDirectory;
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkImageData> vesKiwiBaselineImageTester::imageFromFile(const std::string& filename)
{
  vtkNew<vtkPNGReader> reader;
  reader->SetFileName(filename.c_str());
  reader->Update();

  if (reader->GetErrorCode() != vtkErrorCode::NoError || reader->GetOutput()->GetNumberOfPoints() <= 1) {
    return 0;
  }

  return reader->GetOutput();
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkImageData> vesKiwiBaselineImageTester::imageFromRenderView()
{
  int width = this->app()->viewWidth();
  int height = this->app()->viewHeight();

  vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
  image->SetDimensions(width, height, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

  unsigned char* outPtr = static_cast<unsigned char*>(image->GetScalarPointer(0, 0, 0));
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, (void*)outPtr);
  return image;
}

//----------------------------------------------------------------------------
void vesKiwiBaselineImageTester::writeImage(vtkImageData* image, const std::string& filename)
{
  vtkNew<vtkPNGWriter> writer;
  writer->SetInputData(image);
  writer->SetFileName(filename.c_str());
  writer->Write();
}

//----------------------------------------------------------------------------
bool vesKiwiBaselineImageTester::performTest(const std::string& testName, double threshold)
{
  std::string inFile = this->baselineImageDirectory() + "/" + testName + ".png";
  std::string outFile = testName + ".png";

  vtkSmartPointer<vtkImageData> baselineImage = this->imageFromFile(inFile);
  vtkSmartPointer<vtkImageData> image = this->imageFromRenderView();

  if (!baselineImage) {
    std::cout << "Could not read baseline image: " << inFile << std::endl;
    this->writeImage(image, outFile.c_str());
    return false;
  }

  vtkNew<vtkImageDifference> imageDiff;
  imageDiff->SetInputData(image);
  imageDiff->SetImageData(baselineImage);
  imageDiff->Update();

  double thresholdError = imageDiff->GetThresholdedError();
  if (thresholdError > threshold) {
    std::cout << "Test '" << testName << "' image difference test failed with thresholded error: " << thresholdError << std::endl;
    vtkNew<vtkImageShiftScale> gamma;
    gamma->SetInputConnection(imageDiff->GetOutputPort());
    gamma->SetShift(0);
    gamma->SetScale(10);
    gamma->Update();

    this->writeImage(gamma->GetOutput(), testName + ".diff.png");
    this->writeImage(image, outFile);
    return false;
  }

  std::cout << "Test '" << testName << "' image difference test passed with thresholded error: " << thresholdError << std::endl;
  return true;
}
