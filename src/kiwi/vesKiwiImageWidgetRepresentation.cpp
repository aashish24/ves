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

#include "vesKiwiImageWidgetRepresentation.h"

#include "vesRenderer.h"
#include "vesCamera.h"
#include "vesMapper.h"
#include "vesKiwiDataConversionTools.h"
#include "vesKiwiImagePlaneDataRepresentation.h"
#include "vesKiwiPolyDataRepresentation.h"

#include <vtkNew.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkLookupTable.h>
#include <vtkOutlineFilter.h>
#include <vtkPointData.h>
#include <vtkExtractVOI.h>
#include <vtkContourFilter.h>
#include <vtkCellLocator.h>
#include <vtkAppendPolyData.h>

#include <vector>
#include <map>
#include <cassert>

//----------------------------------------------------------------------------
class vesKiwiImageWidgetRepresentation::vesInternal
{
public:

  vesInternal()
  {
    this->SelectedImageDimension = -1;
    this->ContourVis = 0;
    this->ImageScalarRange[0] = 0;
    this->ImageScalarRange[1] = 1;
    for (int i = 0; i < 3; ++i)
      this->CurrentSliceIndices[i] = 0;

    this->ContourRep = 0;
    this->OutlineRep = 0;
    this->UseContour = false;
    this->InteractionEnabled = true;
  }

  ~vesInternal()
  {
    for (size_t i = 0; i < this->AllReps.size(); ++i) {
      delete this->AllReps[i];
    }
  }


  int SelectedImageDimension;
  int CurrentSliceIndices[3];
  int ContourVis;
  bool UseContour;
  bool InteractionEnabled;
  double ImageScalarRange[2];

  std::vector<vesKiwiDataRepresentation*> AllReps;
  std::vector<vesKiwiImagePlaneDataRepresentation*> SliceReps;

  std::map<int, int> TargetSliceIndex;

  vesKiwiPolyDataRepresentation* ContourRep;
  vesKiwiPolyDataRepresentation* OutlineRep;

  vtkSmartPointer<vtkExtractVOI> SliceFilter;
  vtkSmartPointer<vtkCellLocator> Locator;
  vtkSmartPointer<vtkAppendPolyData> AppendFilter;
};

//----------------------------------------------------------------------------
vesKiwiImageWidgetRepresentation::vesKiwiImageWidgetRepresentation()
{
  this->Internal = new vesInternal();
}

