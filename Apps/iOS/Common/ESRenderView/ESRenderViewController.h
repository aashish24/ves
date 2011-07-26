/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    ESRenderViewController.h
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#import <UIKit/UIKit.h>

@class ESRenderView;

@interface ESRenderViewController : UIViewController {
@private
    
  ESRenderView* renderView;
  
  // Touch-handling 
	float startingTouchDistance;
  float previousScale;
	float instantObjectScale; 
  float instantXRotation;
  float instantYRotation;
  float instantXTranslation;
  float instantYTranslation;
  float instantZTranslation;
	CGPoint lastMovementPosition;
  CGPoint lastMovementXYUnitDelta;
  float lastRotationMotionNorm;
  CGPoint previousDirectionOfPanning;
	BOOL twoFingersAreMoving;
  BOOL pinchGestureUnderway;
	float scalingForMovement;
  
  NSThread* inertialRotationThread;
}


@property (nonatomic, retain) ESRenderView* renderView;
@property (nonatomic) BOOL cameraInteractionEnabled;


- (void) render;
- (void) forceRender;

// Touch handling
- (float)distanceBetweenTouches:(NSSet *)touches;
- (CGPoint)commonDirectionOfTouches:(NSSet *)touches;
- (void)handleTouchesEnding:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)rotate: (CGPoint)delta;

// inertia handling
- (void)handleInertialRotation;
- (void)stopInertialMotion;

@end
