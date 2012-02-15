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

#include "vesKiwiImagePlaneDataRepresentation.h"
#include "vesKiwiDataConversionTools.h"
#include "vesSetGet.h"
#include "vesTexture.h"

#include <vtkScalarsToColors.h>
#include <vtkImageData.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkNew.h>
#include <vtkQuad.h>

#include <cassert>

//----------------------------------------------------------------------------
class vesKiwiImagePlaneDataRepresentation::vesInternal
{
public:

  vesInternal()
  {
  }

  ~vesInternal()
  {
  }

  vtkSmartPointer<vtkScalarsToColors> ColorMap;
  vtkSmartPointer<vtkUnsignedCharArray> TextureData;
  vtkSmartPointer<vtkPolyData> ImagePlane;
};

//----------------------------------------------------------------------------
vesKiwiImagePlaneDataRepresentation::vesKiwiImagePlaneDataRepresentation()
{
  this->Internal = new vesInternal();
}

//----------------------------------------------------------------------------
vesKiwiImagePlaneDataRepresentation::~vesKiwiImagePlaneDataRepresentation()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vesKiwiImagePlaneDataRepresentation::setImageData(vtkImageData* imageData)
{
  vtkSmartPointer<vtkPolyData> imagePlane = this->polyDataForImagePlane(imageData);
  this->setPolyData(imagePlane);
  this->Internal->ImagePlane = imagePlane;

  vesSharedPtr<vesTexture> texture = this->texture();
  if (!texture) {
    this->setTexture(vesSharedPtr<vesTexture>(new vesTexture()));
  }

  this->setTextureFromImage(this->texture(), imageData);
}

//----------------------------------------------------------------------------
vesVector2f vesKiwiImagePlaneDataRepresentation::textureSize() const
{
  assert(this->texture());
  vesImage::Ptr image = this->texture()->image();
  return vesVector2f(image->width(), image->height());
}

//----------------------------------------------------------------------------
vtkPolyData* vesKiwiImagePlaneDataRepresentation::imagePlanePolyData()
{
  return this->Internal->ImagePlane;
}

//----------------------------------------------------------------------------
vtkScalarsToColors* vesKiwiImagePlaneDataRepresentation::colorMap()
{
  return this->Internal->ColorMap;
}

//----------------------------------------------------------------------------
void vesKiwiImagePlaneDataRepresentation::setColorMap(vtkScalarsToColors* map)
{
  this->Internal->ColorMap = map;
}

//----------------------------------------------------------------------------
void vesKiwiImagePlaneDataRepresentation::setGrayscaleColorMap(double scalarRange[2])
{
  this->setColorMap(vesKiwiDataConversionTools::GetGrayscaleLookupTable(scalarRange));
}

//----------------------------------------------------------------------------
void vesKiwiImagePlaneDataRepresentation::setShaderProgram(
  vesSharedPtr<vesShaderProgram> shaderProgram)
{
  // Do nothing.
  vesNotUsed(shaderProgram);
}

//----------------------------------------------------------------------------
void vesKiwiImagePlaneDataRepresentation::setTextureFromImage(
  vesSharedPtr<vesTexture> texture, vtkImageData* image)
{
  assert(image);
  assert(image->GetDataDimension() == 2);
  assert(image->GetPointData()->GetScalars());

  vtkSmartPointer<vtkUnsignedCharArray> pixels = vtkUnsignedCharArray::SafeDownCast(image->GetPointData()->GetScalars());

  if (!pixels) {
    vtkScalarsToColors* map = this->colorMap();
    assert(map);
    pixels = vesKiwiDataConversionTools::MapScalars(image->GetPointData()->GetScalars(), map);
  }

  int dimensions[3];
  image->GetDimensions(dimensions);
  const int flatDimension = this->imageFlatDimension(image);

  int width;
  int height;

  if (flatDimension == 2) {
    // XY plane
    width = image->GetDimensions()[0];
    height = image->GetDimensions()[1];
  }
  else if (flatDimension == 1) {
    // XZ plane
    width = image->GetDimensions()[0];
    height = image->GetDimensions()[2];
  }
  else {
    // YZ plane
    width = image->GetDimensions()[1];
    height = image->GetDimensions()[2];
  }

  vesKiwiDataConversionTools::SetTextureData(pixels, texture, width, height);
  this->Internal->TextureData = pixels;
}

//----------------------------------------------------------------------------
int vesKiwiImagePlaneDataRepresentation::imageFlatDimension(vtkImageData* image)
{
  int dimensions[3];
  image->GetDimensions(dimensions);
  for (int i = 0; i < 3; ++i) {
    if (dimensions[i] == 1) {
      return i;
    }
  }
  return -1;
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> vesKiwiImagePlaneDataRepresentation::polyDataForImagePlane(vtkImageData* image)
{
  double bounds[6];
  image->GetBounds(bounds);

  vtkNew<vtkPoints> quadPoints;
  quadPoints->SetNumberOfPoints(4);

  const int flatDimension = vesKiwiImagePlaneDataRepresentation::imageFlatDimension(image);
  if (flatDimension == 2) {
    // XY plane
    quadPoints->SetPoint(0, bounds[0],bounds[2],bounds[4]);
    quadPoints->SetPoint(1, bounds[1],bounds[2],bounds[4]);
    quadPoints->SetPoint(2, bounds[1],bounds[3],bounds[4]);
    quadPoints->SetPoint(3, bounds[0],bounds[3],bounds[4]);
  }
  else if (flatDimension == 1) {
    // XZ plane
    quadPoints->SetPoint(0, bounds[0],bounds[2],bounds[4]);
    quadPoints->SetPoint(1, bounds[1],bounds[2],bounds[4]);
    quadPoints->SetPoint(2, bounds[1],bounds[2],bounds[5]);
    quadPoints->SetPoint(3, bounds[0],bounds[2],bounds[5]);
  }
  else {
    // YZ plane
    quadPoints->SetPoint(0, bounds[0],bounds[2],bounds[4]);
    quadPoints->SetPoint(1, bounds[0],bounds[3],bounds[4]);
    quadPoints->SetPoint(2, bounds[0],bounds[3],bounds[5]);
    quadPoints->SetPoint(3, bounds[0],bounds[2],bounds[5]);
  }

  vtkNew<vtkQuad> quad;
  quad->GetPointIds()->SetId(0, 0);
  quad->GetPointIds()->SetId(1, 1);
  quad->GetPointIds()->SetId(2, 2);
  quad->GetPointIds()->SetId(3, 3);

  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
  polyData->Allocate(1, 1);
  polyData->InsertNextCell(quad->GetCellType(), quad->GetPointIds());
  polyData->SetPoints(quadPoints.GetPointer());

  // add texture coordinates
  vtkNew<vtkFloatArray> tcoords;
  tcoords->SetName("tcoords");
  tcoords->SetNumberOfComponents(2);
  tcoords->SetNumberOfTuples(4);
  tcoords->SetTuple2(0, 0,0);
  tcoords->SetTuple2(1, 1,0);
  tcoords->SetTuple2(2, 1,1);
  tcoords->SetTuple2(3, 0,1);
  polyData->GetPointData()->SetScalars(tcoords.GetPointer());

  return polyData;
}
