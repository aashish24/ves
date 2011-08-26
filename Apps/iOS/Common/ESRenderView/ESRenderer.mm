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

#import "ESRenderer.h"

#include "vesRenderer.h"
#include "vesShaderProgram.h"
#include "vesShader.h"
#include "vesCamera.h"
#include "vesTexture.h"

@implementation ESRenderer

// Create an OpenGL ES 2.0 context
- (id)init
{
  self = [super init];
  if (self)
  {		
		// Create a C++ renderer object
    renderer = new vesRenderer();
    
    NSString *vertShaderPathname, *fragShaderPathname;
    GLchar* vertexSourceStr, *fragmentSourceStr;
    
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];
    
    vertexSourceStr = (GLchar *)[[NSString stringWithContentsOfFile:vertShaderPathname 
                                                           encoding:NSUTF8StringEncoding 
                                                              error:nil] UTF8String];
    fragmentSourceStr = (GLchar *)[[NSString stringWithContentsOfFile:fragShaderPathname 
                                                             encoding:NSUTF8StringEncoding 
                                                                error:nil] UTF8String];
    
    shaderProgram = new vesShaderProgram(vertexSourceStr,
                                   fragmentSourceStr,
                                   (_uni("u_mvpMatrix"),
                                    _uni("u_normalMatrix"),
                                    _uni("u_ecLightDir"),
                                    _uni("u_opacity")),
                                   (_att("a_vertex"),
                                    _att("a_normal"),
                                    _att("a_vertex_color"))
                                   );
    self->Shader = new vesShader(shaderProgram);
    
    // Creating background shader
    NSString *vertShaderPathname1, *fragShaderPathname1;
    GLchar* vertexSourceStr1, *fragmentSourceStr1;
    
    vertShaderPathname1 = [[NSBundle mainBundle] pathForResource:@"BackgroundTexture" ofType:@"vsh"];
    fragShaderPathname1 = [[NSBundle mainBundle] pathForResource:@"BackgroundTexture" ofType:@"fsh"];
    
    vertexSourceStr1 = (GLchar *)[[NSString stringWithContentsOfFile:vertShaderPathname1 
                                                           encoding:NSUTF8StringEncoding 
                                                              error:nil] UTF8String];
    fragmentSourceStr1 = (GLchar *)[[NSString stringWithContentsOfFile:fragShaderPathname1 
                                                             encoding:NSUTF8StringEncoding 
                                                                error:nil] UTF8String];
    
		backgroundShaderProgram = new vesShaderProgram(vertexSourceStr1,
                                         fragmentSourceStr1,
                                         (_uni("u_ortho"),
                                          _uni("u_null1")),
                                         (_att("a_position"),
                                          _att("a_texCoord"))
                                         );
    
    // Getting image
    SFImage image;
    NSString *imagePathname;
    imagePathname = [[NSBundle mainBundle] pathForResource:@"Kitware" ofType:@"png"];
    UIImage* uiImage = [UIImage imageWithContentsOfFile:imagePathname];
    CGImageRef cgImage = uiImage.CGImage;
    image.width = CGImageGetWidth(cgImage);
    image.height = CGImageGetHeight(cgImage);
    image.data = (void*) CFDataGetBytePtr(CGDataProviderCopyData(CGImageGetDataProvider(cgImage)));
    
    // Create background texture
    self->backgroundTexture = new vesTexture(backgroundShaderProgram,image);
    renderer->SetBackground(self->backgroundTexture);
    }
  
  return self;
}

- (vesRenderer*) getRenderer
{
  return self->renderer;
}

- (vesShader*) getShader
{
  return self->Shader;
}

- (vesCamera*) getCamera
{
  return renderer->GetCamera();
}

- (void) render
{
  glClearColor(63/255.0f, 96/255.0f, 144/255.0, 1.0f);
  // Use shader program
  //shaderProgram->Use();
  renderer->ResetCameraClippingRange();
  renderer->Render();
}

- (BOOL) resizeFromLayer:(int) w height:(int) h
{
  renderer->Resize(w, h, 1.0f);
  return YES;
}

- (void)dealloc
{
  shaderProgram->Delete();
  delete renderer;
	renderer = 0;
	
  [super dealloc];
}

- (void)resetView
{
    renderer->ResetCamera();
}

@end
