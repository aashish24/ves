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

#include "vesKiwiWidgetRepresentation.h"


//----------------------------------------------------------------------------
vesKiwiWidgetRepresentation::vesKiwiWidgetRepresentation()
  : m_interactionIsActive(false)
{
}

//----------------------------------------------------------------------------
vesKiwiWidgetRepresentation::~vesKiwiWidgetRepresentation()
{
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetRepresentation::interactionIsActive() const
{
  return this->m_interactionIsActive;
}

//----------------------------------------------------------------------------
void vesKiwiWidgetRepresentation::interactionOn()
{
  this->m_interactionIsActive = true;
}

//----------------------------------------------------------------------------
void vesKiwiWidgetRepresentation::interactionOff()
{
  this->m_interactionIsActive = false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetRepresentation::handleSingleTouchPanGesture(double deltaX, double deltaY)
{
  vesNotUsed(deltaX);
  vesNotUsed(deltaY);
  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetRepresentation::handleSingleTouchDown(int displayX, int displayY)
{
  vesNotUsed(displayX);
  vesNotUsed(displayY);
  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetRepresentation::handleSingleTouchTap(int displayX, int displayY)
{
  vesNotUsed(displayX);
  vesNotUsed(displayY);
  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetRepresentation::handleDoubleTap(int displayX, int displayY)
{
  vesNotUsed(displayX);
  vesNotUsed(displayY);
  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetRepresentation::handleLongPress(int displayX, int displayY)
{
  vesNotUsed(displayX);
  vesNotUsed(displayY);
  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiWidgetRepresentation::handleSingleTouchUp()
{
  return false;
}

//----------------------------------------------------------------------------
void vesKiwiWidgetRepresentation::addSelfToRenderer(vesSharedPtr<vesRenderer> renderer)
{
  this->m_renderer = renderer;
}

//----------------------------------------------------------------------------
void vesKiwiWidgetRepresentation::removeSelfFromRenderer(vesSharedPtr<vesRenderer> renderer)
{
  vesNotUsed(renderer);
  this->m_renderer.reset();
}
