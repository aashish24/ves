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

#include "vesKiwiPolyDataRepresentation.h"

#include "vesActor.h"
#include "vesBlend.h"
#include "vesMapper.h"
#include "vesMaterial.h"
#include "vesRenderer.h"
#include "vesTriangleData.h"
#include "vesShaderProgram.h"
#include "vesTexture.h"

#include "vesDataConversionTools.h"

#include <vtkNew.h>
#include <vtkTriangleFilter.h>
#include <vtkLookupTable.h>

#include <cassert>

//----------------------------------------------------------------------------
namespace {

void ConvertVertexArrays(vtkDataSet* dataSet, vesTriangleData* triangleData)
{
  vtkUnsignedCharArray* colors = vesDataConversionTools::FindRGBColorsArray(dataSet);
  vtkDataArray* scalars = vesDataConversionTools::FindScalarsArray(dataSet);
  vtkDataArray* tcoords = vesDataConversionTools::FindTextureCoordinatesArray(dataSet);
  if (colors)
    {
    vesDataConversionTools::SetVertexColors(colors, triangleData);
    }
  else if (scalars)
    {
    vtkSmartPointer<vtkLookupTable> lut = vesDataConversionTools::GetRedToBlueLookupTable(scalars->GetRange());
    vesDataConversionTools::SetVertexColors(scalars, lut, triangleData);
    }
  else if (tcoords)
    {
    vesDataConversionTools::SetTextureCoordinates(tcoords, triangleData);
    }
}

vesTriangleData* TriangleDataFromPolyData(vtkPolyData* polyData)
{
  // Use triangle filter for now to ensure that models containing
  // polygons other than tris and quads will be rendered correctly.

  vtkNew<vtkTriangleFilter> triangleFilter;
  triangleFilter->PassLinesOn();
  triangleFilter->SetInput(polyData);
  triangleFilter->Update();
  polyData = triangleFilter->GetOutput();
  vesTriangleData* triangleData = vesDataConversionTools::Convert(polyData);
  ConvertVertexArrays(polyData, triangleData);
  return triangleData;
}
};

//----------------------------------------------------------------------------
class vesKiwiPolyDataRepresentation::vesInternal
{
public:

  vesInternal()
  {
    this->Actor = 0;
    this->Mapper = 0;
    this->Material = 0;
    this->Texture = 0;
    this->Blend = 0;
  }

  ~vesInternal()
  {
    delete this->Actor;
    delete this->Mapper->data();
    delete this->Mapper;
    delete this->Material;
    delete this->Texture;
    delete this->Blend;
  }

  vesActor*     Actor;
  vesMapper*    Mapper;
  vesMaterial*  Material;
  vesTexture*   Texture;
  vesBlend*     Blend;
};

//----------------------------------------------------------------------------
vesKiwiPolyDataRepresentation::vesKiwiPolyDataRepresentation()
{
  this->Internal = new vesInternal();
}

//----------------------------------------------------------------------------
vesKiwiPolyDataRepresentation::~vesKiwiPolyDataRepresentation()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setPolyData(vtkPolyData* polyData)
{
  assert(polyData);
  assert(this->Internal->Mapper);

  vesTriangleData* triangleData = TriangleDataFromPolyData(polyData);
  delete this->Internal->Mapper->data();
  this->Internal->Mapper->setData(triangleData);
}

//----------------------------------------------------------------------------
vesTriangleData* vesKiwiPolyDataRepresentation::triangleData() const
{
  if (this->Internal->Mapper) {
    return this->Internal->Mapper->data();
  }
  return 0;
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setTranslation(const vesVector3f& translation)
{
  assert(this->Internal->Actor);
  this->Internal->Actor->setTranslation(translation);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::initializeWithShader(vesShaderProgram* shaderProgram)
{
  assert(shaderProgram);
  assert(!this->Internal->Mapper && !this->Internal->Actor);

  this->Internal->Mapper = new vesMapper();
  this->Internal->Mapper->setData(new vesTriangleData);

  this->Internal->Actor = new vesActor();
  this->Internal->Actor->setMapper(this->Internal->Mapper);

  this->Internal->Material = new vesMaterial();
  this->Internal->Actor->setMaterial(this->Internal->Material);

  this->Internal->Blend = new vesBlend();

  this->Internal->Actor->material()->addAttribute(shaderProgram);
  this->Internal->Actor->material()->addAttribute(this->Internal->Blend);

  this->Internal->Actor->mapper()->setColor(0.9, 0.9, 0.9, 1.0);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setBinNumber(int binNumber)
{
  assert(this->Internal->Actor);
  this->Internal->Actor->material()->setBinNumber(binNumber);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setTexture(vesTexture* texture)
{
  assert(this->Internal->Actor);
  this->Internal->Texture = texture;
  this->Internal->Actor->material()->addAttribute(texture);
}

//----------------------------------------------------------------------------
vesTexture* vesKiwiPolyDataRepresentation::texture() const
{
  return this->Internal->Texture;
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setColor(double r, double g, double b, double a)
{
  assert(this->Internal->Actor && this->Internal->Actor->mapper());
  this->Internal->Actor->mapper()->setColor(r, g, b, a);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::addSelfToRenderer(vesRenderer* renderer)
{
  assert(renderer);
  renderer->addActor(this->Internal->Actor);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::removeSelfFromRenderer(vesRenderer* renderer)
{
  assert(renderer);
  renderer->removeActor(this->Internal->Actor);
}

//----------------------------------------------------------------------------
vesActor* vesKiwiPolyDataRepresentation::actor() const
{
  return this->Internal->Actor;
}

//----------------------------------------------------------------------------
vesMapper* vesKiwiPolyDataRepresentation::mapper() const
{
  return this->Internal->Mapper;
}

//----------------------------------------------------------------------------
int vesKiwiPolyDataRepresentation::numberOfFacets()
{
  return static_cast<int>(this->triangleData()->GetTriangles().size());
}

//----------------------------------------------------------------------------
int vesKiwiPolyDataRepresentation::numberOfVertices()
{
  return static_cast<int>(this->triangleData()->GetPoints().size());
}

//----------------------------------------------------------------------------
int vesKiwiPolyDataRepresentation::numberOfLines()
{
  return static_cast<int>(this->triangleData()->GetLines().size());
}
