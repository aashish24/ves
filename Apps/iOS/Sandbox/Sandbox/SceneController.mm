//
//  SceneController.mm
//

#import "SceneController.h"
#import "ESRenderView.h"
#import "ESRenderer.h"

#include "vesActor.h"
#include "vesMapper.h"
#include "vesRenderer.h"
#include "vesPolyDataToTriangleData.h"

#include "vtkSmartPointer.h"
#include "vtkConeSource.h"

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
-(vesTriangleData*)makeConeWithCenter:(double[3])center
{
  vtkSmartPointer<vtkConeSource> cone = vtkSmartPointer<vtkConeSource>::New();
  cone->SetCenter(center);
  cone->SetDirection(0,1,0);
  cone->SetResolution(24);
  cone->Update();
  return vesPolyDataToTriangleData::Convert(cone->GetOutput());
}

//------------------------------------------------------------------------------
-(void)initializeScene
{
  double center[3] = {0,0,0};
  vesTriangleData* data = [self makeConeWithCenter:center];
  vesActor* actor = [self addActorWithData:data];
  actor->SetColor(0, 0.8, 0, .5);
  vesVector4f angleAxis(0, 0, 1, M_PI_2);
  actor->SetRotation(angleAxis);

  center[0] = 3;
  data = [self makeConeWithCenter:center];
  actor = [self addActorWithData:data];
  actor->SetColor(0, 0.0, 0.8, .5);
  vesVector3f translation(-3, 1, 0);
  actor->SetTranslation(translation);
}


//------------------------------------------------------------------------------
-(void)resetView
{
  [self->renderView resetView];
  [self->renderView drawView:nil];
}

@end
