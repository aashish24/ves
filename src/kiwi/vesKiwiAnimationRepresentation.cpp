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

#include "vesKiwiAnimationRepresentation.h"

#include "vesRenderer.h"
#include "vesCamera.h"
#include "vesMapper.h"
#include "vesActor.h"
#include "vesShaderProgram.h"
#include "vesTexture.h"
#include "vesKiwiColorMapCollection.h"
#include "vesKiwiDataLoader.h"
#include "vesKiwiDataConversionTools.h"
#include "vesKiwiText2DRepresentation.h"
#include "vesKiwiPolyDataRepresentation.h"

#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkPolyData.h>
#include <vtkTimerLog.h>
#include <vtkNew.h>
#include <vtkDoubleArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkLookupTable.h>

#include <vtksys/SystemTools.hxx>

#include <vector>
#include <cassert>
#include <sstream>

//----------------------------------------------------------------------------
class vesKiwiAnimationRepresentation::vesInternal
{
public:

  vesInternal()
  {
    this->TextRep = 0;
    this->PlayRep = 0;
    this->CurrentFrame = 0;
    this->LastFrame = 0;
    this->NumberOfFrames = 0;
    this->InteractionDelta = 0;

    this->PlayMode = false;
    this->AnimationT0 = 0.0;
    this->AnimationFrameStart = 0;
    this->AnimationFramesPerSecond = 24;
    this->EnableTapToPlay = false;
  }

  ~vesInternal()
  {

  }

  int CurrentFrame;
  int LastFrame;
  int AnimationFrameStart;
  int NumberOfFrames;
  bool PlayMode;
  bool EnableTapToPlay;

  double AnimationT0;
  double AnimationFramesPerSecond;
  double InteractionDelta;
  vesKiwiText2DRepresentation* TextRep;

  vesKiwiText2DRepresentation* PlayRep;

  std::vector<vesKiwiDataRepresentation::Ptr> AllReps;
  std::vector<vesKiwiPolyDataRepresentation::Ptr> FrameReps;

  //vtkSmartPointer<vtkUnsignedCharArray> ColorTable;

  vesSharedPtr<vesShaderProgram> GeometryShader;
  vesSharedPtr<vesShaderProgram> TextureShader;
};

//----------------------------------------------------------------------------
vesKiwiAnimationRepresentation::vesKiwiAnimationRepresentation()
{
  this->Internal = new vesInternal();
}

//----------------------------------------------------------------------------
vesKiwiAnimationRepresentation::~vesKiwiAnimationRepresentation()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vesKiwiAnimationRepresentation::initializeWithShader(
  vesSharedPtr<vesShaderProgram> geometryShader, 
  vesSharedPtr<vesShaderProgram> textureShader,
  vesSharedPtr<vesShaderProgram> gouraudTextureShader)
{
  this->Internal->GeometryShader = geometryShader;
  this->Internal->TextureShader = gouraudTextureShader;

  /*
  this->Internal->TextRep = new vesKiwiText2DRepresentation();
  this->Internal->TextRep->initializeWithShader(textureShader);
  this->Internal->TextRep->setDisplayPosition(vesVector2f(10, 10));
  this->Internal->TextRep->setText("Time: 0.000 s");
  this->Internal->AllReps.push_back(this->Internal->TextRep);


  this->Internal->PlayRep = new vesKiwiText2DRepresentation();
  this->Internal->PlayRep->initializeWithShader(textureShader);
  this->Internal->PlayRep->setText("[Play]");
  if (this->Internal->EnableTapToPlay) {
    this->Internal->AllReps.push_back(this->Internal->PlayRep);
  }
  */
}

//----------------------------------------------------------------------------
void vesKiwiAnimationRepresentation::setRepresentations(const std::vector<vesKiwiPolyDataRepresentation::Ptr> reps)
{
  this->Internal->FrameReps = reps;
  this->Internal->NumberOfFrames = static_cast<int>(this->Internal->FrameReps.size());
}

