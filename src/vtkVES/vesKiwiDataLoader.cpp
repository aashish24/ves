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

#include "vesTriangleData.h"
#include "vtkPolyDataToTriangleData.h"

#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyDataReader.h>
#include <vtkOBJReader.h>
#include <vtkSTLReader.h>
#include <vtkTriangleFilter.h>
#include <vtkErrorCode.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
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
vesTriangleData* vesKiwiDataLoader::dataFromPolyDataAlgorithm(vtkAlgorithm* algorithm)
{
  if (!this->updateAlgorithmOrSetErrorString(algorithm))
    {
    return 0;
    }

  vtkPolyData* polyData = vtkPolyData::SafeDownCast(algorithm->GetOutputDataObject(0));
  assert(polyData);

  vtkIdType maximumNumberOfPoints = 65536;
  if (polyData->GetNumberOfPoints() > maximumNumberOfPoints)
    {
    this->setMaximumNumberOfPointsErrorMessage();
    return 0;
    }


  // Always use triangle filter for now.  This will ensure that models containing
  // polygons other than tris and quads will be rendered correctly.
  const bool useTriangleFilter = true;
  if (useTriangleFilter)
    {
    vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
    triangleFilter->PassLinesOn();
    triangleFilter->SetInputConnection(algorithm->GetOutputPort());
    if (!this->updateAlgorithmOrSetErrorString(triangleFilter))
      {
      return 0;
      }
    polyData = triangleFilter->GetOutput();
    vesTriangleData* triangleData = vtkPolyDataToTriangleData::Convert(polyData);
    vtkPolyDataToTriangleData::ComputeVertexColorFromScalars(polyData, triangleData);
    return triangleData;
    }
  else
    {
    return vtkPolyDataToTriangleData::Convert(polyData);
    }
}

//----------------------------------------------------------------------------
vesTriangleData* vesKiwiDataLoader::loadDataset(const std::string& filename)
{
  vesTriangleData* newData = 0;

  if (this->hasEnding(filename, "vtk"))
    {
    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(filename.c_str());
    return this->dataFromPolyDataAlgorithm(reader);
    }
  if (this->hasEnding(filename, "vtp"))
    {
    vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    reader->SetFileName(filename.c_str());
    return this->dataFromPolyDataAlgorithm(reader);
    }
  else if (this->hasEnding(filename, "obj"))
    {
    vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
    reader->SetFileName(filename.c_str());
    return this->dataFromPolyDataAlgorithm(reader);
    }
  else if (this->hasEnding(filename, "stl"))
    {
    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName(filename.c_str());
    return this->dataFromPolyDataAlgorithm(reader);
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

    return this->dataFromPolyDataAlgorithm(append.GetPointer());
    }
  else if (this->hasEnding(filename, "g"))
    {
    vtkSmartPointer<vtkBYUReader> reader = vtkSmartPointer<vtkBYUReader>::New();
    reader->SetFileName(filename.c_str());
    return this->dataFromPolyDataAlgorithm(reader);
    }
  else
    {
    this->Internal->ErrorTitle = "Unsupported file format";
    this->Internal->ErrorMessage = "Cannot read files of this format";
    return 0;
    }


  this->Internal->ErrorTitle = std::string();
  this->Internal->ErrorMessage = std::string();
  return newData;

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
