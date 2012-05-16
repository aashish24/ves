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

#include "vesKiwiCameraSpinner.h"
#include "vesKiwiCameraInteractor.h"

//----------------------------------------------------------------------------
vesKiwiCameraSpinner::vesKiwiCameraSpinner()
{
  mIsEnabled = false;
  mLastPanDistance = 0;
  mLastPanDirection = vesVector2d(0, 0);
}

//----------------------------------------------------------------------------
vesKiwiCameraSpinner::~vesKiwiCameraSpinner()
{
}

//----------------------------------------------------------------------------
void vesKiwiCameraSpinner::setInteractor(vesKiwiCameraInteractor::Ptr interactor)
{
  mInteractor = interactor;
}

//----------------------------------------------------------------------------
vesKiwiCameraInteractor::Ptr vesKiwiCameraSpinner::interactor() const
{
  return mInteractor;
}

//----------------------------------------------------------------------------
double vesKiwiCameraSpinner::currentMagnitude() const
{
  return mLastPanDistance;
}

//----------------------------------------------------------------------------
bool vesKiwiCameraSpinner::isEnabled() const
{
  return mIsEnabled;
}

//----------------------------------------------------------------------------
void vesKiwiCameraSpinner::enable()
{
  mIsEnabled = true;
}

//----------------------------------------------------------------------------
void vesKiwiCameraSpinner::disable()
{
  mIsEnabled = false;
  mLastPanDistance = 0.0;
}

//----------------------------------------------------------------------------
void vesKiwiCameraSpinner::handlePanGesture(const vesVector2d& pan)
{
  mLastPanDistance = pan.norm();
  if (mLastPanDistance > 0) {
    mLastPanDirection = pan / mLastPanDistance;
  }
  else {
    mLastPanDirection = vesVector2d(0, 0);
  }
}

//----------------------------------------------------------------------------
void vesKiwiCameraSpinner::updateSpin()
{
  assert(mInteractor);

  if (!mIsEnabled) {
    return;
  }

  vesVector2d panDelta;

  if (mLastPanDistance > 1.0) {
    panDelta = mLastPanDirection * mLastPanDistance;
    mLastPanDistance *= 0.90;
    mInteractor->rotate(panDelta);
  }
  else {
    this->disable();
  }
}
