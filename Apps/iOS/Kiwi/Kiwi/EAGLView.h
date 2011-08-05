/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    EAGLView.h
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import "ESRenderer.h"
#import "kiwiAppDelegate.h"

// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface EAGLView : UIView
{    
@private
  EAGLContext *context;	
  // The pixel dimensions of the CAEAGLLayer
  GLint backingWidth;
  GLint backingHeight;
  
  /* OpenGL names for the renderbuffer, framebuffers used to render to this view */
  GLuint viewRenderbuffer;
  GLuint depthRenderbuffer;
  GLuint viewFramebuffer;

  id <ESRenderer> renderer;
  NSString *filePath;
	
	// inertia handling 
  CGPoint lastMovementXYUnitDelta;
  float lastRotationMotionNorm;  
  NSThread* inertialRotationThread;
}

- (void)drawView:(id)sender;
- (void)resetView;
- (void)setFilePath:(NSString*)fpath;

// Touch handling
- (void) createGestureRecognizers;
- (IBAction)handleSingleFingerPanGesture:(UIPanGestureRecognizer *)sender;
- (IBAction)handleDoubleFingerPanGesture:(UIPanGestureRecognizer *)sender;
- (IBAction)handlePinchGesture:(UIPinchGestureRecognizer *)sender;
- (IBAction)handle2DRotationGesture:(UIRotationGestureRecognizer *)sender;
- (IBAction)handleTapGesture:(UITapGestureRecognizer *)sender;

- (void)rotate: (CGPoint)delta;

// inertia handling
- (void)handleInertialRotation;
- (void)stopInertialMotion;

// model information
- (int)getNumberOfFacetsForCurrentModel;
- (int)getNumberOfLinesForCurrentModel;
- (int)getNumberOfVerticesForCurrentModel;

@end
