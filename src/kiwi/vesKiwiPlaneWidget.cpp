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

#include "vesKiwiPlaneWidget.h"

#include "vesActor.h"
#include "vesRenderer.h"
#include "vesCamera.h"
#include "vesMapper.h"
#include "vesUniform.h"
#include "vesKiwiPolyDataRepresentation.h"

#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkCellLocator.h>
#include <vtkAppendPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>

#include <vector>
#include <cassert>

//----------------------------------------------------------------------------
class vesKiwiPlaneWidget::vesInternal
{
public:

  vesInternal()
  {
    this->WidgetVisibility = true;
    this->InteractionIsRotate = false;
    this->InteractionIsTranslate = false;
    this->DefaultPlaneNormal = vesVector3d(1, 0, 0);

    this->WidgetTransform = vtkSmartPointer<vtkTransform>::New();
    this->WidgetTransform->PostMultiply();

    this->PlaneFunction = vtkSmartPointer<vtkPlane>::New();
    this->PlaneSource = vtkSmartPointer<vtkPlaneSource>::New();
    this->PlaneSource->SetNormal(this->DefaultPlaneNormal.data());

    vtkNew<vtkLineSource> line;
    line->SetPoint1(0,0,0);
    line->SetPoint2(0.5,0,0);

    this->SphereSource = vtkSmartPointer<vtkSphereSource>::New();
    this->SphereSource->SetCenter(0.5,0,0);
    this->SphereSource->SetRadius(0.03);
    this->SphereSource->SetPhiResolution(16);
    this->SphereSource->SetThetaResolution(16);

    this->Handle = vtkSmartPointer<vtkAppendPolyData>::New();
    this->Handle->AddInputConnection(line->GetOutputPort());
    this->Handle->AddInputConnection(this->SphereSource->GetOutputPort());
  }

  ~vesInternal()
  {
  }

  void updatePlaneFunction()
  {
    vesVector3d origin;
    vesVector3d normal;
    this->WidgetTransform->GetPosition(origin.data());
    this->WidgetTransform->TransformNormal(this->DefaultPlaneNormal.data(), normal.data());
    this->PlaneFunction->SetOrigin(origin.data());
    this->PlaneFunction->SetNormal(normal.data());
  }

  void setupForBrainAtlas()
  {
    // this sets the scale/position/orientation of the widget
    // so that it works well with the brain atlas demo
    double scale = 200;
    this->WidgetTransform->Scale(scale, scale, scale);
    this->WidgetTransform->RotateZ(-90);
    this->WidgetTransform->RotateX(-60);
    this->WidgetTransform->Translate(0,-50,30);
  }

  bool InteractionIsRotate;
  bool InteractionIsTranslate;
  bool WidgetVisibility;

  vesVector3d DefaultPlaneNormal;

  vtkSmartPointer<vtkPlane> PlaneFunction;
  vtkSmartPointer<vtkPlaneSource> PlaneSource;
  vtkSmartPointer<vtkAppendPolyData> Handle;
  vtkSmartPointer<vtkSphereSource> SphereSource;
  vtkSmartPointer<vtkTransform> WidgetTransform;
  vtkSmartPointer<vtkAppendPolyData> AppendFilter;

  std::vector<vesKiwiDataRepresentation::Ptr> AllReps;

  vesSharedPtr<vesUniform> ClipUniform;
  vesSharedPtr<vesKiwiPolyDataRepresentation> PlaneRep;
  vesSharedPtr<vesKiwiPolyDataRepresentation> NormalRep;
};

//----------------------------------------------------------------------------
vesKiwiPlaneWidget::vesKiwiPlaneWidget()
{
  this->Internal = new vesInternal();
  this->Internal->setupForBrainAtlas();
  this->Internal->updatePlaneFunction();
}

//----------------------------------------------------------------------------
vesKiwiPlaneWidget::~vesKiwiPlaneWidget()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vesKiwiPlaneWidget::initializeWithShader(vesSharedPtr<vesShaderProgram> geometryShader,
                                              vesSharedPtr<vesUniform> clipUniform)
{
  this->Internal->ClipUniform = clipUniform;

  this->Internal->PlaneSource->Update();
  this->Internal->Handle->Update();

  this->Internal->PlaneRep = vesKiwiPolyDataRepresentation::Ptr(new vesKiwiPolyDataRepresentation());
  this->Internal->PlaneRep->initializeWithShader(geometryShader);
  this->Internal->PlaneRep->setBinNumber(10);
  this->Internal->PlaneRep->setPolyData(this->Internal->PlaneSource->GetOutput());
  this->Internal->PlaneRep->setColor(1.0, 1.0, 0.0, 0.15);

  this->Internal->NormalRep = vesKiwiPolyDataRepresentation::Ptr(new vesKiwiPolyDataRepresentation());
  this->Internal->NormalRep->initializeWithShader(geometryShader);
  this->Internal->NormalRep->setBinNumber(1);
  this->Internal->NormalRep->setPolyData(this->Internal->Handle->GetOutput());

  // make the handle bigger so that it is easier to pick
  this->Internal->SphereSource->SetRadius(0.3);
  this->Internal->Handle->Update();

  this->Internal->AllReps.push_back(this->Internal->PlaneRep);
  this->Internal->AllReps.push_back(this->Internal->NormalRep);

  this->setTransformOnActor(this->Internal->PlaneRep->actor(), this->Internal->WidgetTransform);
  this->setTransformOnActor(this->Internal->NormalRep->actor(), this->Internal->WidgetTransform);
}

