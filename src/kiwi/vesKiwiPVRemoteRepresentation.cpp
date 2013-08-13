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

#include "vesKiwiPVRemoteRepresentation.h"

#include "vesRenderer.h"
#include "vesCamera.h"
#include "vesMapper.h"
#include "vesGeometryData.h"
#include "vesActor.h"
#include "vesBackground.h"
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
#include <vtkCharArray.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkShortArray.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkMultiThreader.h>
#include <vtkMutexLock.h>
#include <vtkConditionVariable.h>

#include <vtksys/SystemTools.hxx>

#include <vector>
#include <cassert>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>

#include "vesPVWebClient.h"
#include "vesPVWebDataSet.h"

typedef std::map<std::string, std::vector<vesKiwiPolyDataRepresentation::Ptr> > RepMap;

//----------------------------------------------------------------------------
class vesKiwiPVRemoteRepresentation::vesInternal
{
public:

  vesInternal()
  {
    this->ShouldQuit = false;
    this->ShouldRequestScene = false;
    this->HaveNew = false;
    this->NewRemoteScene = false;
    this->HaveBackground2 = false;
    this->ClientThreadId = -1;
    this->RemoteBackground1 = vesVector3f(0,0,0);
    this->RemoteBackground2 = vesVector3f(0,0,0);
    this->NewCameraState = false;
  }

  ~vesInternal()
  {
  }

  int ClientThreadId;
  bool HaveNew;
  bool ShouldQuit;
  bool ShouldRequestScene;

  vtkNew<vtkClientSocket> Comm;
  vtkNew<vtkMultiThreader> MultiThreader;
  vtkNew<vtkMutexLock> Lock;
  vtkNew<vtkMutexLock> ExitLock;
  vtkNew<vtkConditionVariable> WaitCondition;

  RepMap Reps;
  std::vector<std::pair<std::string, vesKiwiPolyDataRepresentation::Ptr> >  RepsToAdd;
  std::set<std::string> RepsToRemove;
  std::set<std::string> CurrentReps;

  vesShaderProgram::Ptr GeometryShader;


  struct CameraStateStruct {

    vesVector3f Position;
    vesVector3f FocalPoint;
    vesVector3f ViewUp;

  };

  bool NewRemoteScene;
  bool HaveBackground2;
  vesVector3f RemoteBackground1;
  vesVector3f RemoteBackground2;
  CameraStateStruct RemoteCameraState;

  CameraStateStruct CameraState;
  bool NewCameraState;

};

//----------------------------------------------------------------------------
vesKiwiPVRemoteRepresentation::vesKiwiPVRemoteRepresentation()
{
  this->Internal = new vesInternal();
}

//----------------------------------------------------------------------------
vesKiwiPVRemoteRepresentation::~vesKiwiPVRemoteRepresentation()
{
  this->Internal->ShouldQuit = true;
  this->Internal->ExitLock->Lock();
  this->Internal->ExitLock->Unlock();
  delete this->Internal;
}

//----------------------------------------------------------------------------
bool vesKiwiPVRemoteRepresentation::connectToServer(const std::string& host, int port)
{
  return (this->Internal->Comm->ConnectToServer(host.c_str(), port) == 0);
}

