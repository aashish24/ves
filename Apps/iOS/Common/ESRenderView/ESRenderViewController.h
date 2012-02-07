/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.
  Copyright 2012 Willow Garage, Inc.

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
