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
#include "vesDepth.h"
#include "vesGeometryData.h"
#include "vesMapper.h"
#include "vesMaterial.h"
#include "vesRenderer.h"
#include "vesShaderProgram.h"
#include "vesTexture.h"

#include "vesKiwiDataConversionTools.h"

#include <vtkNew.h>
#include <vtkTriangleFilter.h>
#include <vtkLookupTable.h>

#include <cassert>

//----------------------------------------------------------------------------
namespace {

void ConvertVertexArrays(vtkDataSet* dataSet, vesSharedPtr<vesGeometryData> geometryData, vtkScalarsToColors* scalarsToColors=NULL)
{
  vtkUnsignedCharArray* colors = vesKiwiDataConversionTools::FindRGBColorsArray(dataSet);
  vtkDataArray* scalars = vesKiwiDataConversionTools::FindScalarsArray(dataSet);
  vtkDataArray* tcoords = vesKiwiDataConversionTools::FindTextureCoordinatesArray(dataSet);
  if (colors)
    {
    vesKiwiDataConversionTools::SetVertexColors(colors, geometryData);
    }
  else if (scalars)
    {
    vtkSmartPointer<vtkScalarsToColors> colorMap = scalarsToColors;
    if (!colorMap)
      {
      colorMap = vesKiwiDataConversionTools::GetRedToBlueLookupTable(scalars->GetRange());
      }
    vesKiwiDataConversionTools::SetVertexColors(scalars, colorMap, geometryData);
    }
  else if (tcoords)
    {
    vesKiwiDataConversionTools::SetTextureCoordinates(tcoords, geometryData);
    }
}

vesSharedPtr<vesGeometryData> GeometryDataFromPolyData(vtkPolyData* polyData)
{
  if (!polyData->GetNumberOfStrips() && !polyData->GetNumberOfPolys() && !polyData->GetNumberOfLines())
    {
    return vesKiwiDataConversionTools::ConvertPoints(polyData);
    }

  // Use triangle filter for now to ensure that models containing
  // polygons other than tris and quads will be rendered correctly.

  vtkNew<vtkTriangleFilter> triangleFilter;
  triangleFilter->PassLinesOn();
  triangleFilter->PassVertsOn();
  triangleFilter->SetInputData(polyData);
  triangleFilter->Update();

  return vesKiwiDataConversionTools::Convert(triangleFilter->GetOutput());
}

}

//----------------------------------------------------------------------------
class vesKiwiPolyDataRepresentation::vesInternal
{
public:

  vesInternal()
  {
  }

  ~vesInternal()
  {
  }

