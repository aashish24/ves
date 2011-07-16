/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    EAGLView.mm
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#import "EAGLView.h"
#import "ES2Renderer.h"

#define USE_DEPTH_BUFFER 1

@interface EAGLView ()
@property (nonatomic, retain) EAGLContext *context;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;


@end

@implementation EAGLView

@synthesize context;

// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder
{    
  self = [super initWithCoder:coder];
  if (self)
  {
    // Get the layer
    CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
    
    eaglLayer.opaque = TRUE;
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        if (!context || ![EAGLContext setCurrentContext:context])
        {
          [self release];
          return nil;
        }
    

    renderer = [[ES2Renderer alloc] init];
    
    if (!renderer)
    {
			[self release];
			return nil;
    }
    
    //delegate =nil;
    self.multipleTouchEnabled = YES;
		
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
  
  return self;
}

- (void)layoutSubviews {
  [EAGLContext setCurrentContext:context];
  [self destroyFramebuffer];
  [self createFramebuffer];
  [renderer resizeFromLayer:backingWidth height:backingHeight];
  [self drawView:nil];
}

- (BOOL)createFramebuffer {
  
  glGenFramebuffers(1, &viewFramebuffer);
  glGenRenderbuffers(1, &viewRenderbuffer);
  
  glBindFramebuffer(GL_FRAMEBUFFER, viewFramebuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
  [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, viewRenderbuffer);
  
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
  
  if (USE_DEPTH_BUFFER) {
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
  }
  
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    return NO;
  }
  
  return YES;
}


- (void)destroyFramebuffer {
  
  glDeleteFramebuffers(1, &viewFramebuffer);
  viewFramebuffer = 0;
  glDeleteRenderbuffers(1, &viewRenderbuffer);
  viewRenderbuffer = 0;
  
  if(depthRenderbuffer) {
    glDeleteRenderbuffers(1, &depthRenderbuffer);
    depthRenderbuffer = 0;
  }
}

- (void)drawView:(id) sender {  
  [EAGLContext setCurrentContext:context];
  glBindFramebuffer(GL_FRAMEBUFFER, viewFramebuffer);
  glViewport(0, 0, backingWidth, backingHeight);
  [renderer render];
  glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
  [context presentRenderbuffer:GL_RENDERBUFFER];
}


- (void)dealloc
{
  if ([EAGLContext currentContext] == context)
    [EAGLContext setCurrentContext:nil];
	
  [context release];
  context = nil;
  [renderer release];
  [super dealloc];
}



- (void)resetView
{
  [self stopInertialMotion]; 
  [renderer resetView];
  [self drawView:nil];
}

- (void) setFilePath :(NSString *) fpath
{
	if(renderer){
		[renderer setFilePath:fpath];
    [self resetView];
	}
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
			point1 = [currentTouch locationInView:self];
			currentStage++;
		}
		else if (currentStage == 1) 
		{
			point2 = [currentTouch locationInView:self];
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
			previousLocationOfTouch1 = [currentTouch previousLocationInView:self];
			currentLocationOfTouch1 = [currentTouch locationInView:self];
			currentStage++;
		}
		else if (currentStage == 1) 
		{
			previousLocationOfTouch2 = [currentTouch previousLocationInView:self];
			currentLocationOfTouch2 = [currentTouch locationInView:self];
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

  NSMutableSet *currentTouches = [[[event touchesForView:self] mutableCopy] autorelease];
  [currentTouches minusSet:touches];
	  
	// New touches are not yet included in the current touches for the view
	NSSet *totalTouches = [touches setByAddingObjectsFromSet:[event touchesForView:self]];
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
		lastMovementPosition = [[touches anyObject] locationInView:self];
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
{
	if ([[event touchesForView:self] count] > 1 )//&& [[event touchesForView:self] count] <3) // Pinch gesture, possibly two-finger movement
	{
#if 1
		CGPoint directionOfPanning = CGPointZero;
		
		// Two finger panning
		if ([touches count] > 1 )//&& [touches count] <3) // Check to make sure that both fingers are moving
		{
			directionOfPanning = [self commonDirectionOfTouches:touches];
		}
    float newTouchDistance = [self distanceBetweenTouches:[event touchesForView:self]];
    // Scale using pinch gesture
    [renderer _drawViewByRotatingAroundX:0.0 rotatingAroundY:0.0 scaling:(newTouchDistance / startingTouchDistance) / previousScale 
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
		CGPoint currentMovementPosition = [[touches anyObject] locationInView:self];
    
    CGPoint lastMovementXYDelta;
    lastMovementXYDelta.x = currentMovementPosition.x - lastMovementPosition.x;
    lastMovementXYDelta.y = currentMovementPosition.y - lastMovementPosition.y;

    // compute unit delta so that we can easily compute inertia later
    lastRotationMotionNorm = sqrtf(lastMovementXYDelta.x*lastMovementXYDelta.x + 
                                   lastMovementXYDelta.y*lastMovementXYDelta.y);
    lastMovementXYUnitDelta.x = lastMovementXYDelta.x / lastRotationMotionNorm;
    lastMovementXYUnitDelta.y = lastMovementXYDelta.y / lastRotationMotionNorm;
    
		[renderer _drawViewByRotatingAroundX:(lastMovementXYDelta.x) rotatingAroundY:(lastMovementXYDelta.y) scaling:1.0f translationInX:0.0f translationInY:0.0f];
		lastMovementPosition = currentMovementPosition;
	}
	 [self drawView:nil];
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
    
    [renderer _drawViewByRotatingAroundX:(lastRotationMotionNorm*lastMovementXYUnitDelta.x) 
                         rotatingAroundY:(lastRotationMotionNorm*lastMovementXYUnitDelta.y) 
                                 scaling:1.0f 
                          translationInX:0.0f 
                          translationInY:0.0f];
    [self drawView:nil];
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

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event 
{
	[self handleTouchesEnding:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event 
{
	[self handleTouchesEnding:touches withEvent:event];
}

- (void)handleTouchesEnding:(NSSet *)touches withEvent:(UIEvent *)event
{
    NSMutableSet *remainingTouches = [[[event touchesForView:self] mutableCopy] autorelease];
    [remainingTouches minusSet:touches];
	if ([remainingTouches count] < 2)
	{
		twoFingersAreMoving = NO;
		pinchGestureUnderway = NO;
		previousDirectionOfPanning = CGPointZero;
		
		lastMovementPosition = [[remainingTouches anyObject] locationInView:self];
	}	
  
  if ([remainingTouches count] == 0 && lastRotationMotionNorm > 0.0f)
  {
    inertialRotationThread = [[NSThread alloc] initWithTarget:self selector:@selector(handleInertialRotation) object:nil];
    [inertialRotationThread start];
  }
}

-(int) getNumberOfFacetsForCurrentModel
{
  if (renderer)
  {
    return [renderer getNumberOfFacetsForCurrentModel];
  }
  return 0;
}

-(int) getNumberOfLinesForCurrentModel
{
  if (renderer)
  {
    return [renderer getNumberOfLinesForCurrentModel];
  }
  return 0;
}


-(int) getNumberOfVerticesForCurrentModel
{
  if (renderer)
  {
    return [renderer getNumberOfVerticesForCurrentModel];
  }
  return 0;
}



@end
