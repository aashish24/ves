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
#import "MyGLKViewController.h"
#import "MyGestureHandler.h"

#include <vesKiwiViewerApp.h>


@interface MyGLKViewController () {

  vesKiwiViewerApp::Ptr mKiwiApp;

  MyGestureHandler* mGestureHandler;
}

@property (strong, nonatomic) EAGLContext *context;

- (void)tearDownGL;

@end

@implementation MyGLKViewController


- (void)viewDidLoad
{
  [super viewDidLoad];

  self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

  if (!self.context) {
      NSLog(@"Failed to create ES context");
  }

  GLKView *view = (GLKView *)self.view;
  view.context = self.context;
  view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
  view.drawableMultisample = GLKViewDrawableMultisample4X;

  [self initializeKiwiApp];
  [self initializeGestureHandler];
}

-(void) initializeKiwiApp
{
  [EAGLContext setCurrentContext:self.context];

  std::string dataset = [[[NSBundle mainBundle] pathForResource:@"teapot" ofType:@"vtp"] UTF8String];

  self->mKiwiApp = vesKiwiViewerApp::Ptr(new vesKiwiViewerApp);
  self->mKiwiApp->initGL();
  [self resizeView];
  self->mKiwiApp->loadDataset(dataset);
  self->mKiwiApp->resetView();
}

-(void) initializeGestureHandler
{
  self->mGestureHandler = [[MyGestureHandler alloc] init];
  self->mGestureHandler.view = self.view;
  self->mGestureHandler.kiwiApp = self->mKiwiApp;
  [self->mGestureHandler createGestureRecognizers];
}

- (void)dealloc
{
  [self tearDownGL];

  if ([EAGLContext currentContext] == self.context) {
      [EAGLContext setCurrentContext:nil];
  }
}

- (void)didReceiveMemoryWarning
{
  [super didReceiveMemoryWarning];

  if ([self isViewLoaded] && ([[self view] window] == nil)) {
    self.view = nil;

    [self tearDownGL];

    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    self.context = nil;
  }

  // Dispose of any resources that can be recreated.
}

- (void)tearDownGL
{
  [EAGLContext setCurrentContext:self.context];

  // free GL resources
  // ...
}

-(void) resizeView
{
  double scale = self.view.contentScaleFactor;
  self->mKiwiApp->resizeView(self.view.bounds.size.width*scale, self.view.bounds.size.height*scale);
}

- (void)viewWillLayoutSubviews
{
  [self resizeView];
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
  if (self->mKiwiApp && !self.paused) {
    self->mKiwiApp->render();
  }
}

@end
