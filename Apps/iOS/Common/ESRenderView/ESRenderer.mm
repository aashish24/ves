/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    ESRenderer.mm
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

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
                                    _uni("u_ecLightDir")),
                                   (_att("a_vertex"),
                                    _att("a_normal"),
                                    _att("a_texcoord"))
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
