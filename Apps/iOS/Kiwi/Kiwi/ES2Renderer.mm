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

#include "vesKiwiViewerApp.h"

@implementation ES2Renderer

@synthesize app = mApp;


-(NSString*) stringFromFileContents:(NSString*)filename
{
    NSString* absoluteFilename = [[NSBundle mainBundle] pathForResource:filename ofType:nil];
    return [NSString stringWithContentsOfFile:absoluteFilename
              encoding:NSUTF8StringEncoding error:nil];
}

- (id)init
{
  self = [super init];
  if (self)
  {
    self->mApp = new vesKiwiViewerApp;
  }

  return self;
}

- (vesKiwiViewerApp*) getApp
{
  return self->mApp;
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

  bool loadSuccess = self->mApp->loadDataset([fpath UTF8String]);
  if (!loadSuccess)
  {
    [readerAlert dismissWithClickedButtonIndex:0 animated:YES];
    UIAlertView *alert = [[UIAlertView alloc]
      initWithTitle:[NSString stringWithUTF8String:self->mApp->loadDatasetErrorTitle().c_str()]
      message:[NSString stringWithUTF8String:self->mApp->loadDatasetErrorMessage().c_str()]
      delegate:self
      cancelButtonTitle:NSLocalizedStringFromTable(@"OK", @"Localized", nil)
      otherButtonTitles: nil, nil];
    [alert show];
    [alert release];
    return;
    }

  [readerAlert dismissWithClickedButtonIndex:0 animated:YES];
}

-(int) getNumberOfFacetsForCurrentModel
{
  return mApp->numberOfModelFacets();
}

-(int) getNumberOfLinesForCurrentModel
{
  return mApp->numberOfModelLines();
}

-(int) getNumberOfVerticesForCurrentModel
{
  return mApp->numberOfModelVertices();
}
@end
