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

#include "vesTriangleData.h"
#include "vesMapper.h"
#include "vesActor.h"
#include "vesRenderer.h"
#include "vesTriangleData.h"
#include "vesShaderProgram.h"

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
  }

  ~vesInternal()
  {
    delete this->Actor;
    delete this->Mapper->triangleData();
    delete this->Mapper;
  }

  vesActor* Actor;
  vesMapper* Mapper;
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
  delete this->Internal->Mapper->triangleData();
  this->Internal->Mapper->setTriangleData(triangleData);
}

//----------------------------------------------------------------------------
vesTriangleData* vesKiwiDataRepresentation::triangleData() const
{
  if (this->Internal->Mapper) {
    return this->Internal->Mapper->triangleData();
  }
  return 0;
}

//----------------------------------------------------------------------------
void vesKiwiDataRepresentation::initializeWithShader(vesShaderProgram* shaderProgram)
{
  assert(shaderProgram);
  assert(!this->Internal->Mapper && !this->Internal->Actor);

  this->Internal->Mapper = new vesMapper();
  this->Internal->Mapper->setTriangleData(new vesTriangleData);
  this->Internal->Actor = new vesActor(this->Internal->Mapper);
  this->Internal->Actor->appearance()->addAttribute(shaderProgram);
  this->Internal->Actor->setColor(0.8, 0.8, 0.8, 1.0);
}

//----------------------------------------------------------------------------
void vesKiwiDataRepresentation::addSelfToRenderer(vesRenderer* renderer)
{
  assert(renderer);
  renderer->AddActor(this->Internal->Actor);
}

//----------------------------------------------------------------------------
void vesKiwiDataRepresentation::removeSelfFromRenderer(vesRenderer* renderer)
{
  assert(renderer);
  renderer->RemoveActor(this->Internal->Actor);
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
