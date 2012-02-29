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
  }

  ~vesInternal()
  {
    for (size_t i = 0; i < this->AllReps.size(); ++i) {
      delete this->AllReps[i];
    }
  }

  int CurrentFrame;
  int LastFrame;
  int AnimationFrameStart;
  int NumberOfFrames;
  bool PlayMode;

  double AnimationT0;
  double AnimationFramesPerSecond;
  double InteractionDelta;
  vesKiwiText2DRepresentation* TextRep;

  vesKiwiText2DRepresentation* PlayRep;

  std::vector<vesKiwiDataRepresentation*> AllReps;
  std::vector<vesKiwiPolyDataRepresentation*> FrameReps;

  vtkSmartPointer<vtkUnsignedCharArray> ColorTable;

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

  this->Internal->TextRep = new vesKiwiText2DRepresentation();
  this->Internal->TextRep->initializeWithShader(textureShader);
  this->Internal->TextRep->setDisplayPosition(vesVector2f(10, 10));
  this->Internal->TextRep->setText("Time: 0.000 s");
  this->Internal->AllReps.push_back(this->Internal->TextRep);

  this->Internal->PlayRep = new vesKiwiText2DRepresentation();
  this->Internal->PlayRep->initializeWithShader(textureShader);
  this->Internal->PlayRep->setText("[Play]");
  this->Internal->AllReps.push_back(this->Internal->PlayRep);
}

