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

#import "ESRenderView.h"
#import "ESRenderer.h"

#define USE_DEPTH_BUFFER 1

@interface ESRenderView ()
@property (nonatomic, retain) EAGLContext *context;
- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;
@end


@implementation ESRenderView

@synthesize context;
@synthesize renderer;


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
    

    renderer = [[ESRenderer alloc] init];
    
    if (!renderer)
    {
			[self release];
			return nil;
    }
    
  self.multipleTouchEnabled = YES;
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
  // CF: https://devforums.apple.com/thread/45850
  
  // reqhested samples should be > 1; 4 is used in the apple example code
  int requestedSamples = 4;
    
  [EAGLContext setCurrentContext:context];
  
  // create the view frame buffer
  glGenFramebuffers(1, &viewFramebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, viewFramebuffer);
  
  // create renderbuffer attched to core animation and ask for dimensions
  glGenRenderbuffers(1, &viewRenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
  [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, viewRenderbuffer);
  
  // determine how many samples to use
  GLint maxSamplesAllowed;
  glGetIntegerv(GL_MAX_SAMPLES_APPLE, &maxSamplesAllowed);
  int samplesToUse = (requestedSamples > maxSamplesAllowed) ? maxSamplesAllowed : requestedSamples;
  
  // create msaa (offscreen) framebuffer
  glGenFramebuffers(1, &msaaFramebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, msaaFramebuffer);
  
  // create msaa (offscrean) color buffer
  glGenRenderbuffers(1, &msaaRenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, msaaRenderbuffer);
  glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, samplesToUse, GL_RGBA8_OES, backingWidth, backingHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msaaRenderbuffer);
  
  if (USE_DEPTH_BUFFER) 
  {
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, samplesToUse, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
  }
  
  // validate msaa framebuffer
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
  {
    NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    return NO;
  }

  // validate the viewFramebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, viewFramebuffer);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
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
  
  glDeleteFramebuffers(1, &msaaFramebuffer);
  msaaFramebuffer = 0;
  glDeleteRenderbuffers(1, &msaaRenderbuffer);
  
  if(depthRenderbuffer) {
    glDeleteRenderbuffers(1, &depthRenderbuffer);
    depthRenderbuffer = 0;
  }
}


- (void)drawView:(id) sender {  
  [EAGLContext setCurrentContext:context];
  
  glBindFramebuffer(GL_FRAMEBUFFER, msaaFramebuffer);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  glViewport(0, 0, backingWidth, backingHeight);
  [renderer render];

  glDisable(GL_SCISSOR_TEST);     
  glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, msaaFramebuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, viewFramebuffer);
  glResolveMultisampleFramebufferAPPLE();
  
  GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT };
  glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 3, attachments);
  
  glBindRenderbuffer(GL_RENDERBUFFER, viewRenderbuffer);
  [context presentRenderbuffer:GL_RENDERBUFFER];
}


- (void)resetView
{
  [renderer resetView];
  [self drawView:nil];
}


- (void)dealloc
{
  if ([EAGLContext currentContext] == context) {
    [EAGLContext setCurrentContext:nil];
	}

  [renderer release];  
  [context release];
  context = nil;
  [super dealloc];
}

@end
