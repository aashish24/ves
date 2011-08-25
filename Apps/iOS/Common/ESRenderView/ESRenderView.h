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