//----------------------------------------------------------------------------
vesKiwiImageWidgetRepresentation::~vesKiwiImageWidgetRepresentation()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::willRender(vesSharedPtr<vesRenderer> renderer)
{
  vesNotUsed(renderer);

  if (this->Internal->TargetSliceIndex.size()) {

    std::map<int, int>::const_iterator itr;
    for (itr = this->Internal->TargetSliceIndex.begin(); itr != this->Internal->TargetSliceIndex.end(); ++itr) {
      this->setSliceIndex(itr->first, itr->second);
    }

    this->Internal->TargetSliceIndex.clear();
  }
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::setImageData(vtkImageData* image)
{
  image->GetPointData()->GetScalars()->GetRange(this->Internal->ImageScalarRange);
  vtkSmartPointer<vtkScalarsToColors> colorMap =
    vesKiwiDataConversionTools::GetGrayscaleLookupTable(this->Internal->ImageScalarRange);
  for (int i = 0; i < 3; ++i)
    this->Internal->SliceReps[i]->setColorMap(colorMap);


  int dimensions[3];
  image->GetDimensions(dimensions);
  this->Internal->CurrentSliceIndices[0] = dimensions[0]/2;
  this->Internal->CurrentSliceIndices[1] = dimensions[1]/2;
  this->Internal->CurrentSliceIndices[2] = dimensions[2]/2;

  this->Internal->SliceFilter = vtkSmartPointer<vtkExtractVOI>::New();
  this->Internal->SliceFilter->SetInputData(image);

  for (int i = 0; i < 3; ++i)
    this->setSliceIndex(i, this->Internal->CurrentSliceIndices[i]);

  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputData(image);
  outline->Update();
  this->Internal->OutlineRep->setPolyData(outline->GetOutput());
  this->Internal->OutlineRep->setColor(0.5, 0.5, 0.5, 0.5);

  if (image->GetNumberOfPoints() < 600000) {
    vtkNew<vtkContourFilter> contour;
    contour->SetInputData(image);
    // contour value hardcoded for head image dataset
    contour->SetValue(0, 1400);
    contour->ComputeScalarsOff();
    contour->ComputeNormalsOff();
    contour->Update();

    this->Internal->ContourRep->setPolyData(contour->GetOutput());
    this->Internal->ContourRep->setColor(0.8, 0.8, 0.8, 0.4);
    this->Internal->ContourVis = 1;
    this->Internal->AllReps.push_back(this->Internal->ContourRep);
    this->Internal->UseContour = true;
  }
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::initializeWithShader(
  vesSharedPtr<vesShaderProgram> geometryShader,
  vesSharedPtr<vesShaderProgram> textureShader)
{

  this->Internal->AppendFilter = vtkSmartPointer<vtkAppendPolyData>::New();

  this->Internal->ContourRep = new vesKiwiPolyDataRepresentation();
  this->Internal->ContourRep->initializeWithShader(geometryShader);
  this->Internal->ContourRep->setBinNumber(2);
  this->Internal->OutlineRep = new vesKiwiPolyDataRepresentation();
  this->Internal->OutlineRep->initializeWithShader(geometryShader);
  this->Internal->OutlineRep->setBinNumber(2);
  this->Internal->AllReps.push_back(this->Internal->OutlineRep);

  for (int i = 0; i < 3; ++i) {
    vesKiwiImagePlaneDataRepresentation* rep = new vesKiwiImagePlaneDataRepresentation();
    rep->initializeWithShader(textureShader);
    rep->setBinNumber(1);
    this->Internal->SliceReps.push_back(rep);
    this->Internal->AllReps.push_back(rep);
    this->Internal->AppendFilter->AddInputData(vtkSmartPointer<vtkPolyData>::New());
  }

}

//----------------------------------------------------------------------------
bool vesKiwiImageWidgetRepresentation::scrollSliceModeActive() const
{
  return (this->Internal->SelectedImageDimension >= 0);
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::setShaderProgram(
  vesSharedPtr<vesShaderProgram> shaderProgram)
{
  if (!shaderProgram) {
    return;
  }

  // Use new shader only for polydata representations.
  this->Internal->ContourRep->setShaderProgram(shaderProgram);
  this->Internal->OutlineRep->setShaderProgram(shaderProgram);
}

//----------------------------------------------------------------------------
vesSharedPtr<vesShaderProgram> vesKiwiImageWidgetRepresentation::shaderProgram() const
{
  return this->Internal->ContourRep->shaderProgram();
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::scrollImageSlice(double deltaX, double deltaY)
{
  deltaY *= -1;

  vesSharedPtr<vesRenderer> ren = this->renderer();
  vesSharedPtr<vesCamera> camera = ren->camera();
  vesVector3f viewFocus = camera->focalPoint();
  vesVector3f viewFocusDisplay = ren->computeWorldToDisplay(viewFocus);
  float focalDepth = viewFocusDisplay[2];

  double x0 = viewFocusDisplay[0];
  double y0 = viewFocusDisplay[1];
  double x1 = x0 + deltaX;
  double y1 = y0 + deltaY;

  // map change into world coordinates
  vesVector3f point0 = ren->computeDisplayToWorld(vesVector3f(x0, y0, focalDepth));
  vesVector3f point1 = ren->computeDisplayToWorld(vesVector3f(x1, y1, focalDepth));
  vesVector3f motionVector = point1 - point0;

  int flatDimension = this->Internal->SelectedImageDimension;

  vesVector3f planeNormal(0, 0, 0);
  planeNormal[flatDimension] = 1.0;

  double vectorDot = motionVector.dot(planeNormal);
  double delta = vectorDot;
  if (fabs(delta) < 1e-6) {
    delta = deltaY;
  }

  int sliceDelta = static_cast<int>(delta);
  if (sliceDelta == 0) {
    sliceDelta = delta > 0 ? 1 : -1;
  }

  int sliceIndex = this->Internal->CurrentSliceIndices[flatDimension] + sliceDelta;
  this->Internal->TargetSliceIndex[flatDimension] = sliceIndex;
  this->Internal->CurrentSliceIndices[flatDimension] = sliceIndex;
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::setSliceIndex(int planeIndex, int sliceIndex)
{
  int dimensions[3];
  vtkImageData::SafeDownCast(this->Internal->SliceFilter->GetInput())->GetDimensions(dimensions);

  if (sliceIndex < 0) {
    sliceIndex = 0;
  }
  else if (sliceIndex >= dimensions[planeIndex]) {
    sliceIndex = dimensions[planeIndex] - 1;
  }

  if (planeIndex == 0) {
    this->Internal->SliceFilter->SetVOI(sliceIndex, sliceIndex, 0, dimensions[1], 0, dimensions[2]);
  }
  else if (planeIndex == 1) {
    this->Internal->SliceFilter->SetVOI(0, dimensions[0], sliceIndex, sliceIndex, 0, dimensions[2]);
  }
  else {
    this->Internal->SliceFilter->SetVOI(0, dimensions[0], 0, dimensions[1], sliceIndex, sliceIndex);
  }

  this->Internal->SliceFilter->Update();
  vtkImageData* sliceImage = this->Internal->SliceFilter->GetOutput();

  vesKiwiImagePlaneDataRepresentation* rep = this->Internal->SliceReps[planeIndex];
  rep->setImageData(sliceImage);
  this->Internal->AppendFilter->GetInput(planeIndex)->DeepCopy(rep->imagePlanePolyData());
  this->Internal->CurrentSliceIndices[planeIndex] = sliceIndex;
}

//----------------------------------------------------------------------------
bool vesKiwiImageWidgetRepresentation::handleSingleTouchPanGesture(double deltaX, double deltaY)
{
  if (!this->interactionIsActive()) {
    return false;
  }

  this->scrollImageSlice(deltaX, deltaY);
  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiImageWidgetRepresentation::handleSingleTouchDown(int displayX, int displayY)
{
  if (!this->Internal->InteractionEnabled) {
    return false;
  }

  // calculate the focal depth so we'll know how far to move
  vesSharedPtr<vesRenderer> ren = this->renderer();

  // flip Y coordinate
  displayY = ren->height() - displayY;

  std::tr1::shared_ptr<vesCamera> camera = ren->camera();
  vesVector3f cameraFocalPoint = camera->focalPoint();
  vesVector3f cameraPosition = camera->position();
  vesVector3f displayFocus = ren->computeWorldToDisplay(cameraFocalPoint);
  float focalDepth = displayFocus[2];

  vesVector3f rayPoint0 = cameraPosition;
  vesVector3f rayPoint1 = ren->computeDisplayToWorld(vesVector3f(displayX, displayY, focalDepth));

  vesVector3f rayDirection = rayPoint1 - rayPoint0;

  rayDirection.normalize();
  rayDirection *= 1000.0;
  rayPoint1 += rayDirection;

  vtkNew<vtkCellLocator> locator;
  this->Internal->AppendFilter->Update();
  locator->SetDataSet(this->Internal->AppendFilter->GetOutput());
  locator->BuildLocator();

  double p0[3] = {rayPoint0[0], rayPoint0[1], rayPoint0[2]};
  double p1[3] = {rayPoint1[0], rayPoint1[1], rayPoint1[2]};

  double pickPoint[3];
  double t;
  double paramCoords[3];
  vtkIdType cellId = -1;
  int subId;

  int result = locator->IntersectWithLine(p0, p1, 0.0, t, pickPoint, paramCoords, subId, cellId);
  if (result == 1) {
    this->Internal->SelectedImageDimension = cellId;
    this->interactionOn();
  }
  else {
    this->Internal->SelectedImageDimension = -1;
    this->interactionOff();
  }

  return this->interactionIsActive();
}

//----------------------------------------------------------------------------
bool vesKiwiImageWidgetRepresentation::handleDoubleTap(int displayX, int displayY)
{
  vesNotUsed(displayX);
  vesNotUsed(displayY);

  if (!this->Internal->UseContour) {

    this->Internal->InteractionEnabled = !this->Internal->InteractionEnabled;
    return true;
  }

  this->Internal->ContourVis = (this->Internal->ContourVis + 1) % 3;
  if (this->Internal->ContourVis == 0) {
    this->Internal->ContourRep->removeSelfFromRenderer(this->renderer());
  }
  else if (this->Internal->ContourVis == 1) {
    this->Internal->ContourRep->addSelfToRenderer(this->renderer());
    this->Internal->ContourRep->mapper()->setColor(0.8, 0.8, 0.8, 0.3);
  }
  else {
    this->Internal->ContourRep->mapper()->setColor(0.8, 0.8, 0.8, 1.0);
  }

  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiImageWidgetRepresentation::handleSingleTouchUp()
{
  if (!this->interactionIsActive()) {
    return false;
  }

  this->Internal->SelectedImageDimension = -1;
  this->interactionOff();
  return true;
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::addSelfToRenderer(
  vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::addSelfToRenderer(renderer);
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i) {
    this->Internal->AllReps[i]->addSelfToRenderer(renderer);
  }
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::removeSelfFromRenderer(
  vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::removeSelfFromRenderer(renderer);
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i) {
    this->Internal->AllReps[i]->removeSelfFromRenderer(renderer);
  }
}

//----------------------------------------------------------------------------
int vesKiwiImageWidgetRepresentation::numberOfFacets()
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i)
    count += this->Internal->AllReps[i]->numberOfFacets();
  return count;
}

//----------------------------------------------------------------------------
int vesKiwiImageWidgetRepresentation::numberOfVertices()
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i)
    count += this->Internal->AllReps[i]->numberOfVertices();
  return count;
}

//----------------------------------------------------------------------------
int vesKiwiImageWidgetRepresentation::numberOfLines()
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i)
    count += this->Internal->AllReps[i]->numberOfLines();
  return count;
}
