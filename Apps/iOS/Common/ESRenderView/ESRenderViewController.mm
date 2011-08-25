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

#import <QuartzCore/QuartzCore.h>

#import "ESRenderViewController.h"
#import "ESRenderView.h"
#import "ESRenderer.h"
#include <vesCamera.h>
#include <vesRenderer.h>

@implementation ESRenderViewController

@synthesize renderView;
@synthesize cameraInteractionEnabled;

- (void)awakeFromNib
{

  self->renderView = (ESRenderView*)self.view;
  self.cameraInteractionEnabled = YES;
  
    // Initialize values for the touch interaction
		previousScale = 1.f;
		instantObjectScale = .01f;
		instantXRotation = 1.0f;
		instantYRotation = 0.0f;
		instantXTranslation = 0.0f;
		instantYTranslation = 0.0f;
		instantZTranslation = 0.0f;
		twoFingersAreMoving = NO;
		pinchGestureUnderway = NO;
		// stepsSinceLastRotation = 0;
		scalingForMovement = .0009f;//.00085f;//85.0f;
}


- (void)dealloc
{
  [super dealloc];
}


- (void) render
{
  // todo -
  // figure out how to schedule a render to happen later, notification center maybe?
  [self forceRender];
}

- (void) forceRender
{
  [self.renderView drawView:nil];
}




#pragma mark -
#pragma mark Touch handling

- (float)distanceBetweenTouches:(NSSet *)touches;
{
	int currentStage = 0;
	CGPoint point1 = CGPointZero;
	CGPoint point2 = CGPointZero;
	
	
	for (UITouch *currentTouch in touches)
	{
		if (currentStage == 0)
		{
			point1 = [currentTouch locationInView:self.view];
			currentStage++;
		}
		else if (currentStage == 1) 
		{
			point2 = [currentTouch locationInView:self.view];
			currentStage++;
		}
		else
		{
		}
	}
	return (sqrt((point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y)));
}

