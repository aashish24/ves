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

#include "vesKiwiDataRepresentation.h"

#include "vesActor.h"
#include "vesBlend.h"
#include "vesMapper.h"
#include "vesMaterial.h"
#include "vesRenderer.h"
#include "vesTriangleData.h"
#include "vesShaderProgram.h"
#include "vesTexture.h"

#include "vtkPolyDataToTriangleData.h"

#include <vtkNew.h>
#include <vtkTriangleFilter.h>

#include <cassert>

//----------------------------------------------------------------------------
namespace {
vesTriangleData* triangleDataFromPolyData(vtkPolyData* polyData)
{
  // Always use triangle filter for now.  This will ensure that models containing
  // polygons other than tris and quads will be rendered correctly.
  const bool useTriangleFilter = true;
  if (useTriangleFilter)
    {
    vtkNew<vtkTriangleFilter> triangleFilter;
    triangleFilter->PassLinesOn();
    triangleFilter->SetInput(polyData);
    triangleFilter->Update();
    polyData = triangleFilter->GetOutput();
    vesTriangleData* triangleData = vtkPolyDataToTriangleData::Convert(polyData);
    vtkPolyDataToTriangleData::ComputeVertexColorFromScalars(polyData, triangleData);
    vtkPolyDataToTriangleData::ConvertTextureCoordinates(polyData, triangleData);
    return triangleData;
    }
  else
    {
    return vtkPolyDataToTriangleData::Convert(polyData);
    }
}
};

//----------------------------------------------------------------------------
class vesKiwiDataRepresentation::vesInternal
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
vesKiwiDataRepresentation::vesKiwiDataRepresentation()
{
  this->Internal = new vesInternal();
}

//----------------------------------------------------------------------------
vesKiwiDataRepresentation::~vesKiwiDataRepresentation()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vesKiwiDataRepresentation::setDataSet(vtkDataSet* dataSet)
{
  vtkPolyData* polyData = vtkPolyData::SafeDownCast(dataSet);
  assert(polyData);
  assert(this->Internal->Mapper);

  vesTriangleData* triangleData = triangleDataFromPolyData(polyData);
  delete this->Internal->Mapper->data();
  this->Internal->Mapper->setData(triangleData);
}

//----------------------------------------------------------------------------
vesTriangleData* vesKiwiDataRepresentation::triangleData() const
{
  if (this->Internal->Mapper) {
    return this->Internal->Mapper->data();
  }
  return 0;
}

//----------------------------------------------------------------------------
void vesKiwiDataRepresentation::initializeWithShader(vesShaderProgram* shaderProgram)
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
void vesKiwiDataRepresentation::setTexture(vesTexture* texture)
{
  this->Internal->Texture = texture;
  this->Internal->Actor->material()->addAttribute(texture);
}

//----------------------------------------------------------------------------
vesTexture* vesKiwiDataRepresentation::texture() const
{
  return this->Internal->Texture;
}

//----------------------------------------------------------------------------
void vesKiwiDataRepresentation::setColor(double r, double g, double b, double a)
{
  assert(this->Internal->Actor && this->Internal->Actor->mapper());
  this->Internal->Actor->mapper()->setColor(r, g, b, a);
}

//----------------------------------------------------------------------------
void vesKiwiDataRepresentation::addSelfToRenderer(vesRenderer* renderer)
{
  assert(renderer);
  renderer->addActor(this->Internal->Actor);
}

//----------------------------------------------------------------------------
void vesKiwiDataRepresentation::removeSelfFromRenderer(vesRenderer* renderer)
{
  assert(renderer);
  renderer->removeActor(this->Internal->Actor);
}

//----------------------------------------------------------------------------
vesActor* vesKiwiDataRepresentation::actor() const
{
  return this->Internal->Actor;
}

//----------------------------------------------------------------------------
vesMapper* vesKiwiDataRepresentation::mapper() const
{
  return this->Internal->Mapper;
}