//----------------------------------------------------------------------------
vtkPlane* vesKiwiPlaneWidget::plane() const
{
  return this->Internal->PlaneFunction;
}

//----------------------------------------------------------------------------
void vesKiwiPlaneWidget::planeEquation(double equation[4])
{
  vesVector3d origin;
  vesVector3d normal;
  this->Internal->WidgetTransform->GetPosition(origin.data());
  this->Internal->WidgetTransform->TransformNormal(this->Internal->DefaultPlaneNormal.data(), normal.data());

  equation[0] = -normal[0];
  equation[1] = -normal[1];
  equation[2] = -normal[2];
  equation[3] = normal.dot(origin);
}

//----------------------------------------------------------------------------
void vesKiwiPlaneWidget::willRender(vesSharedPtr<vesRenderer> renderer)
{
  vesNotUsed(renderer);
  assert(this->Internal->ClipUniform);
  double planeEquation[4];
  this->planeEquation(planeEquation);
  this->Internal->ClipUniform->set(vesVector4f(planeEquation[0], planeEquation[1], planeEquation[2], planeEquation[3]));
}

//----------------------------------------------------------------------------
bool vesKiwiPlaneWidget::handleSingleTouchPanGesture(double deltaX, double deltaY)
{
  if (!this->interactionIsActive()) {
    return false;
  }

  deltaY *= -1;

  vesSharedPtr<vesRenderer> ren = this->renderer();
  vesSharedPtr<vesCamera> camera = ren->camera();
  int width = ren->width();
  int height = ren->height();
  vesVector3f viewFocus = camera->focalPoint();
  vesVector3f viewPlaneNormal = camera->viewPlaneNormal();
  vesVector3f viewFocusDisplay = ren->computeWorldToDisplay(viewFocus);
  float focalDepth = viewFocusDisplay[2];

  double x0 = viewFocusDisplay[0];
  double y0 = viewFocusDisplay[1];
  double x1 = x0 + deltaX;
  double y1 = y0 + deltaY;

  vesVector3f point0 = ren->computeDisplayToWorld(vesVector3f(x0, y0, focalDepth));
  vesVector3f point1 = ren->computeDisplayToWorld(vesVector3f(x1, y1, focalDepth));
  vesVector3f motionVector = point1 - point0;

  if (this->Internal->InteractionIsTranslate) {
    this->Internal->WidgetTransform->Translate(motionVector.data());
  }
  else if (this->Internal->InteractionIsRotate) {

    // Compute axis of rotation
    vesVector3f axisOfRotation = viewPlaneNormal.cross(motionVector);
    if (axisOfRotation.norm() == 0.0) {
      return true;
    }

    // Compute angle of rotation
    double motionMagnitudeSquared = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
    double theta = 360.0 * sqrt(motionMagnitudeSquared/(width*width+height*height));


    double centerOfRotation[3];
    this->Internal->WidgetTransform->GetPosition(centerOfRotation);

    // Translate to the origin, do the rotation, then move back to the current position
    this->Internal->WidgetTransform->Translate(-centerOfRotation[0],
                                               -centerOfRotation[1],
                                               -centerOfRotation[2]);
    this->Internal->WidgetTransform->RotateWXYZ(theta, axisOfRotation.data());
    this->Internal->WidgetTransform->Translate(centerOfRotation[0],
                                               centerOfRotation[1],
                                               centerOfRotation[2]);
  }

  this->setTransformOnActor(this->Internal->PlaneRep->actor(), this->Internal->WidgetTransform);
  this->setTransformOnActor(this->Internal->NormalRep->actor(), this->Internal->WidgetTransform);
  this->Internal->updatePlaneFunction();

  return true;
}