  vesSharedPtr<vesActor>     Actor;
  vesSharedPtr<vesMapper>    Mapper;
  vesSharedPtr<vesMaterial>  Material;
  vesSharedPtr<vesTexture>   Texture;
  vesSharedPtr<vesBlend>     Blend;
  vesSharedPtr<vesDepth>     Depth;
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
void vesKiwiPolyDataRepresentation::setPolyData(vtkPolyData* polyData, vtkScalarsToColors* scalarsToColors)
{
  assert(polyData);
  assert(this->Internal->Mapper);

  vesSharedPtr<vesGeometryData> geometryData = GeometryDataFromPolyData(polyData);
  ConvertVertexArrays(polyData, geometryData, scalarsToColors);
  this->Internal->Mapper->setGeometryData(geometryData);
}

//----------------------------------------------------------------------------
vesSharedPtr<vesGeometryData> vesKiwiPolyDataRepresentation::geometryData() const
{
  if (this->Internal->Mapper) {
    return this->Internal->Mapper->geometryData();
  }
  return vesSharedPtr<vesGeometryData>();
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::addTextureCoordinates(vtkDataArray* textureCoordinates)
{
  assert(this->Internal->Mapper);
  assert(this->Internal->Mapper->geometryData());

  vesGeometryData::Ptr geometryData = this->Internal->Mapper->geometryData();
  vesKiwiDataConversionTools::SetTextureCoordinates(textureCoordinates, geometryData);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setTranslation(const vesVector3f& translation)
{
  assert(this->Internal->Actor);
  this->Internal->Actor->setTranslation(translation);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setShaderProgram(
  vesSharedPtr<vesShaderProgram> shaderProgram)
{
  if (!shaderProgram) {
    return;
  }

  this->Internal->Actor->material()->setShaderProgram(shaderProgram);
}

//----------------------------------------------------------------------------
vesSharedPtr<vesShaderProgram> vesKiwiPolyDataRepresentation::shaderProgram() const
{
  return this->Internal->Actor->material()->shaderProgram();
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::initializeWithShader(
  vesSharedPtr<vesShaderProgram> shaderProgram)
{
  assert(shaderProgram);
  assert(!this->Internal->Mapper && !this->Internal->Actor);

  this->Internal->Mapper = vesSharedPtr<vesMapper>(new vesMapper());

  this->Internal->Actor = vesSharedPtr<vesActor>(new vesActor());
  this->Internal->Actor->setMapper(this->Internal->Mapper);

  this->Internal->Material = vesSharedPtr<vesMaterial>(new vesMaterial());
  this->Internal->Actor->setMaterial(this->Internal->Material);

  this->Internal->Blend = vesSharedPtr<vesBlend>(new vesBlend());
  this->Internal->Depth = vesSharedPtr<vesDepth>(new vesDepth());

  this->Internal->Actor->material()->addAttribute(shaderProgram);
  this->Internal->Actor->material()->addAttribute(this->Internal->Blend);
  this->Internal->Actor->material()->addAttribute(this->Internal->Depth);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setBinNumber(int binNumber)
{
  assert(this->Internal->Actor);
  this->Internal->Actor->material()->setBinNumber(binNumber);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setTexture(vesSharedPtr<vesTexture> texture)
{
  assert(this->Internal->Actor);
  this->Internal->Texture = texture;
  this->Internal->Actor->material()->addAttribute(texture);
}

//----------------------------------------------------------------------------
vesSharedPtr<vesTexture> vesKiwiPolyDataRepresentation::texture() const
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
vesVector4f vesKiwiPolyDataRepresentation::color()
{
  float* color = this->Internal->Actor->mapper()->color();
  return vesVector4f(color[0], color[1], color[2], color[4]);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::addSelfToRenderer(
  vesSharedPtr<vesRenderer> renderer)
{
  assert(renderer);
  renderer->addActor(this->Internal->Actor);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::removeSelfFromRenderer(
  vesSharedPtr<vesRenderer> renderer)
{
  assert(renderer);
  renderer->removeActor(this->Internal->Actor);
}

//----------------------------------------------------------------------------
vesSharedPtr<vesActor> vesKiwiPolyDataRepresentation::actor() const
{
  return this->Internal->Actor;
}

//----------------------------------------------------------------------------
vesSharedPtr<vesMapper> vesKiwiPolyDataRepresentation::mapper() const
{
  return this->Internal->Mapper;
}

//----------------------------------------------------------------------------
int vesKiwiPolyDataRepresentation::numberOfFacets()
{
  vesPrimitive::Ptr tris = this->geometryData()->triangles();
  return tris ? static_cast<int>(tris->size()) : 0;
}

//----------------------------------------------------------------------------
int vesKiwiPolyDataRepresentation::numberOfVertices()
{
  vesSourceData::Ptr points = this->geometryData()->sourceData(vesVertexAttributeKeys::Position);
  return points ? static_cast<int>(points->sizeOfArray()) : 0;
}

//----------------------------------------------------------------------------
int vesKiwiPolyDataRepresentation::numberOfLines()
{
  vesPrimitive::Ptr lines = this->geometryData()->lines();
  return lines ? static_cast<int>(lines->size()) : 0;
}
