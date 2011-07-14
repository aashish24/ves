/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    ESRenderView.h
 
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


@class ESRenderer;


@interface ESRenderView : UIView
{    
@private
  GLint backingWidth;
  GLint backingHeight;
  GLuint viewRenderbuffer;
  GLuint viewFramebuffer;
  GLuint msaaFramebuffer;
  GLuint msaaRenderbuffer;
  GLuint depthRenderbuffer;
  
  EAGLContext* context;
  ESRenderer* renderer;
}

@property (readonly, nonatomic) ESRenderer* renderer;

- (void)drawView:(id)sender;
- (void)resetView;


@end
