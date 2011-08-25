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

#import "SceneController.h"
#import "ESRenderView.h"
#import "ESRenderer.h"

#include "vesActor.h"
#include "vesMapper.h"
#include "vesRenderer.h"
#include "vesPolyDataToTriangleData.h"

#include "vtkSmartPointer.h"
#include "vtkConeSource.h"
#include "vtkLineSource.h"
#include "vtkSphereSource.h"
#include "vtkTubeFilter.h"

//------------------------------------------------------------------------------
class SceneControllerImpl
{
public:

  NSMutableArray* Actors;
};

//------------------------------------------------------------------------------
@interface SceneController ()
-(vesActor*)addActorWithData:(vesTriangleData*)data;
-(void)cleanupActors;
@end

@implementation SceneController
@synthesize renderView;


//------------------------------------------------------------------------------
- (id)init
{
  self = [super init];
  if (self)
    {
    self->Internal = new SceneControllerImpl;
    self->Internal->Actors = [NSMutableArray new];
    }
  return self;
}

//------------------------------------------------------------------------------
- (void)dealloc
{
  [self cleanupActors];
  [self->Internal->Actors release];
  delete self->Internal;
  [super dealloc];
}

//------------------------------------------------------------------------------
-(vesActor*)addActorWithData:(vesTriangleData*)data
{
  vesMapper* mapper = new vesMapper();
  mapper->SetTriangleData(data);
  vesActor* actor = new vesActor([self.renderView.renderer getShader], mapper);
  [self.renderView.renderer getRenderer]->AddActor(actor);
  [self->Internal->Actors addObject:[NSValue valueWithPointer:actor]];
  return actor;
}

//------------------------------------------------------------------------------
-(void)cleanupActors
{
  for (NSValue* actorPointerValue in self->Internal->Actors)
    {
    vesActor* tempActor = (vesActor*)[actorPointerValue pointerValue];
    if (self->renderView)
      {
      [self->renderView.renderer getRenderer]->RemoveActor(tempActor);
      delete tempActor->GetMapper()->GetTriangleData();
      delete tempActor->GetMapper();
      delete tempActor;
      }
    }
  [self->Internal->Actors removeAllObjects];
}

//------------------------------------------------------------------------------
-(vesTriangleData*)makeSphereWithCenter:(double[3])center Radius:(double)radius
{
  vtkSmartPointer<vtkSphereSource> cone = vtkSmartPointer<vtkSphereSource>::New();
  cone->SetCenter(center);
  cone->SetRadius(radius);
  cone->SetPhiResolution(24);
  cone->SetThetaResolution(24);
  cone->Update();
  return vesPolyDataToTriangleData::Convert(cone->GetOutput());
}

//------------------------------------------------------------------------------
-(vesTriangleData*)makeConeWithCenter:(double[3])center Scale:(double)scale
{
  vtkSmartPointer<vtkConeSource> cone = vtkSmartPointer<vtkConeSource>::New();
  cone->SetCenter(center);
  cone->SetDirection(0,1,0);
  cone->SetRadius(cone->GetRadius()*scale);
  cone->SetHeight(cone->GetHeight()*scale);
  cone->SetResolution(24);
  cone->Update();
  return vesPolyDataToTriangleData::Convert(cone->GetOutput());
}

//------------------------------------------------------------------------------
-(vesTriangleData*)makeTubeWithEndPoints:(double[6])endPoints Radius:(double)radius
{
  vtkSmartPointer<vtkLineSource> line = vtkSmartPointer<vtkLineSource>::New();
  line->SetPoint1(endPoints);
  line->SetPoint2(endPoints+3);
  vtkSmartPointer<vtkTubeFilter> tube = vtkSmartPointer<vtkTubeFilter>::New();
  tube->SetInputConnection(line->GetOutputPort());
  tube->SetNumberOfSides(24);
  tube->SetRadius(radius);
  tube->Update();
  return vesPolyDataToTriangleData::Convert(tube->GetOutput());
}

//------------------------------------------------------------------------------
-(void)initializeScene
{
  vesActor* actor;

  double endPoints[6] = {-100,-200,200,  100,-200,200};

  actor = [self addActorWithData:[self makeTubeWithEndPoints:endPoints Radius:10]];
  actor->SetColor(0.8, 0, 0, 1.0);

  actor = [self addActorWithData:[self makeSphereWithCenter:endPoints Radius:20]];
  actor->SetColor(0, 0.8, 0, 1.0);

  actor = [self addActorWithData:[self makeSphereWithCenter:endPoints+3 Radius:20]];
  actor->SetColor(0, 0, 0.8, 1.0);

  [self resetView];
}


//------------------------------------------------------------------------------
-(void)resetView
{

  // reset the camera
  //[self->renderView.renderer getRenderer]->ResetCamera();

  // set camera focal point to [-100,-200,200] so the green sphere is centered in the view
  // set camera position to be [-100,-200,200] + X*[0,0,-1] so that the red cylinder is perpendicular to the view direction
  vesCamera* camera = [self->renderView.renderer getCamera];
  camera->SetFocalPoint(vesVector3f(-100, -200, 200));
  camera->SetPosition(vesVector3f(-100, -200, -200));
  camera->SetViewUp(vesVector3f(-1, 0, 0));

  [self->renderView drawView:nil];
}

@end