//----------------------------------------------------------------------------
bool vesKiwiAnimationRepresentation::handleDoubleTap(int displayX, int displayY)
{
  if (this->Internal->PlayMode) {
    this->onPause();
  }
  else {
    this->onPlay();
  }
  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiAnimationRepresentation::playMode() const
{
  return this->Internal->PlayMode;
}

//----------------------------------------------------------------------------
bool vesKiwiAnimationRepresentation::handleSingleTouchDown(int displayX, int displayY)
{
  vesNotUsed(displayY);

  int cornerSize = 50;
  if (displayX < cornerSize) {

    this->Internal->InteractionDelta = 0;
    this->interactionOn();
    return true;
  }

  return false;
}


//----------------------------------------------------------------------------
bool vesKiwiAnimationRepresentation::handleSingleTouchUp()
{
  if (!this->interactionIsActive()) {
    return false;
  }

  this->interactionOff();
  return true;
}

//----------------------------------------------------------------------------
void vesKiwiAnimationRepresentation::onPause()
{
  this->Internal->PlayMode = false;

//  std::string playText = this->Internal->PlayMode ? "[Pause]" : "[Play]";
//  this->Internal->PlayRep->setText(playText);
}

//----------------------------------------------------------------------------
void vesKiwiAnimationRepresentation::onPlay()
{
  this->Internal->PlayMode = true;

  this->Internal->AnimationT0 = vtkTimerLog::GetUniversalTime();
  this->Internal->AnimationFrameStart = this->Internal->CurrentFrame;

//  std::string playText = this->Internal->PlayMode ? "[Pause]" : "[Play]";
//  this->Internal->PlayRep->setText(playText);
}

//----------------------------------------------------------------------------
bool vesKiwiAnimationRepresentation::handleSingleTouchTap(int displayX, int displayY)
{
/*
  vesVector2f textSize = this->Internal->PlayRep->textureSize();
  double margin = 30;
  textSize += vesVector2f(margin, margin);

  if (this->Internal->EnableTapToPlay && displayX <= textSize[0] && displayY <= textSize[1]) {

    if (this->Internal->PlayMode) {
      this->onPause();
    }
    else {
      this->onPlay();
    }

    return true;
  }
  else if (displayX > this->renderer()->width() - 50 && displayY < 50) {

    vesShaderProgram::Ptr newShader = this->Internal->GeometryShader;
    if (this->Internal->FrameReps[0]->shaderProgram() == newShader) {
      newShader = this->Internal->TextureShader;
    }

    for (size_t i = 0; i < this->Internal->FrameReps.size(); ++i) {
      this->Internal->FrameReps[i]->setShaderProgram(newShader);
    }

  }
*/
  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiAnimationRepresentation::handleSingleTouchPanGesture(double deltaX, double deltaY)
{
  vesNotUsed(deltaX);

  if (!this->interactionIsActive()) {
    return false;
  }

  if (this->Internal->PlayMode) {

    double framesDelta = (-deltaY/this->renderer()->height()) * 40;
    this->setFramesPerSecond(this->Internal->AnimationFramesPerSecond + framesDelta);

  }
  else {

    double framesDelta = (deltaY/this->renderer()->height()) * 80;

    int currentFrame = this->Internal->CurrentFrame;
    if (currentFrame >= this->Internal->NumberOfFrames-1 && framesDelta > 0) {
      return true;
    }
    else if (currentFrame <= 0 && framesDelta < 0) {
      return true;
    }

    this->Internal->InteractionDelta += framesDelta;

    int deltaFrames = static_cast<int>(this->Internal->InteractionDelta);
    if (deltaFrames != 0) {
      this->Internal->InteractionDelta = 0;
      this->setCurrentFrame(currentFrame + deltaFrames);
    }
  }


  return true;
}

//----------------------------------------------------------------------------
std::vector<std::string> vesKiwiAnimationRepresentation::actions() const
{
  std::string action = this->Internal->PlayMode ? "Pause" : "Play";
  std::vector<std::string> actions;
  actions.push_back(action);
  return actions;
}

//----------------------------------------------------------------------------
bool vesKiwiAnimationRepresentation::handleAction(const std::string& action)
{
  if (action == "Play" && !this->Internal->PlayMode) {
    this->onPlay();
    return true;
  }
  else if (action == "Pause" && this->Internal->PlayMode) {
    this->onPause();
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
void vesKiwiAnimationRepresentation::setFramesPerSecond(float fps)
{
  this->Internal->AnimationFramesPerSecond = fps;

  if (this->Internal->AnimationFramesPerSecond > 120) {
    this->Internal->AnimationFramesPerSecond = 120;
  }
  else if (this->Internal->AnimationFramesPerSecond <= 1) {
    this->Internal->AnimationFramesPerSecond = 1;
  }
}

//----------------------------------------------------------------------------
void vesKiwiAnimationRepresentation::setCurrentFrame(int frameIndex)
{
  int maxFrameIndex = this->Internal->NumberOfFrames - 1;
  frameIndex = frameIndex > maxFrameIndex ? maxFrameIndex : (frameIndex < 0 ? 0 : frameIndex);
  this->Internal->CurrentFrame = frameIndex;
}

//----------------------------------------------------------------------------
void vesKiwiAnimationRepresentation::willRender(vesSharedPtr<vesRenderer> renderer)
{
  vesNotUsed(renderer);

  if (this->Internal->PlayMode) {

    double currentTime = vtkTimerLog::GetUniversalTime();
    double elapsedTime = currentTime - this->Internal->AnimationT0;

    double animationFramesPerSecond = this->Internal->AnimationFramesPerSecond;

    int elapsedFrames = static_cast<int>(elapsedTime * animationFramesPerSecond);

    if (elapsedFrames != 0) {
      this->Internal->CurrentFrame += elapsedFrames;
      this->Internal->CurrentFrame = this->Internal->CurrentFrame % this->Internal->NumberOfFrames;
      this->Internal->AnimationT0 = currentTime;
    }
  }

/*
  int screenHeight = this->renderer()->height();
  double margin = 10;
  vesVector2f textSize = this->Internal->PlayRep->textureSize();
  this->Internal->PlayRep->setDisplayPosition(vesVector2f(margin, screenHeight - (margin + textSize[1])));
*/

  if (this->Internal->FrameReps.size() && this->Internal->LastFrame != this->Internal->CurrentFrame) {

    /*
    std::stringstream str;
    str.precision(4);
    str << "Time: " << std::fixed << 0.0001*this->Internal->CurrentFrame << " s";
    this->Internal->TextRep->setText(str.str());
    */

    this->Internal->FrameReps[this->Internal->LastFrame]->removeSelfFromRenderer(this->renderer());
    this->Internal->FrameReps[this->Internal->CurrentFrame]->addSelfToRenderer(this->renderer());
    this->Internal->LastFrame = this->Internal->CurrentFrame;
  }
}

//----------------------------------------------------------------------------
vesKiwiPolyDataRepresentation::Ptr vesKiwiAnimationRepresentation::currentFrameRepresentation()
{
  if (this->Internal->FrameReps.size()) {
    return this->Internal->FrameReps[this->Internal->CurrentFrame];
  }
  return vesKiwiPolyDataRepresentation::Ptr();
}

//----------------------------------------------------------------------------
void vesKiwiAnimationRepresentation::addSelfToRenderer(vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::addSelfToRenderer(renderer);
  if (this->currentFrameRepresentation()) {
    this->currentFrameRepresentation()->addSelfToRenderer(renderer);
  }

/*
  this->Internal->TextRep->addSelfToRenderer(renderer);

  if (this->Internal->EnableTapToPlay) {
    this->Internal->PlayRep->addSelfToRenderer(renderer);
  }
*/
}

//----------------------------------------------------------------------------
void vesKiwiAnimationRepresentation::removeSelfFromRenderer(vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::removeSelfFromRenderer(renderer);
  if (this->currentFrameRepresentation()) {
    this->currentFrameRepresentation()->removeSelfFromRenderer(renderer);
  }

/*
  this->Internal->TextRep->removeSelfFromRenderer(renderer);

  if (this->Internal->EnableTapToPlay) {
    this->Internal->PlayRep->removeSelfFromRenderer(renderer);
  }
*/
}