namespace {

bool PickDataSet(vtkDataSet* dataSet, const vesVector3f& rayPoint0, const vesVector3f& rayPoint1)
{
  vtkNew<vtkCellLocator> locator;
  locator->SetDataSet(dataSet);
  locator->BuildLocator();

  double p0[3] = {rayPoint0[0], rayPoint0[1], rayPoint0[2]};
  double p1[3] = {rayPoint1[0], rayPoint1[1], rayPoint1[2]};

  double pickPoint[3];
  double t;
  double paramCoords[3];
  vtkIdType cellId = -1;
  int subId;

  int result = locator->IntersectWithLine(p0, p1, 0, t, pickPoint, paramCoords, subId, cellId);
  return (result == 1);
}

}

//----------------------------------------------------------------------------
bool vesKiwiPlaneWidget::handleSingleTouchDown(int displayX, int displayY)
{
  if (!this->Internal->WidgetVisibility) {
    return false;
  }

  // calculate the focal depth so we'll know how far to move
  vesRenderer::Ptr ren = this->renderer();

  // flip Y coordinate
  displayY = ren->height() - displayY;

  vesVector3f rayPoint0 = ren->computeDisplayToWorld(vesVector3f(displayX, displayY, /*focalDepth=*/0.0));
  vesVector3f rayPoint1 = ren->computeDisplayToWorld(vesVector3f(displayX, displayY, /*focalDepth=*/1.0));

  this->Internal->WidgetTransform->GetLinearInverse()->TransformPoint(rayPoint0.data(), rayPoint0.data());
  this->Internal->WidgetTransform->GetLinearInverse()->TransformPoint(rayPoint1.data(), rayPoint1.data());

  if (PickDataSet(this->Internal->Handle->GetOutput(), rayPoint0, rayPoint1)) {
    this->Internal->InteractionIsRotate = true;
    this->Internal->InteractionIsTranslate = false;
    this->Internal->NormalRep->setColor(0.0, 1.0, 0.0, 1.0);
    this->interactionOn();
  }
  else if (PickDataSet(this->Internal->PlaneSource->GetOutput(), rayPoint0, rayPoint1)) {
    this->Internal->InteractionIsRotate = false;
    this->Internal->InteractionIsTranslate = true;
    this->Internal->PlaneRep->setColor(0.0, 1.0, 0.0, 0.15);
    this->interactionOn();
  }
  else {
    this->Internal->InteractionIsRotate = false;
    this->Internal->InteractionIsTranslate = false;
    this->interactionOff();
  }

  return this->interactionIsActive();
}

//----------------------------------------------------------------------------
bool vesKiwiPlaneWidget::handleSingleTouchTap(int displayX, int displayY)
{
  vesSharedPtr<vesRenderer> ren = this->renderer();
  displayY = ren->height() - displayY;

  int cornerSize = 80;
  if (displayX < cornerSize && (ren->height() - displayY) < cornerSize) {

    this->Internal->WidgetVisibility = !this->Internal->WidgetVisibility;
    if (this->Internal->WidgetVisibility) {
      this->Internal->PlaneRep->addSelfToRenderer(ren);
      this->Internal->NormalRep->addSelfToRenderer(ren);
    }
    else {
      this->Internal->PlaneRep->removeSelfFromRenderer(ren);
      this->Internal->NormalRep->removeSelfFromRenderer(ren);
    }
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiPlaneWidget::handleSingleTouchUp()
{
  if (!this->interactionIsActive()) {
    return false;
  }

  this->Internal->NormalRep->setColor(0.9, 0.9, 0.9, 1.0);
  this->Internal->PlaneRep->setColor(1.0, 1.0, 0.0, 0.15);
  this->Internal->InteractionIsRotate = false;
  this->Internal->InteractionIsTranslate = false;
  this->interactionOff();
  return true;
}

//----------------------------------------------------------------------------
void vesKiwiPlaneWidget::addSelfToRenderer(vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::addSelfToRenderer(renderer);
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i) {
    this->Internal->AllReps[i]->addSelfToRenderer(renderer);
  }
}

//----------------------------------------------------------------------------
void vesKiwiPlaneWidget::removeSelfFromRenderer(vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::removeSelfFromRenderer(renderer);
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i) {
    this->Internal->AllReps[i]->removeSelfFromRenderer(renderer);
  }
}

//----------------------------------------------------------------------------
int vesKiwiPlaneWidget::numberOfFacets()
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i)
    count += this->Internal->AllReps[i]->numberOfFacets();
  return count;
}

//----------------------------------------------------------------------------
int vesKiwiPlaneWidget::numberOfVertices()
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i)
    count += this->Internal->AllReps[i]->numberOfVertices();
  return count;
}

//----------------------------------------------------------------------------
int vesKiwiPlaneWidget::numberOfLines()
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i)
    count += this->Internal->AllReps[i]->numberOfLines();
  return count;
}
