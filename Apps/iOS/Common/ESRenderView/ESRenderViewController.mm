/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    ESRenderViewController.mm
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#import <QuartzCore/QuartzCore.h>

#import "ESRenderViewController.h"
#import "ESRenderView.h"
#import "ESRenderer.h"

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
#if 1
		CGPoint directionOfPanning = CGPointZero;
		
		// Two finger panning
		if ([touches count] > 1 )//&& [touches count] <3) // Check to make sure that both fingers are moving
		{
			directionOfPanning = [self commonDirectionOfTouches:touches];
		}
    float newTouchDistance = [self distanceBetweenTouches:[event touchesForView:self.view]];
    // Scale using pinch gesture

    [self->renderView.renderer _drawViewByRotatingAroundX:0.0 rotatingAroundY:0.0 scaling:(newTouchDistance / startingTouchDistance) / previousScale 
                            //translationInX:0 translationInY:0];
      translationInX:directionOfPanning.x translationInY:directionOfPanning.y];
    previousScale = (newTouchDistance / startingTouchDistance);
#else
		CGPoint directionOfPanning = CGPointZero;
		
		// Two finger panning
		if ([touches count] > 1) // Check to make sure that both fingers are moving
		{
			directionOfPanning = [self commonDirectionOfTouches:touches];
		}
		
		if ( (directionOfPanning.x != 0) || (directionOfPanning.y != 0) ) // Don't scale while doing the two-finger panning
		{
			if (pinchGestureUnderway)
			{
				
				
				if (sqrt(previousDirectionOfPanning.x * previousDirectionOfPanning.x + previousDirectionOfPanning.y * previousDirectionOfPanning.y) > 0.1 )
				{
					pinchGestureUnderway = NO;
				}
				previousDirectionOfPanning.x += directionOfPanning.x;
				previousDirectionOfPanning.y += directionOfPanning.y;
			}
			if (!pinchGestureUnderway)
			{
				twoFingersAreMoving = YES;
				[renderer _drawViewByRotatingAroundX:0.0f rotatingAroundY:0.0f scaling:1.0f translationInX:directionOfPanning.x translationInY:directionOfPanning.y];
				previousDirectionOfPanning = CGPointZero;
			}
		}
		else
		{
			float newTouchDistance = [self distanceBetweenTouches:[event touchesForView:self]];
			if (twoFingersAreMoving)
			{
				// If fingers have moved more than 10% apart, start pinch gesture again
				if ( fabs(1 - (newTouchDistance / startingTouchDistance) / previousScale) > 0.6 )
				{
					twoFingersAreMoving = NO;
				}
			}
			if (!twoFingersAreMoving)
			{
				// Scale using pinch gesture
				[renderer _drawViewByRotatingAroundX:0.0 rotatingAroundY:0.0 scaling:(newTouchDistance / startingTouchDistance) / previousScale 
                              translationInX:0 translationInY:0];
                              //translationInX:directionOfPanning.x translationInY:directionOfPanning.y];
				previousScale = (newTouchDistance / startingTouchDistance);
				pinchGestureUnderway = YES;
			}
		}
#endif
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

		[self->renderView.renderer _drawViewByRotatingAroundX:(lastMovementXYDelta.x) rotatingAroundY:(lastMovementXYDelta.y) scaling:1.0f translationInX:0.0f translationInY:0.0f];
		lastMovementPosition = currentMovementPosition;
	}
  
  [self->renderView drawView:nil];
}

- (void)handleInertialRotation
{
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  float velocityDelta = 4.5;
  while (lastRotationMotionNorm > velocityDelta)
  {
    [NSThread sleepForTimeInterval:1/30.0];
    
    if ([[NSThread currentThread] isCancelled])
    {
      break;
    }

    [self->renderView.renderer _drawViewByRotatingAroundX:(lastRotationMotionNorm*lastMovementXYUnitDelta.x) 
                         rotatingAroundY:(lastRotationMotionNorm*lastMovementXYUnitDelta.y) 
                                 scaling:1.0f 
                          translationInX:0.0f 
                          translationInY:0.0f];
    [self->renderView drawView:nil];
    lastRotationMotionNorm -= velocityDelta;
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
  
  if ([remainingTouches count] == 0 && lastRotationMotionNorm > 0.0f)
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
