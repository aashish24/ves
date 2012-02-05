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

#include "vesKiwiText2DRepresentation.h"
#include "vesActor.h"
#include "vesRenderer.h"
#include "vesCamera.h"
#include "vesTexture.h"
#include "vesImage.h"

#include <vtkFreeTypeStringToImage.h>
#include <vtkTextProperty.h>
#include <vtkSmartPointer.h>
#include <vtkStdString.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkNew.h>

#include <cassert>

//----------------------------------------------------------------------------
class vesKiwiText2DRepresentation::vesInternal
{
public:

  vesInternal()
  {
    this->TextToImage = vtkSmartPointer<vtkFreeTypeStringToImage>::New();
    this->WorldAnchorPointEnabled = false;
    this->AnchorOffset = 0.0;
  }

  ~vesInternal()
  {
  }

  vtkSmartPointer<vtkFreeTypeStringToImage> TextToImage;

  bool WorldAnchorPointEnabled;
  double AnchorOffset;
  vesVector2f DisplayPosition;
  vesVector3f WorldAnchorPoint;
};

//----------------------------------------------------------------------------
vesKiwiText2DRepresentation::vesKiwiText2DRepresentation()
{
  this->Internal = new vesInternal();
}

//----------------------------------------------------------------------------
vesKiwiText2DRepresentation::~vesKiwiText2DRepresentation()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vesKiwiText2DRepresentation::setText(const std::string& text)
{
  assert(this->actor());

  vtkNew<vtkTextProperty> textProperty;
  textProperty->SetFontFamilyToArial();
  textProperty->SetFontSize(32);
  textProperty->SetColor(1.0, 0.8, 0.0);

  vtkNew<vtkImageData> imageTexture;
  this->Internal->TextToImage->RenderString(textProperty.GetPointer(), text, imageTexture.GetPointer());

  // The above call to RenderString() sets the imageTexture origin so some nonzero value, not sure why
  imageTexture->SetOrigin(0, 0, 0);

  this->setImageData(imageTexture.GetPointer());

  this->actor()->setIsOverlayNode(true);
  this->setBinNumber(20);
}

//----------------------------------------------------------------------------
void vesKiwiText2DRepresentation::setWorldAnchorPointEnabled(bool enabled)
{
  this->Internal->WorldAnchorPointEnabled = enabled;
}

//----------------------------------------------------------------------------
bool vesKiwiText2DRepresentation::worldAnchorPointEnabled() const
{
  return this->Internal->WorldAnchorPointEnabled;
}

//----------------------------------------------------------------------------
void vesKiwiText2DRepresentation::setWorldAnchorPoint(vesVector3f worldPoint)
{
  this->Internal->WorldAnchorPoint = worldPoint;
}

//----------------------------------------------------------------------------
vesVector3f vesKiwiText2DRepresentation::worldAnchorPoint() const
{
  return this->Internal->WorldAnchorPoint;
}

//----------------------------------------------------------------------------
void vesKiwiText2DRepresentation::setAnchorOffset(double offset)
{
  this->Internal->AnchorOffset = offset;
}

//----------------------------------------------------------------------------
double vesKiwiText2DRepresentation::anchorOffset() const
{
  return this->Internal->AnchorOffset;
}

//----------------------------------------------------------------------------
void vesKiwiText2DRepresentation::setDisplayPosition(vesVector2f displayPosition)
{
  this->Internal->DisplayPosition = displayPosition;
  this->setTranslation(vesVector3f(displayPosition[0], displayPosition[1], 0.0));
}

//----------------------------------------------------------------------------
vesVector2f vesKiwiText2DRepresentation::displayPosition() const
{
  return this->Internal->DisplayPosition;
}

//----------------------------------------------------------------------------
int vesKiwiText2DRepresentation::textWidth()
{
  return this->texture()->image()->width();
}

//----------------------------------------------------------------------------
void vesKiwiText2DRepresentation::willRender(vesSharedPtr<vesRenderer> renderer)
{
  if (!this->Internal->WorldAnchorPointEnabled || !this->texture()) {
    return;
  }

  assert(renderer);
  vesVector3f anchorOffset = renderer->camera()->viewUp();
  anchorOffset.normalize();
  anchorOffset *= this->Internal->AnchorOffset;
  vesVector3f worldPoint = this->Internal->WorldAnchorPoint - anchorOffset;
  vesVector3f displayPoint = renderer->computeWorldToDisplay(worldPoint);
  displayPoint[0] -= this->texture()->image()->width() / 2.0;

  this->setDisplayPosition(vesVector2f(displayPoint[0], displayPoint[1]));
}
