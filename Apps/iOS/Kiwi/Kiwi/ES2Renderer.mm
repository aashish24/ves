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

#import "ES2Renderer.h"
#import "DataReader.h"

#include "vesKiwiViewerApp.h"
#include "vesRenderer.h"
#include "vesCamera.h"
#include "vesTriangleData.h"
#include "vesMapper.h"
#include "vesActor.h"

@implementation ES2Renderer

@synthesize app = mApp;

// Create an OpenGL ES 2.0 context
- (id)init
{
  self = [super init];
  if (self)
  {
    self->mApp = new vesKiwiViewerApp;

    NSString* vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
    NSString* fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];

    NSString* vertexSourceStr = [NSString stringWithContentsOfFile:vertShaderPathname
                                                           encoding:NSUTF8StringEncoding
                                                              error:nil];
    NSString* fragmentSourceStr = [NSString stringWithContentsOfFile:fragShaderPathname
                                                             encoding:NSUTF8StringEncoding
                                                                error:nil];


    self->mApp->setVertexShaderSource([vertexSourceStr UTF8String]);
    self->mApp->setFragmentShaderSource([fragmentSourceStr UTF8String]);
    self->mApp->initializeShaderProgram();
    self->mApp->initializeRendering();

    renderer = self->mApp->renderer();
    mMapper = self->mApp->mapper();
    mActor = self->mApp->actor();

    NSString* defaultFile = [[NSBundle mainBundle] pathForResource:@"current" ofType:@"vtk"];
    vesTriangleData* triangleData = [[[DataReader new] autorelease] readData:defaultFile];
    mMapper->setTriangleData(triangleData);
    }

  return self;
}

- (vesKiwiViewerApp*) getApp
{
  return self->mApp;
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
  self->mApp->render();
}

- (BOOL) resizeFromLayer:(int) w height:(int) h
{
  self->mApp->resizeView(w, h);
  return YES;
}

- (void)dealloc
{
  delete self->mApp;
  [super dealloc];
}

- (void)resetView
{
  self->mApp->resetView();
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

  if (mMapper->triangleData())
  {
    delete mMapper->triangleData();
  }

  mMapper->setTriangleData(newData);
  mActor->read();
  [readerAlert dismissWithClickedButtonIndex:0 animated:YES];
}

-(int) getNumberOfFacetsForCurrentModel
{
  return mMapper->triangleData()->GetTriangles().size();
}

-(int) getNumberOfLinesForCurrentModel
{
  return mMapper->triangleData()->GetLines().size();
}

-(int) getNumberOfVerticesForCurrentModel
{
  return mMapper->triangleData()->GetPoints().size();
}
@end