namespace {

//----------------------------------------------------------------------------
bool SendCommand(vesKiwiPVRemoteRepresentation::vesInternal* selfInternal, int command)
{
  if (selfInternal->Comm->Send(&command, sizeof(command)) == 0) {
    return false;
  }
  return (selfInternal->ShouldQuit != true);
}

//----------------------------------------------------------------------------
bool ReceiveCommand(vesKiwiPVRemoteRepresentation::vesInternal* selfInternal, int& command)
{
  if (selfInternal->Comm->Receive(&command, sizeof(command)) == 0) {
    return false;
  }
  return (selfInternal->ShouldQuit != true);
}

//----------------------------------------------------------------------------
bool WaitForReadyCommand(vesKiwiPVRemoteRepresentation::vesInternal* selfInternal)
{
  int command = 0;
  if (!ReceiveCommand(selfInternal, command)) {
    return false;
  }

  const int readyCommand = 1;
  if (command != readyCommand) {
    return false;
  }

  return (selfInternal->ShouldQuit != true);
}

//----------------------------------------------------------------------------
bool WaitForNewCameraState(vesKiwiPVRemoteRepresentation::vesInternal* selfInternal)
{
  for (int i = 0; i < 100; ++i) {
    if (selfInternal->NewCameraState || selfInternal->ShouldQuit) {
      break;
    }
    usleep(1000);
  }

  return (selfInternal->ShouldQuit != true);
}

//----------------------------------------------------------------------------
bool SendCameraState(vesKiwiPVRemoteRepresentation::vesInternal* selfInternal)
{
  selfInternal->Lock->Lock();
  vesKiwiPVRemoteRepresentation::vesInternal::CameraStateStruct cameraState = selfInternal->CameraState;
  selfInternal->NewCameraState = false;
  selfInternal->Lock->Unlock();

  if (!SendCommand(selfInternal, 4)) {
    return false;
  }
  if (selfInternal->Comm->Send(&cameraState, sizeof(cameraState)) == 0) {
    return false;
  }
  return (selfInternal->ShouldQuit != true);
}

//----------------------------------------------------------------------------
bool ReceiveSceneMetaData(vesKiwiPVRemoteRepresentation::vesInternal* selfInternal, std::stringstream& resp)
{
  if (!SendCommand(selfInternal, 2)) {
    return false;
  }

  unsigned long long streamLength = 0;
  if (selfInternal->Comm->Receive(&streamLength, 8) == 0) {
    return false;
  }

  vtkCharArray* streamData = vtkCharArray::New();
  streamData->SetNumberOfTuples(streamLength);

  if (selfInternal->Comm->Receive(streamData->GetPointer(0), streamLength) == 0) {
    return false;
  }

  resp << std::string(streamData->GetPointer(0), streamLength);

  return (selfInternal->ShouldQuit != true);
}

//----------------------------------------------------------------------------
vesKiwiPolyDataRepresentation::Ptr CreateRepForPVWebData(vesSharedPtr<vesPVWebDataSet> dataset, vesShaderProgram::Ptr shader)
{
  vesKiwiPolyDataRepresentation::Ptr rep;

  vesGeometryData::Ptr geometryData = vesKiwiDataConversionTools::ConvertPVWebData(dataset);
  rep = vesKiwiPolyDataRepresentation::Ptr(new vesKiwiPolyDataRepresentation);
  rep->initializeWithShader(shader);
  rep->mapper()->setGeometryData(geometryData);

  vtkNew<vtkTransform> transform;
  const float* matrix = dataset->matrix();
  double* matrixElements = (*transform->GetMatrix())[0];
  for (int i = 0; i < 16; ++i) {
    matrixElements[i] = matrix[i];
  }
  rep->setTransformOnActor(rep->actor(), transform.GetPointer());
  if (dataset->m_transparency) {
    rep->setOpacity(0.4);
  }
  return rep;
}

//----------------------------------------------------------------------------
bool RepExists(vesKiwiPVRemoteRepresentation::vesInternal* selfInternal, const std::string& md5)
{
  bool exists = (selfInternal->CurrentReps.find(md5) != selfInternal->CurrentReps.end());
  return exists;
}

//----------------------------------------------------------------------------
void ChooseDataSetsToRemove(vesKiwiPVRemoteRepresentation::vesInternal* selfInternal, const std::set<std::string>& remoteReps)
{
  selfInternal->Lock->Lock();

  // RepToRemove = CurrentReps - remoteReps
  std::set_difference(
    selfInternal->CurrentReps.begin(), selfInternal->CurrentReps.end(),
    remoteReps.begin(), remoteReps.end(),
    std::inserter(selfInternal->RepsToRemove, selfInternal->RepsToRemove.end()));

  selfInternal->Lock->Unlock();
}

//----------------------------------------------------------------------------
bool ReceiveScene(vesKiwiPVRemoteRepresentation::vesInternal* selfInternal, const std::stringstream& resp)
{
  vesPVWebClient client;

  bool success = client.parseSceneMetaData(resp);
  if (!success) {
    return false;
  }

  selfInternal->Lock->Lock();
  selfInternal->NewRemoteScene = true;
  selfInternal->RemoteCameraState.Position = vesVector3f(client.lookAt()[7],client.lookAt()[8],client.lookAt()[9]);
  selfInternal->RemoteCameraState.FocalPoint = vesVector3f(client.lookAt()[1],client.lookAt()[2],client.lookAt()[3]);
  selfInternal->RemoteCameraState.ViewUp = vesVector3f(client.lookAt()[4],client.lookAt()[5],client.lookAt()[6]);

  const std::vector<double>& backgroundColor = client.backgroundColor();
  if (backgroundColor.size() == 3) {
    selfInternal->RemoteBackground1 = vesVector3f(backgroundColor[0], backgroundColor[1], backgroundColor[2]);
    selfInternal->RemoteBackground2 = selfInternal->RemoteBackground1;
  }
  else if (backgroundColor.size() >= 6) {
    selfInternal->RemoteBackground1 = vesVector3f(backgroundColor[0], backgroundColor[1], backgroundColor[2]);
    selfInternal->RemoteBackground2 = vesVector3f(backgroundColor[3], backgroundColor[4], backgroundColor[5]);
  }
  selfInternal->Lock->Unlock();

  const std::vector<vesPVWebDataSet::Ptr>& datasets = client.datasets();

  std::set<std::string> sceneReps;
  std::set<std::string> remoteReps;
  for (size_t i = 0; i < datasets.size(); ++i) {
    remoteReps.insert(datasets[i]->m_md5);
  }

  ChooseDataSetsToRemove(selfInternal, remoteReps);


  if (!SendCommand(selfInternal, 3)) {
    return false;
  }

  unsigned long long streamLength;

  for (size_t i = 0; i < datasets.size(); ++i) {


    vesPVWebDataSet::Ptr dataset = datasets[i];

    bool skip = false;

    if (RepExists(selfInternal, dataset->m_md5)) {
      sceneReps.insert(dataset->m_md5);
      skip = true;
    }
    else if (dataset->m_layer != 0) {
      skip = true;
    }

    if (selfInternal->Comm->Send(&skip, 1) == 0) {
      return false;
    }

    if (selfInternal->ShouldQuit) {
      return false;
    }

    if (skip) {
      continue;
    }

    if (selfInternal->Comm->Receive(&streamLength, 8) == 0) {
      return false;
    }

    if (selfInternal->ShouldQuit) {
      return false;
    }

    dataset->m_buffer = new char[streamLength];
    if (selfInternal->Comm->Receive(dataset->m_buffer, streamLength) == 0) {
      return false;
    }

    if (selfInternal->ShouldQuit) {
      return false;
    }

    dataset->initFromBuffer();

    if (dataset->m_numberOfVerts == 0) {
      continue;
    }


    vesKiwiPolyDataRepresentation::Ptr rep = CreateRepForPVWebData(dataset, selfInternal->GeometryShader);

    sceneReps.insert(dataset->m_md5);
    selfInternal->Lock->Lock();
    selfInternal->RepsToAdd.push_back(std::make_pair(dataset->m_md5, rep));
    selfInternal->RepsToRemove.erase(dataset->m_md5);
    selfInternal->Lock->Unlock();
  }

  selfInternal->CurrentReps = sceneReps;

  return (selfInternal->ShouldQuit != true);
}

//----------------------------------------------------------------------------
bool RequestScene(vesKiwiPVRemoteRepresentation::vesInternal* selfInternal)
{
  std::stringstream resp;
  if (!ReceiveSceneMetaData(selfInternal, resp)) {
    return false;
  }

  if (!WaitForReadyCommand(selfInternal)) {
    return false;
  }

  if (!ReceiveScene(selfInternal, resp)) {
    return false;
  }

  selfInternal->Lock->Lock();
  selfInternal->ShouldRequestScene = false;
  selfInternal->Lock->Unlock();

  return (selfInternal->ShouldQuit != true);
}

//----------------------------------------------------------------------------
void ClientLoop(vesKiwiPVRemoteRepresentation::vesInternal* selfInternal)
{

  while (!selfInternal->ShouldQuit) {

    if (!WaitForReadyCommand(selfInternal)) {
      break;
    }

    if (selfInternal->ShouldRequestScene) {
      if (!RequestScene(selfInternal)) {
        break;
      }
      continue;
    }

    if (!WaitForNewCameraState(selfInternal)) {
      break;
    }

    if (selfInternal->NewCameraState) {
      if (!SendCameraState(selfInternal)) {
        break;
      }
    }
    else {
      if (!SendCommand(selfInternal, 5)) {
        break;
      }
    }
  }

}


//----------------------------------------------------------------------------
VTK_THREAD_RETURN_TYPE ThreadStart(void* arg)
{
  vtkMultiThreader::ThreadInfo* threadInfo = static_cast<vtkMultiThreader::ThreadInfo*>(arg);

  vesKiwiPVRemoteRepresentation::vesInternal* selfInternal =
    static_cast<vesKiwiPVRemoteRepresentation::vesInternal*>(threadInfo->UserData);

  selfInternal->ExitLock->Lock();

  ClientLoop(selfInternal);

  if (selfInternal->Comm->GetConnected()) {
    selfInternal->Comm->CloseSocket();
  }

  selfInternal->ExitLock->Unlock();
  return VTK_THREAD_RETURN_VALUE;
}

}


