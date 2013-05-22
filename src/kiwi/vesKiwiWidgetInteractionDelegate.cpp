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

#include "vesKiwiWidgetInteractionDelegate.h"

#include "vesKiwiWidgetRepresentation.h"

using vesDynamicPtrCast;


//----------------------------------------------------------------------------
bool vesKiwiWidgetInteractionDelegate::handleSingleTouchPanGesture(double deltaX, double deltaY, std::vector<vesKiwiDataRepresentation::Ptr> reps)
{
  for (size_t i = 0; i < reps.size(); ++i) {
    vesKiwiWidgetRepresentation::Ptr rep = dynamic_pointer_cast<vesKiwiWidgetRepresentation>(reps[i]);
    if (rep) {
      if (rep->handleSingleTouchPanGesture(deltaX, deltaY)) {
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetInteractionDelegate::handleSingleTouchUp(std::vector<vesKiwiDataRepresentation::Ptr> reps)
{
  for (size_t i = 0; i < reps.size(); ++i) {
    vesKiwiWidgetRepresentation::Ptr rep = dynamic_pointer_cast<vesKiwiWidgetRepresentation>(reps[i]);
    if (rep) {
      if (rep->handleSingleTouchUp()) {
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetInteractionDelegate::handleSingleTouchTap(int displayX, int displayY, std::vector<vesKiwiDataRepresentation::Ptr> reps)
{
  for (size_t i = 0; i < reps.size(); ++i) {
    vesKiwiWidgetRepresentation::Ptr rep = dynamic_pointer_cast<vesKiwiWidgetRepresentation>(reps[i]);
    if (rep) {
      if (rep->handleSingleTouchTap(displayX, displayY)) {
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetInteractionDelegate::handleSingleTouchDown(int displayX, int displayY, std::vector<vesKiwiDataRepresentation::Ptr> reps)
{
  for (size_t i = 0; i < reps.size(); ++i) {
    vesKiwiWidgetRepresentation::Ptr rep = dynamic_pointer_cast<vesKiwiWidgetRepresentation>(reps[i]);
    if (rep) {
      if (rep->handleSingleTouchDown(displayX, displayY)) {
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetInteractionDelegate::handleTwoTouchPanGesture(double x0, double y0, double x1, double y1, std::vector<vesSharedPtr<vesKiwiDataRepresentation> > reps)
{
  for (size_t i = 0; i < reps.size(); ++i) {
    vesKiwiWidgetRepresentation::Ptr rep = dynamic_pointer_cast<vesKiwiWidgetRepresentation>(reps[i]);
    if (rep) {
      if (rep->handleTwoTouchPanGesture(x0, y0, x1, y1)) {
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetInteractionDelegate::handleTwoTouchPinchGesture(double scale, std::vector<vesSharedPtr<vesKiwiDataRepresentation> > reps)
{
  for (size_t i = 0; i < reps.size(); ++i) {
    vesKiwiWidgetRepresentation::Ptr rep = dynamic_pointer_cast<vesKiwiWidgetRepresentation>(reps[i]);
    if (rep) {
      if (rep->handleTwoTouchPinchGesture(scale)) {
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetInteractionDelegate::handleTwoTouchRotationGesture(double rotation, std::vector<vesSharedPtr<vesKiwiDataRepresentation> > reps)
{
  for (size_t i = 0; i < reps.size(); ++i) {
    vesKiwiWidgetRepresentation::Ptr rep = dynamic_pointer_cast<vesKiwiWidgetRepresentation>(reps[i]);
    if (rep) {
      if (rep->handleTwoTouchRotationGesture(rotation)) {
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetInteractionDelegate::handleDoubleTap(int displayX, int displayY, std::vector<vesKiwiDataRepresentation::Ptr> reps)
{
  for (size_t i = 0; i < reps.size(); ++i) {
    vesKiwiWidgetRepresentation::Ptr rep = dynamic_pointer_cast<vesKiwiWidgetRepresentation>(reps[i]);
    if (rep) {
      if (rep->handleDoubleTap(displayX, displayY)) {
        return true;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetInteractionDelegate::handleLongPress(int displayX, int displayY, std::vector<vesKiwiDataRepresentation::Ptr> reps)
{
  for (size_t i = 0; i < reps.size(); ++i) {
    vesKiwiWidgetRepresentation::Ptr rep = dynamic_pointer_cast<vesKiwiWidgetRepresentation>(reps[i]);
    if (rep) {
      if (rep->handleLongPress(displayX, displayY)) {
        return true;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetInteractionDelegate::interactionIsActive(std::vector<vesKiwiDataRepresentation::Ptr> reps)
{
  for (size_t i = 0; i < reps.size(); ++i) {
    vesKiwiWidgetRepresentation::Ptr rep = dynamic_pointer_cast<vesKiwiWidgetRepresentation>(reps[i]);
    if (rep) {
      if (rep->interactionIsActive()) {
        return true;
      }
    }
  }
  return false;
}
