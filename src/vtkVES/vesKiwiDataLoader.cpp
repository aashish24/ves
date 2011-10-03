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

#include "vesKiwiDataLoader.h"

#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLImageDataReader.h>
#include <vtkPNGReader.h>
#include <vtkJPEGReader.h>
#include <vtkPolyDataReader.h>
#include <vtkOBJReader.h>
#include <vtkSTLReader.h>
#include <vtkErrorCode.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkBYUReader.h>
#include <vtkSphereSource.h>
#include <vtkPDBReader.h>
#include <vtkGlyph3D.h>
#include <vtkAppendPolyData.h>

#include <cassert>

//----------------------------------------------------------------------------
class vesKiwiDataLoader::vesInternal
{
public:

  vesInternal()
  {
  }

  std::string ErrorTitle;
  std::string ErrorMessage;

};

//----------------------------------------------------------------------------
vesKiwiDataLoader::vesKiwiDataLoader()
{
  this->Internal = new vesInternal();
}

//----------------------------------------------------------------------------
vesKiwiDataLoader::~vesKiwiDataLoader()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
bool vesKiwiDataLoader::hasEnding(const std::string& fullString, const std::string& ending) const
{
  if (fullString.length() > ending.length()) {
    return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
  }
  else {
    return false;
  }
}

//----------------------------------------------------------------------------
bool vesKiwiDataLoader::updateAlgorithmOrSetErrorString(vtkAlgorithm* algorithm)
{
  algorithm->Update();

  unsigned long errorCode = algorithm->GetErrorCode();
  if (errorCode == vtkErrorCode::NoError) {
    return true;
  }
  else {
    this->Internal->ErrorTitle = "Could not read file";
    const char* errorString = vtkErrorCode::GetStringFromErrorCode(errorCode);
    if (errorString && errorString[0]) {
      this->Internal->ErrorMessage = errorString;
    }
    else {
      this->Internal->ErrorMessage = "Could not read the file specified";
    }
    return false;
  }
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkDataSet> vesKiwiDataLoader::datasetFromAlgorithm(vtkAlgorithm* algorithm)
{
  if (!this->updateAlgorithmOrSetErrorString(algorithm))
    {
    return 0;
    }

  vtkDataSet* dataset = vtkDataSet::SafeDownCast(algorithm->GetOutputDataObject(0));
  assert(dataset);

  // VES cannot handle too many points, so handle the error at this point
  const vtkIdType maximumNumberOfPoints = 65536;
  if (vtkPointSet::SafeDownCast(dataset) && dataset->GetNumberOfPoints() > maximumNumberOfPoints)
    {
    this->setMaximumNumberOfPointsErrorMessage();
    return 0;
    }

  return dataset;
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkDataSet> vesKiwiDataLoader::loadDataset(const std::string& filename)
{
  this->Internal->ErrorTitle = std::string();
  this->Internal->ErrorMessage = std::string();

  if (this->hasEnding(filename, "vtk"))
    {
    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(filename.c_str());
    return this->datasetFromAlgorithm(reader);
    }
  if (this->hasEnding(filename, "vtp"))
    {
    vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    reader->SetFileName(filename.c_str());
    return this->datasetFromAlgorithm(reader);
    }
  else if (this->hasEnding(filename, "obj"))
    {
    vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
    reader->SetFileName(filename.c_str());
    return this->datasetFromAlgorithm(reader);
    }
  else if (this->hasEnding(filename, "stl"))
    {
    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName(filename.c_str());
    return this->datasetFromAlgorithm(reader);
    }
  else if (this->hasEnding(filename, "pdb"))
    {
    vtkNew<vtkPDBReader> reader;
    reader->SetFileName(filename.c_str());
    reader->SetHBScale(1.0);
    reader->SetBScale(1.0);

    vtkNew<vtkSphereSource> sphere;
    sphere->SetCenter(0.0, 0.0, 0.0);
    sphere->SetRadius(1.0);
    sphere->SetThetaResolution(8);
    sphere->SetPhiResolution(8);

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputConnection(reader->GetOutputPort());
    glyph->SetSource(sphere->GetOutput());
    glyph->SetOrient(1);
    glyph->SetScaleMode(2);
    glyph->SetScaleFactor(0.25);
    glyph->SetColorMode(1);

    vtkNew<vtkAppendPolyData> append;
    append->AddInputConnection(reader->GetOutputPort());
    append->AddInputConnection(glyph->GetOutputPort());

    return this->datasetFromAlgorithm(append.GetPointer());
    }
  else if (this->hasEnding(filename, ".g"))
    {
    vtkSmartPointer<vtkBYUReader> reader = vtkSmartPointer<vtkBYUReader>::New();
    reader->SetFileName(filename.c_str());
    return this->datasetFromAlgorithm(reader);
    }
  else if (this->hasEnding(filename, "vti"))
    {
    vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
    reader->SetFileName(filename.c_str());
    return datasetFromAlgorithm(reader);
    }
  else if (this->hasEnding(filename, "png"))
    {
    vtkSmartPointer<vtkPNGReader> reader = vtkSmartPointer<vtkPNGReader>::New();
    reader->SetFileName(filename.c_str());
    return datasetFromAlgorithm(reader);
    }
  else if (this->hasEnding(filename, "jpg"))
    {
    vtkSmartPointer<vtkJPEGReader> reader = vtkSmartPointer<vtkJPEGReader>::New();
    reader->SetFileName(filename.c_str());
    return datasetFromAlgorithm(reader);
    }
  else
    {
    this->Internal->ErrorTitle = "Unsupported file format";
    this->Internal->ErrorMessage = "Cannot read files of this format";
    return 0;
    }
}

//----------------------------------------------------------------------------
void vesKiwiDataLoader::setMaximumNumberOfPointsErrorMessage()
{
  this->Internal->ErrorTitle = "Could not read file";
  this->Internal->ErrorMessage = "The file contains more than the supported maximum number of points.";
}

//----------------------------------------------------------------------------
std::string vesKiwiDataLoader::errorTitle() const
{
  return this->Internal->ErrorTitle;
}

//----------------------------------------------------------------------------
std::string vesKiwiDataLoader::errorMessage() const
{
  return this->Internal->ErrorMessage;
}
