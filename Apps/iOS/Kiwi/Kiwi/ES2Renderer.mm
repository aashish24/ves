/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    ES2Renderer.mm
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#import "ES2Renderer.h"
#import "DataReader.h"

#include "vesRenderer.h"
#include "vesMultitouchCamera.h"
#include "vesShader.h"
#include "vesShaderProgram.h"
#include "vesTriangleData.h"
#include "vesMapper.h"
#include "vesActor.h"

@implementation ES2Renderer

// Create an OpenGL ES 2.0 context
- (id)init
{
  self = [super init];
  if (self)
  {		
  
  		// Create a C++ renderer object
		mCamera = new vesMultitouchCamera;
    renderer = new vesRenderer(mCamera);
    
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
    
    NSString* defaultFile = [[NSBundle mainBundle] pathForResource:@"current" ofType:@"vtk"];
    vesTriangleData* triangleData = [[[DataReader new] autorelease] readData:defaultFile];
    mMapper = new vesMapper();
    mMapper->SetTriangleData(triangleData);
    mActor = new vesActor(self->Shader, mMapper);
    mActor->SetColor(0.8, 0.8, 0.8, 1.0);
    renderer->AddActor(mActor);
    }
  
  return self;
}

- (void) render
{
  glClearColor(63/255.0f, 96/255.0f, 144/255.0, 1.0f);
  // Use shader program
  shaderProgram->Use();
  renderer->Render();
}

- (BOOL) resizeFromLayer:(int) w height:(int) h
{
  renderer->resize(w,h,1.0);
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
    renderer->resetView();
}

- (void) setFilePath :(NSString *) fpath
{
  UIAlertView *readerAlert = [[[UIAlertView alloc]
    initWithTitle:@"Reading File\nPlease Wait..." message:nil
    delegate:self cancelButtonTitle:nil otherButtonTitles: nil] autorelease];
  [readerAlert show];
  UIActivityIndicatorView *indicator = [[UIActivityIndicatorView alloc]
    initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
  // Adjust the indicator so it is up a few pixels from the bottom of the alert
  indicator.center = CGPointMake(readerAlert.bounds.size.width / 2, readerAlert.bounds.size.height - 50);
  [indicator startAnimating];
  [readerAlert addSubview:indicator];
  [indicator release];

  // 250ms pause for the dialog to become active...
  int n=0;
  while(n < 250)
    {
    [[NSRunLoop currentRunLoop] limitDateForMode:NSDefaultRunLoopMode];
    [NSThread sleepForTimeInterval:.001];
    n++;
    }

  DataReader* reader = [[DataReader new] autorelease];
  vesTriangleData* newData = [reader readData:fpath];
  if (!newData)
    {
    [readerAlert dismissWithClickedButtonIndex:0 animated:YES];
    UIAlertView *alert = [[UIAlertView alloc]
      initWithTitle:reader.errorTitle
      message:reader.errorMessage
      delegate:self
      cancelButtonTitle:NSLocalizedStringFromTable(@"OK", @"Localized", nil)
      otherButtonTitles: nil, nil];
    [alert show];
    [alert release];
    return;
    }

  mMapper->SetTriangleData(newData);
  mActor->Read();
  [readerAlert dismissWithClickedButtonIndex:0 animated:YES];
}

- (void)_drawViewByRotatingAroundX:(float)xRotation 
                   rotatingAroundY:(float)yRotation 
                           scaling:(float)scaleFactor 
                    translationInX:(float)xTranslation 
                    translationInY:(float)yTranslation
{
#if GMTL_CAMERA
  mCamera->UpdateMatrixGMTL(xRotation,yRotation,scaleFactor,xTranslation,yTranslation);
#else
  mCamera->UpdateMatrix(xRotation,yRotation,scaleFactor,xTranslation,yTranslation); 
#endif
}

-(int) getNumberOfFacetsForCurrentModel
{
  return mMapper->GetTriangleData()->GetTriangles().size();
}

-(int) getNumberOfLinesForCurrentModel
{
  return mMapper->GetTriangleData()->GetLines().size();
}

-(int) getNumberOfVerticesForCurrentModel
{
  return mMapper->GetTriangleData()->GetPoints().size();
}
@end