- (CGPoint)commonDirectionOfTouches:(NSSet *)touches;
{
	// Check to make sure that both fingers are moving in the same direction
	
	int currentStage = 0;
	CGPoint currentLocationOfTouch1 = CGPointZero, currentLocationOfTouch2 = CGPointZero, previousLocationOfTouch1 = CGPointZero, previousLocationOfTouch2 = CGPointZero;
	
	
	for (UITouch *currentTouch in touches)
	{
		if (currentStage == 0)
		{
			previousLocationOfTouch1 = [currentTouch previousLocationInView:self.view];
			currentLocationOfTouch1 = [currentTouch locationInView:self.view];
			currentStage++;
		}
		else if (currentStage == 1) 
		{
			previousLocationOfTouch2 = [currentTouch previousLocationInView:self.view];
			currentLocationOfTouch2 = [currentTouch locationInView:self.view];
			currentStage++;
		}
		else
		{
		}
	}
	
	CGPoint directionOfTouch1, directionOfTouch2, commonDirection;
	// The sign of the Y touches is inverted, due to the inverted coordinate system of the iPhone
	directionOfTouch1.x = currentLocationOfTouch1.x - previousLocationOfTouch1.x;
	directionOfTouch1.y = previousLocationOfTouch1.y - currentLocationOfTouch1.y;
	directionOfTouch2.x = currentLocationOfTouch2.x - previousLocationOfTouch2.x;
	directionOfTouch2.y = previousLocationOfTouch2.y - currentLocationOfTouch2.y;	
	
	// A two-finger movement should result in the direction of both touches being positive or negative at the same time in X and Y
	if (!( ((directionOfTouch1.x <= 0) && (directionOfTouch2.x <= 0)) || ((directionOfTouch1.x >= 0) && (directionOfTouch2.x >= 0)) ))
		return CGPointZero;
	if (!( ((directionOfTouch1.y <= 0) && (directionOfTouch2.y <= 0)) || ((directionOfTouch1.y >= 0) && (directionOfTouch2.y >= 0)) ))
		return CGPointZero;
	
	// The movement ranges are averaged out 
	commonDirection.x = ((directionOfTouch1.x + directionOfTouch2.x) / 2.0f) * scalingForMovement;
	commonDirection.y = ((directionOfTouch1.y + directionOfTouch2.y) / 2.0f) * scalingForMovement;
	
	
	return commonDirection;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
  [self stopInertialMotion];
  
  if (!self.cameraInteractionEnabled)
    {
    return;
    }

  NSMutableSet *currentTouches = [[[event touchesForView:self.view] mutableCopy] autorelease];
  [currentTouches minusSet:touches];
	  
	// New touches are not yet included in the current touches for the view
	NSSet *totalTouches = [touches setByAddingObjectsFromSet:[event touchesForView:self.view]];
	if ([totalTouches count] > 1)
	{
		startingTouchDistance = [self distanceBetweenTouches:totalTouches];
		previousScale = 1.0f;
		twoFingersAreMoving = NO;
		pinchGestureUnderway = NO;
		previousDirectionOfPanning = CGPointZero;
	}
	else
	{
		lastMovementPosition = [[touches anyObject] locationInView:self.view];
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
{
  if (!self.cameraInteractionEnabled)
    {
    return;
    }
    
	if ([[event touchesForView:self.view] count] > 1 )//&& [[event touchesForView:self] count] <3) // Pinch gesture, possibly two-finger movement
	{
		CGPoint directionOfPanning = CGPointZero;
		// Two finger panning
		if ([touches count] > 1 )//&& [touches count] <3) // Check to make sure that both fingers are moving
		{
			directionOfPanning = [self commonDirectionOfTouches:touches];
		}
    float newTouchDistance = [self distanceBetweenTouches:[event touchesForView:self.view]];

    previousScale = (newTouchDistance / startingTouchDistance);
  
    CGPoint currentLocationOfTouch1 = CGPointZero, currentLocationOfTouch2 = CGPointZero, previousLocationOfTouch1 = CGPointZero, previousLocationOfTouch2 = CGPointZero;
        
    int currentStage = 0;
    for (UITouch *currentTouch in touches)
      {
      if (currentStage == 0)
        {
        previousLocationOfTouch1 = [currentTouch previousLocationInView:self.view];
        currentLocationOfTouch1 = [currentTouch locationInView:self.view];
        }
      if (currentStage == 1)
        {
        previousLocationOfTouch2 = [currentTouch previousLocationInView:self.view];
        currentLocationOfTouch2 = [currentTouch locationInView:self.view];
        }
      ++currentStage;
      }

    //
    // Pan camera.
    // Implemented based on vtkInteractorStyleTrackballCamera::Pan().
    //
    
    vesRenderer* ren = [self->renderView.renderer getRenderer];

    // Average positions of current and previous two touches.
    // Invert y since vesCamera expects y to go in opposite direction.
    CGPoint previousLocation = CGPointZero;
    previousLocation.x = (previousLocationOfTouch1.x + previousLocationOfTouch2.x)/2.0;
    previousLocation.y = ren->GetHeight() - (previousLocationOfTouch1.y + previousLocationOfTouch2.y)/2.0;
    CGPoint currentLocation = CGPointZero;
    currentLocation.x = (currentLocationOfTouch1.x + currentLocationOfTouch2.x)/2.0;
    currentLocation.y = ren->GetHeight() - (currentLocationOfTouch1.y + currentLocationOfTouch2.y)/2.0;
    
    // Calculate the focal depth since we'll be using it a lot   
    vesCamera* camera = ren->GetCamera();
    vesVector3f viewFocus = camera->GetFocalPoint();
    vesVector3f viewFocusDisplay = ren->ComputeWorldToDisplay(viewFocus);
    float focalDepth = viewFocusDisplay[2];
    
    vesVector3f newPos(currentLocation.x,
                       currentLocation.y,
                       focalDepth);
    vesVector3f newPickPoint = ren->ComputeDisplayToWorld(newPos);
    
    vesVector3f oldPos(previousLocation.x,
                       previousLocation.y,
                       focalDepth);
    vesVector3f oldPickPoint = ren->ComputeDisplayToWorld(oldPos);
    
    vesVector3f motionVector = oldPickPoint - newPickPoint;
    
    vesVector3f viewPoint = camera->GetPosition();
    vesVector3f newViewFocus = motionVector + viewFocus;
    vesVector3f newViewPoint = motionVector + viewPoint;
    camera->SetFocalPoint(newViewFocus);
    camera->SetPosition(newViewPoint);

    //
    // Zoom camera.
    // Implemented based on vkInteractorStyleTrackballCamera::Dolly().
    //
    
    double previousDist = sqrt((previousLocationOfTouch1.x - previousLocationOfTouch2.x) *
                               (previousLocationOfTouch1.x - previousLocationOfTouch2.x) + 
                               (previousLocationOfTouch1.y - previousLocationOfTouch2.y) * 
                               (previousLocationOfTouch1.y - previousLocationOfTouch2.y));
    double currentDist = sqrt((currentLocationOfTouch1.x - currentLocationOfTouch2.x) *
                              (currentLocationOfTouch1.x - currentLocationOfTouch2.x) + 
                              (currentLocationOfTouch1.y - currentLocationOfTouch2.y) * 
                              (currentLocationOfTouch1.y - currentLocationOfTouch2.y));
    double dy = currentDist - previousDist;
    double dyf = 10.0 * dy / (ren->GetHeight()/2.0);
    double factor = pow(1.1, dyf);
    camera->Dolly(factor);
    
    //
    // Roll camera.
    // Implemented based on vkInteractorStyleTrackballCamera::Spin().
    //
    
    double pi = 3.14159265358979;
    double newAngle = atan2(currentLocationOfTouch1.y - currentLocationOfTouch2.y,
                            currentLocationOfTouch1.x - currentLocationOfTouch2.x);
    newAngle *= 180.0/pi;
    
    double oldAngle = atan2(previousLocationOfTouch1.y - previousLocationOfTouch2.y,
                            previousLocationOfTouch1.x - previousLocationOfTouch2.x);
    oldAngle *= 180.0/pi;
    
    camera->Roll(newAngle - oldAngle);
    camera->OrthogonalizeViewUp();
  }
	else // Single-touch rotation of object
	{
		CGPoint currentMovementPosition = [[touches anyObject] locationInView:self.view];
    
    CGPoint lastMovementXYDelta;
    lastMovementXYDelta.x = currentMovementPosition.x - lastMovementPosition.x;
    lastMovementXYDelta.y = currentMovementPosition.y - lastMovementPosition.y;

    // compute unit delta so that we can easily compute inertia later
    lastRotationMotionNorm = sqrtf(lastMovementXYDelta.x*lastMovementXYDelta.x + 
                                   lastMovementXYDelta.y*lastMovementXYDelta.y);
    lastMovementXYUnitDelta.x = lastMovementXYDelta.x / lastRotationMotionNorm;
    lastMovementXYUnitDelta.y = lastMovementXYDelta.y / lastRotationMotionNorm;
    
    [self rotate:lastMovementXYDelta];
    
    lastMovementPosition = currentMovementPosition;
	}
  [self->renderView drawView:nil];
}

- (void)rotate: (CGPoint)delta
{
  //
  // Rotate camera
  // Based on vtkInteractionStyleTrackballCamera::Rotate().
  //

  vesRenderer* ren = [self->renderView.renderer getRenderer];
  
  double delta_elevation = -20.0 / ren->GetHeight();
  double delta_azimuth = -20.0 / ren->GetWidth();
  double motionFactor = 10.0;
  
  double rxf = delta.x * delta_azimuth * motionFactor;
  double ryf = delta.y * delta_elevation * motionFactor;
  
  vesCamera *camera = [self->renderView.renderer getRenderer]->GetCamera();
  camera->Azimuth(rxf);
  camera->Elevation(ryf);
  camera->OrthogonalizeViewUp();  
}

- (void)handleInertialRotation
{
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  CGPoint delta;
  while (lastRotationMotionNorm > 0.5)
  {
    [NSThread sleepForTimeInterval:1/30.0];
    
    if ([[NSThread currentThread] isCancelled])
    {
      break;
    }
    
    delta.x = lastRotationMotionNorm*lastMovementXYUnitDelta.x;
    delta.y = lastRotationMotionNorm*lastMovementXYUnitDelta.y;
    [self rotate:delta];

    [self->renderView drawView:nil];
    lastRotationMotionNorm *= 0.8;
  }
  lastRotationMotionNorm = 0;
  [pool release];
  //[NSThread exit];
}

- (void) stopInertialMotion
{
  if (inertialRotationThread)
  {
    [inertialRotationThread setThreadPriority:1.0f];
    [inertialRotationThread cancel];
    
    // TODO: something is wrong with this implementation so there is a hack here. 
    // This busy wait is causing a jerky stop to the inertial rotation so I'm setting the 
    // priorty of the inertia thread up high and pausing here a bit so that it can finish 
    // its render.  The priority and sleep should be removed when the wait is improved
    [NSThread sleepForTimeInterval:1/20.0];
    while (![inertialRotationThread isFinished])
    {
      // busy wait for the thread to exit
    }
    [inertialRotationThread release];
    inertialRotationThread = nil;
  }
}

- (void)handleTouchesEnding:(NSSet *)touches withEvent:(UIEvent *)event
{
  if (!self.cameraInteractionEnabled)
    {
    return;
    }

    NSMutableSet *remainingTouches = [[[event touchesForView:self.view] mutableCopy] autorelease];
    [remainingTouches minusSet:touches];
	if ([remainingTouches count] < 2)
	{
		twoFingersAreMoving = NO;
		pinchGestureUnderway = NO;
		previousDirectionOfPanning = CGPointZero;
		
		lastMovementPosition = [[remainingTouches anyObject] locationInView:self.view];
	}	
  
  if ([remainingTouches count] == 0 && lastRotationMotionNorm > 4.0f)
  {
    inertialRotationThread = [[NSThread alloc] initWithTarget:self selector:@selector(handleInertialRotation) object:nil];
    [inertialRotationThread start];
  }
}


- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event 
{
	[self handleTouchesEnding:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event 
{
	[self handleTouchesEnding:touches withEvent:event];
}


@end
