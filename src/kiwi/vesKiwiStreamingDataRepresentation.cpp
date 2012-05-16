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

#include "vesKiwiStreamingDataRepresentation.h"

#include "vesRenderer.h"
#include "vesCamera.h"
#include "vesMapper.h"
#include "vesGeometryData.h"
#include "vesActor.h"
#include "vesShaderProgram.h"
#include "vesKiwiDataConversionTools.h"
#include "vesKiwiPolyDataRepresentation.h"

#include <vtkPolyData.h>
#include <vtkTimerLog.h>
#include <vtkNew.h>
#include <vtkDoubleArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkSphereSource.h>
#include <vtkClientSocket.h>
#include <vtkCharArray.h>
#include <vtkFloatArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkShortArray.h>
#include <vtkMultiThreader.h>
#include <vtkMutexLock.h>

#include <vtksys/SystemTools.hxx>

#include <vector>
#include <cassert>
#include <sstream>

//----------------------------------------------------------------------------
class vesKiwiStreamingDataRepresentation::vesInternal
{
public:

  vesInternal()
  {
    this->ShouldQuit = false;
    this->HaveNew = false;
    this->ClientThreadId = -1;
  }

  ~vesInternal()
  {
  }

  int ClientThreadId;
  bool HaveNew;
  bool ShouldQuit;

  vtkNew<vtkClientSocket> Comm;

  vesGeometryData::Ptr GeometryData;

  vtkNew<vtkMultiThreader> MultiThreader;
  vtkNew<vtkMutexLock> Lock;

  vesKiwiPolyDataRepresentation::Ptr PolyDataRep;
  vesShaderProgram::Ptr GeometryShader;
};

//----------------------------------------------------------------------------
vesKiwiStreamingDataRepresentation::vesKiwiStreamingDataRepresentation()
{
  this->Internal = new vesInternal();
}

//----------------------------------------------------------------------------
vesKiwiStreamingDataRepresentation::~vesKiwiStreamingDataRepresentation()
{
  this->Internal->Lock->Lock();
  this->Internal->ShouldQuit = true;
  this->Internal->Lock->Unlock();
  delete this->Internal;
}

//----------------------------------------------------------------------------
bool vesKiwiStreamingDataRepresentation::connectToServer(const std::string& host, int port)
{
  return (this->Internal->Comm->ConnectToServer(host.c_str(), port) == 0);
}

namespace {

vesGeometryData::Ptr CreateGeometryData(vtkShortArray* points, vtkUnsignedCharArray* colors)
{
  const int numberOfPoints = points->GetNumberOfTuples()*points->GetNumberOfComponents() / 3;

  vesSharedPtr<vesGeometryData> output(new vesGeometryData());
  vesSourceDataP3f::Ptr sourceData(new vesSourceDataP3f());

  vesVertexDataP3f vertexData;
  for (int i = 0; i < numberOfPoints; ++i) {
    vertexData.m_position[0] = points->GetValue(i*3 + 0);
    vertexData.m_position[1] = points->GetValue(i*3 + 1);
    vertexData.m_position[2] = points->GetValue(i*3 + 2);
    sourceData->pushBack(vertexData);
  }

  output->addSource(sourceData);
  output->setName("PolyData");

  vesPrimitive::Ptr pointPrimitive (new vesPrimitive());
  pointPrimitive->setPrimitiveType(vesPrimitiveRenderType::Points);
  pointPrimitive->setIndexCount(1);
  output->addPrimitive(pointPrimitive);


  vesKiwiDataConversionTools::SetVertexColors(colors, output);
  return output;
}

vesGeometryData::Ptr ReceiveGeometryData(vtkClientSocket* comm)
{
  vtkNew<vtkShortArray> points;
  vtkNew<vtkUnsignedCharArray> colors;
  double startTime = vtkTimerLog::GetUniversalTime();

  int numberOfPoints = 0;

  if (!comm->Send(&numberOfPoints, 4)) {
    return vesGeometryData::Ptr();
  }
  if (!comm->Receive(&numberOfPoints, 4)) {
    return vesGeometryData::Ptr();
  }

  if (!numberOfPoints) {
    return vesGeometryData::Ptr(new vesGeometryData);
  }

  points->SetNumberOfTuples(numberOfPoints*3);
  colors->SetNumberOfComponents(3);
  colors->SetNumberOfTuples(numberOfPoints);

  if (!comm->Receive(points->GetVoidPointer(0), numberOfPoints * 3 * 2)) {
    return vesGeometryData::Ptr();
  }
  if (!comm->Receive(colors->GetVoidPointer(0), numberOfPoints * 3)) {
    return vesGeometryData::Ptr();
  }

  double elapsed = vtkTimerLog::GetUniversalTime() - startTime;
  double kb = points->GetActualMemorySize() + colors->GetActualMemorySize();
  double mb = kb/1024.0;

  std::cout << numberOfPoints << " points in " << elapsed << " seconds "
            << "(" << mb/ elapsed << "mb/s)" << std::endl;


  return CreateGeometryData(points.GetPointer(), colors.GetPointer());
}

//----------------------------------------------------------------------------
VTK_THREAD_RETURN_TYPE ClientLoop(void* arg)
{
  vtkMultiThreader::ThreadInfo* threadInfo = static_cast<vtkMultiThreader::ThreadInfo*>(arg);

  vesKiwiStreamingDataRepresentation::vesInternal* selfInternal =
    static_cast<vesKiwiStreamingDataRepresentation::vesInternal*>(threadInfo->UserData);

  bool shouldQuit = false;
  while (!shouldQuit) {

      vesGeometryData::Ptr geometryData = ReceiveGeometryData(selfInternal->Comm.GetPointer());

      if (!geometryData) {
        break;
      }

      selfInternal->Lock->Lock();
      selfInternal->GeometryData = geometryData;
      selfInternal->HaveNew = true;
      shouldQuit = selfInternal->ShouldQuit;
      selfInternal->Lock->Unlock();
  }

  return VTK_THREAD_RETURN_VALUE;
}

}