//----------------------------------------------------------------------------
void vesKiwiPVRemoteRepresentation::initializeWithShader(
  vesSharedPtr<vesShaderProgram> shader)
{
  this->Internal->GeometryShader = shader;
  this->requestScene();
  this->Internal->ClientThreadId = this->Internal->MultiThreader->SpawnThread(ThreadStart, this->Internal);
}

//----------------------------------------------------------------------------
void vesKiwiPVRemoteRepresentation::requestScene()
{
  this->Internal->Lock->Lock();
  this->Internal->ShouldRequestScene = true;
  this->Internal->Lock->Unlock();
}

//----------------------------------------------------------------------------
bool vesKiwiPVRemoteRepresentation::handleSingleTouchDown(int displayX, int displayY)
{
  vesNotUsed(displayX);
  vesNotUsed(displayY);
  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiPVRemoteRepresentation::handleSingleTouchUp()
{
  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiPVRemoteRepresentation::handleSingleTouchTap(int displayX, int displayY)
{
  vesNotUsed(displayX);
  vesNotUsed(displayY);

  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiPVRemoteRepresentation::handleSingleTouchPanGesture(double deltaX, double deltaY)
{
  vesNotUsed(deltaX);
  vesNotUsed(deltaY);
  return false;
}

//----------------------------------------------------------------------------
void vesKiwiPVRemoteRepresentation::willRender(vesSharedPtr<vesRenderer> renderer)
{
  vesNotUsed(renderer);

  vesCamera::Ptr camera = renderer->camera();

  this->Internal->Lock->Lock();

  // remove reps if needed
  if (this->Internal->RepsToRemove.size()) {
    std::set<std::string>::const_iterator itr = this->Internal->RepsToRemove.begin();
    for (; itr != this->Internal->RepsToRemove.end(); ++itr) {
      RepMap::iterator mapItr = this->Internal->Reps.find(*itr);
      if (mapItr != this->Internal->Reps.end()) {
        std::vector<vesKiwiPolyDataRepresentation::Ptr>& repVec = mapItr->second;
        for (size_t i = 0; i < repVec.size(); ++i) {
          repVec[i]->removeSelfFromRenderer(renderer);
        }

        this->Internal->Reps.erase(mapItr);
      }
    }
    this->Internal->RepsToRemove.clear();
  }

  // add new reps
  if (this->Internal->RepsToAdd.size()) {
    for (size_t i = 0; i < this->Internal->RepsToAdd.size(); ++i) {
      vesKiwiPolyDataRepresentation::Ptr& repToAdd = this->Internal->RepsToAdd[i].second;
      this->Internal->Reps[this->Internal->RepsToAdd[i].first].push_back(repToAdd);
      repToAdd->addSelfToRenderer(renderer);
    }
    this->Internal->RepsToAdd.clear();
  }

  if (this->Internal->NewRemoteScene) {
    this->Internal->NewRemoteScene = false;
    renderer->background()->setGradientColor(this->Internal->RemoteBackground2, this->Internal->RemoteBackground1);
    camera->setPosition(this->Internal->RemoteCameraState.Position);
    camera->setFocalPoint(this->Internal->RemoteCameraState.FocalPoint);
    camera->setViewUp(this->Internal->RemoteCameraState.ViewUp);
  }

  if (camera->position() != this->Internal->CameraState.Position
     || camera->focalPoint() != this->Internal->CameraState.FocalPoint
     || camera->viewUp() != this->Internal->CameraState.ViewUp) {
    this->Internal->CameraState.Position = camera->position();
    this->Internal->CameraState.FocalPoint = camera->focalPoint();
    this->Internal->CameraState.ViewUp = camera->viewUp();
    this->Internal->NewCameraState = true;
  }

  this->Internal->Lock->Unlock();
}

//----------------------------------------------------------------------------
void vesKiwiPVRemoteRepresentation::addSelfToRenderer(vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::addSelfToRenderer(renderer);

  RepMap::const_iterator mapItr = this->Internal->Reps.begin();
  for (; mapItr != this->Internal->Reps.end(); ++mapItr) {
    const std::vector<vesKiwiPolyDataRepresentation::Ptr>& repVec = mapItr->second;
    for (size_t i = 0; i < repVec.size(); ++i) {
      repVec[i]->addSelfToRenderer(renderer);
    }
  }

}

//----------------------------------------------------------------------------
void vesKiwiPVRemoteRepresentation::removeSelfFromRenderer(vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::removeSelfFromRenderer(renderer);

  RepMap::const_iterator mapItr = this->Internal->Reps.begin();
  for (; mapItr != this->Internal->Reps.end(); ++mapItr) {
    const std::vector<vesKiwiPolyDataRepresentation::Ptr>& repVec = mapItr->second;
    for (size_t i = 0; i < repVec.size(); ++i) {
      repVec[i]->removeSelfFromRenderer(renderer);
    }
  }

}
