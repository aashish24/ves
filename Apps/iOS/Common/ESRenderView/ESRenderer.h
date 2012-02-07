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



#import <QuartzCore/QuartzCore.h>

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>


#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

struct vesRenderer;
struct vesShaderProgram;
struct vesFileReader;
struct vesMapper;
struct vesActor;
struct vesCamera;
struct vesShader;
struct vesTexture;


@interface ESRenderer : NSObject
{
@private
  //GLuint program;
  NSString *filePath;
  vesShader *Shader;
  vesShaderProgram *shaderProgram;
  vesShaderProgram *backgroundShaderProgram;
  vesTexture *backgroundTexture;
	vesRenderer *renderer;
}

- (vesRenderer*) getRenderer;
- (vesShader*) getShader;
- (vesCamera*) getCamera;
- (void)render;
- (void)resetView;
- (BOOL)resizeFromLayer:(int)w height:(int) h;

@end