//----------------------------------------------------------------------------
void vesKiwiStreamingDataRepresentation::initializeWithShader(
  vesSharedPtr<vesShaderProgram> shader)
{
  this->Internal->GeometryShader = shader;
  this->Internal->PolyDataRep = vesKiwiPolyDataRepresentation::Ptr(new vesKiwiPolyDataRepresentation);
  this->Internal->PolyDataRep->initializeWithShader(this->Internal->GeometryShader);
  this->Internal->PolyDataRep->mapper()->setGeometryData(ReceiveGeometryData(this->Internal->Comm.GetPointer()));


  this->Internal->ClientThreadId = this->Internal->MultiThreader->SpawnThread(ClientLoop, this->Internal);
}

//----------------------------------------------------------------------------
bool vesKiwiStreamingDataRepresentation::handleSingleTouchDown(int displayX, int displayY)
{
  vesNotUsed(displayX);
  vesNotUsed(displayY);
  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiStreamingDataRepresentation::handleSingleTouchUp()
{
  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiStreamingDataRepresentation::handleSingleTouchTap(int displayX, int displayY)
{
  vesNotUsed(displayX);
  vesNotUsed(displayY);

  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiStreamingDataRepresentation::handleSingleTouchPanGesture(double deltaX, double deltaY)
{
  vesNotUsed(deltaX);
  vesNotUsed(deltaY);
  return false;
}

//----------------------------------------------------------------------------
void vesKiwiStreamingDataRepresentation::willRender(vesSharedPtr<vesRenderer> renderer)
{
  vesNotUsed(renderer);

  this->Internal->Lock->Lock();

  if (this->Internal->HaveNew) {
    this->Internal->PolyDataRep->mapper()->setGeometryData(this->Internal->GeometryData);
    this->Internal->HaveNew = false;
  }

  this->Internal->Lock->Unlock();
}

//----------------------------------------------------------------------------
void vesKiwiStreamingDataRepresentation::addSelfToRenderer(vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::addSelfToRenderer(renderer);
  this->Internal->PolyDataRep->addSelfToRenderer(renderer);
}

//----------------------------------------------------------------------------
void vesKiwiStreamingDataRepresentation::removeSelfFromRenderer(vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::removeSelfFromRenderer(renderer);
  if (this->Internal->PolyDataRep) {
    this->Internal->PolyDataRep->removeSelfFromRenderer(renderer);
  }
}

//----------------------------------------------------------------------------
int vesKiwiStreamingDataRepresentation::numberOfFacets()
{
  if (this->Internal->PolyDataRep) {
    return this->Internal->PolyDataRep->numberOfFacets();
  }
  return 0;
}

//----------------------------------------------------------------------------
int vesKiwiStreamingDataRepresentation::numberOfVertices()
{
  if (this->Internal->PolyDataRep) {
    return this->Internal->PolyDataRep->numberOfVertices();
  }
  return 0;
}

//----------------------------------------------------------------------------
int vesKiwiStreamingDataRepresentation::numberOfLines()
{
  if (this->Internal->PolyDataRep) {
    return this->Internal->PolyDataRep->numberOfLines();
  }
  return 0;
}