//----------------------------------------------------------------------------
void vesKiwiAnimationRepresentation::loadData(const std::string& filename)
{

  std::string dataDir = vtksys::SystemTools::GetFilenamePath(filename);

  std::vector<std::string> filenames;
  for (int i = 0; i < 44; ++i) {
    std::stringstream str;
    str << "can" << setfill ('0') << setw(4) << i << ".vtp";
    filenames.push_back(str.str());
  }

  vesKiwiDataLoader dataLoader;

  double scalarRange[2] = {0.0, 6000.0};
  vtkSmartPointer<vtkScalarsToColors> scalarsToColors = vesKiwiDataConversionTools::GetBlackBodyRadiationColorMap(scalarRange);

  int colorTableResolution = 256;
  vtkNew<vtkDoubleArray> scalarRangeValues;
  scalarRangeValues->SetNumberOfComponents(1);
  scalarRangeValues->SetNumberOfTuples(colorTableResolution);
  double scalarRangeDist = scalarRange[1] - scalarRange[0];
  double colorTableScalarRangeStep = scalarRangeDist/(colorTableResolution-1);
  for (int i = 0; i < colorTableResolution; ++i) {
    double scalarRangeValue = scalarRange[0] + (i*colorTableScalarRangeStep);
    scalarRangeValues->SetValue(i, scalarRangeValue);
  }

  this->Internal->ColorTable = vesKiwiDataConversionTools::MapScalars(scalarRangeValues.GetPointer(), scalarsToColors);

  for (size_t i = 0; i < filenames.size(); ++i) {

    std::string modelFile = dataDir + "/" + filenames[i];

    vtkSmartPointer<vtkPolyData> polyData = vtkPolyData::SafeDownCast(dataLoader.loadDataset(modelFile));
    if (!polyData) {
      printf("Failed to read: %s\n", modelFile.c_str());
      continue;
    }

    vesKiwiPolyDataRepresentation* rep = new vesKiwiPolyDataRepresentation();
    rep->initializeWithShader(this->Internal->GeometryShader);
    rep->setPolyData(polyData, scalarsToColors);

    vtkDataArray* scalars = vesKiwiDataConversionTools::FindScalarsArray(polyData);
    assert(scalars);
    const vtkIdType nTuples = scalars->GetNumberOfTuples();
    vtkNew<vtkDoubleArray> tcoords;
    tcoords->SetNumberOfComponents(2);
    tcoords->SetNumberOfTuples(nTuples);
    for (vtkIdType k = 0; k < nTuples; ++k) {
      double scalarValue = scalars->GetComponent(k, 0);
      double lookupTableValue = (scalarValue - scalarRange[0]) / (scalarRange[1] - scalarRange[0]);
      lookupTableValue = lookupTableValue > 1.0 ? 1.0 : (lookupTableValue < 0.0 ? 0.0 : lookupTableValue);
      tcoords->SetTuple2(k, lookupTableValue, 0);
    }

    rep->addTextureCoordinates(tcoords.GetPointer());

    vesTexture::Ptr texture = vesTexture::Ptr(new vesTexture());
    vesKiwiDataConversionTools::SetTextureData(this->Internal->ColorTable, texture, this->Internal->ColorTable->GetNumberOfTuples(), 1);
    rep->setTexture(texture);

    this->Internal->AllReps.push_back(rep);
    this->Internal->FrameReps.push_back(rep);
  }

  this->Internal->NumberOfFrames = static_cast<int>(this->Internal->FrameReps.size());
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
bool vesKiwiAnimationRepresentation::handleSingleTouchTap(int displayX, int displayY)
{
  vesVector2f textSize = this->Internal->PlayRep->textureSize();
  double margin = 30;
  textSize += vesVector2f(margin, margin);

  if (displayX <= textSize[0] && displayY <= textSize[1]) {

    this->Internal->PlayMode = !this->Internal->PlayMode;
    if (this->Internal->PlayMode) {
      this->Internal->AnimationT0 = vtkTimerLog::GetUniversalTime();
      this->Internal->AnimationFrameStart = this->Internal->CurrentFrame;
    }

    std::string playText = this->Internal->PlayMode ? "[Pause]" : "[Play]";
    this->Internal->PlayRep->setText(playText);

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

    double delta = deltaY *= -1;
    this->Internal->AnimationFramesPerSecond += delta * 0.1;

    if (this->Internal->AnimationFramesPerSecond > 120) {
      this->Internal->AnimationFramesPerSecond = 120;
    }
    else if (this->Internal->AnimationFramesPerSecond <= 1) {
      this->Internal->AnimationFramesPerSecond = 1;
    }

  }
  else {

    double delta = deltaY;
    int currentFrame = this->Internal->CurrentFrame;
    if (currentFrame >= this->Internal->NumberOfFrames-1 && delta > 0) {
      return true;
    }
    else if (currentFrame <= 0 && delta < 0) {
      return true;
    }

    this->Internal->InteractionDelta += delta * 0.1;
    int deltaFrames = static_cast<int>(this->Internal->InteractionDelta);
    if (deltaFrames != 0) {
      this->Internal->InteractionDelta = 0;
      currentFrame += deltaFrames;
      int maxFrameIndex = this->Internal->NumberOfFrames - 1;
      currentFrame = currentFrame > maxFrameIndex ? maxFrameIndex : (currentFrame < 0 ? 0 : currentFrame);
      this->Internal->CurrentFrame = currentFrame;
    }
  }


  return true;
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

  int screenHeight = this->renderer()->height();
  double margin = 10;
  vesVector2f textSize = this->Internal->PlayRep->textureSize();
  this->Internal->PlayRep->setDisplayPosition(vesVector2f(margin, screenHeight - (margin + textSize[1])));


  if (this->Internal->LastFrame != this->Internal->CurrentFrame) {

    std::stringstream str;
    str.precision(4);
    str << "Time: " << std::fixed << 0.0001*this->Internal->CurrentFrame << " s";
    this->Internal->TextRep->setText(str.str());

    this->Internal->FrameReps[this->Internal->LastFrame]->removeSelfFromRenderer(this->renderer());
    this->Internal->FrameReps[this->Internal->CurrentFrame]->addSelfToRenderer(this->renderer());
    this->Internal->LastFrame = this->Internal->CurrentFrame;
  }
}

//----------------------------------------------------------------------------
vesKiwiPolyDataRepresentation* vesKiwiAnimationRepresentation::currentFrameRepresentation()
{
  if (this->Internal->FrameReps.size()) {
    return this->Internal->FrameReps[this->Internal->CurrentFrame];
  }
  return NULL;
}

//----------------------------------------------------------------------------
void vesKiwiAnimationRepresentation::addSelfToRenderer(vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::addSelfToRenderer(renderer);
  if (this->currentFrameRepresentation()) {
    this->currentFrameRepresentation()->addSelfToRenderer(renderer);
  }

  this->Internal->TextRep->addSelfToRenderer(renderer);
  this->Internal->PlayRep->addSelfToRenderer(renderer);
}

//----------------------------------------------------------------------------
void vesKiwiAnimationRepresentation::removeSelfFromRenderer(vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::removeSelfFromRenderer(renderer);
  if (this->currentFrameRepresentation()) {
    this->currentFrameRepresentation()->removeSelfFromRenderer(renderer);
  }

  this->Internal->TextRep->removeSelfFromRenderer(renderer);
  this->Internal->PlayRep->removeSelfFromRenderer(renderer);
}

//----------------------------------------------------------------------------
int vesKiwiAnimationRepresentation::numberOfFacets()
{
  if (this->currentFrameRepresentation()) {
    return this->currentFrameRepresentation()->numberOfFacets();
  }
  return 0;
}

//----------------------------------------------------------------------------
int vesKiwiAnimationRepresentation::numberOfVertices()
{
  if (this->currentFrameRepresentation()) {
    return this->currentFrameRepresentation()->numberOfVertices();
  }
  return 0;
}

//----------------------------------------------------------------------------
int vesKiwiAnimationRepresentation::numberOfLines()
{
  if (this->currentFrameRepresentation()) {
    return this->currentFrameRepresentation()->numberOfLines();
  }
  return 0;
}
