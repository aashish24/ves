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
#import "PVRemoteViewController.h"
#import "PVWebViewController.h"
#import "PointCloudStreamingViewController.h"
#import "SceneSettingsController.h"
#import "MBProgressHUD.h"

#include "kiwiCloudApp.h"
#include "vesKiwiCameraSpinner.h"

#include <vesImage.h>
#include <vesTexture.h>
#include <vesKiwiDataConversionTools.h>
#include <vtkSmartPointer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkMatrix4x4.h>
#include <vtkImageReslice.h>
#include <vtkTransform.h>

@interface MyGLKViewController () <UINavigationControllerDelegate, UIImagePickerControllerDelegate>
{

  bool shouldClearApp;
  bool handleArgsEnabled;
  kiwiApp::Ptr mKiwiApp;

  MyGestureHandler* mGestureHandler;

  UIPopoverController* mediaPopover;

  __weak UIPopoverController *mPopover;
  __weak SceneSettingsController* mSceneSettings;

}

@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation MyGLKViewController

@synthesize settingsButton;
@synthesize zoomToFitButton;
@synthesize toolbar;
@synthesize leftLabel;
@synthesize rightLabel;

- (void)viewDidLoad
{
  [super viewDidLoad];

  self->shouldClearApp = false;
  self->handleArgsEnabled = true;
  self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

  if (!self.context) {
      NSLog(@"Failed to create ES context");
  }

  GLKView *view = (GLKView *)self.view;
  view.context = self.context;
  view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
  [self onMultisamplingChanged];

  [self createDefaultApp];
  [self initializeGestureHandler];

  [self populateToolbar];

  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onMultisamplingChanged)
                                        name:@"EnableMultisamplingChanged" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onClearScene)
                                        name:@"ClearScene" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onToggleFullScreen)
                                        name:@"TripleTapGesture" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onCameraSpinDecelerationChanged)
                                        name:@"CameraSpinDecelerationChanged" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onCameraProjectionModeChanged)
                                        name:@"CameraProjectionModeChanged" object:nil];
}

-(vesSharedPtr<kiwiApp>) currentApp
{
  return self->mKiwiApp;
}

-(void) clearToolbar
{
  NSMutableArray *items = [NSMutableArray new];
  [self.toolbar setItems:[items copy] animated:NO];
}

- (void) populateToolbar
{
  std::vector<std::string> actions;
  if (self->mKiwiApp) {
    actions = self->mKiwiApp->actions();
  }

  NSMutableArray *items = [NSMutableArray arrayWithCapacity:actions.size()];

  for (size_t i = 0; i < actions.size(); ++i) {

    UIBarButtonItem * actionButton;
    if (true && actions[i] == "Reset Camera") {
      actionButton = self.zoomToFitButton;
    }
    else {

      actionButton = [[UIBarButtonItem alloc]
        initWithTitle:[NSString stringWithUTF8String:actions[i].c_str()]
        style:UIBarButtonItemStyleBordered
        target:self
        action:@selector(onAction:)];


    [actionButton setTitleTextAttributes:[NSDictionary dictionaryWithObjectsAndKeys:
      [UIColor lightGrayColor], UITextAttributeTextColor,
    //  [UIFont fontWithName:@"Helvetica-Bold" size:14.0], UITextAttributeFont,
      nil] forState:UIControlStateNormal];
    //[actionButton setTintColor:self.zoomToFitButton.tintColor];

    }


    actionButton.target = self;
    actionButton.action = @selector(onAction:);
    [items addObject:actionButton];
  }


  [items addObject:[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace
                                            target:nil action:nil]];
  [items addObject:settingsButton];
  [self.toolbar setItems:[items copy] animated:NO];
}

-(void) onAction:(UIBarButtonItem*)button
{
  //int buttonIndex = [self.toolbar.items indexOfObject:button];

  std::string action;
  if ([button.title length]) {
    action = [button.title UTF8String];
  }
  else if (button == self.zoomToFitButton) {
    action = "Reset Camera";
  }

  if (self->mKiwiApp) {
    self->mKiwiApp->onAction(action);
    [self populateToolbar];
  }

  if (action == "3D") {
    [SceneSettingsController setBackgroundColor:self->mKiwiApp];
  }
}

-(void) onMultisamplingChanged
{
  GLKView *view = (GLKView *)self.view;
  if ([[NSUserDefaults standardUserDefaults] boolForKey:@"EnableMultisampling"]) {
    view.drawableMultisample = GLKViewDrawableMultisample4X;
  }
  else {
    view.drawableMultisample = GLKViewDrawableMultisampleNone;
  }
}

-(void) onCameraProjectionModeChanged
{
  if (self->mKiwiApp) {
    int projectionMode = [[NSUserDefaults standardUserDefaults] integerForKey:@"CameraProjectionMode"];
    if (projectionMode == 0) {
      self->mKiwiApp->setCameraProjectionToPerspective();
    }
    else {
      self->mKiwiApp->setCameraProjectionToParallel();
    }
  }
}

-(void) onCameraSpinDecelerationChanged
{
  if (self->mKiwiApp) {
    double cameraSpinDeceleration = [[NSUserDefaults standardUserDefaults] doubleForKey:@"CameraSpinDeceleration"];
    self->mKiwiApp->cameraSpinner()->setDeceleration(cameraSpinDeceleration);
  }
}

-(void) updateSceneStatistics
{
  [[NSNotificationCenter defaultCenter] postNotificationName:@"UpdateSceneStatistics" object:nil];
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {

  if (UI_USER_INTERFACE_IDIOM() != UIUserInterfaceIdiomPhone) {
    mPopover = [(UIStoryboardPopoverSegue *)segue popoverController];
    mPopover.delegate = self;
  }

  UINavigationController* navController = (UINavigationController*)[segue destinationViewController];
  mSceneSettings = (SceneSettingsController*)navController.topViewController;
  mSceneSettings.app = self->mKiwiApp;
}

- (BOOL)popoverControllerShouldDismissPopover:(UIPopoverController *)popoverController
{
  [mSceneSettings clearTimer];
  return YES;
}

- (BOOL)shouldPerformSegueWithIdentifier:(NSString *)identifier sender:(id)sender {

  if (mPopover) {
    [mSceneSettings clearTimer];
    [mPopover dismissPopoverAnimated:YES];
    return NO;
  }
  else {
    return YES;
  }
}

- (void)handleTapGesture:(UITapGestureRecognizer *)sender
{
  [mSceneSettings clearTimer];
  [mPopover dismissPopoverAnimated:YES];
}

- (void)showAlertDialogWithTitle:(NSString *)alertTitle message:(NSString *)alertMessage;
{
  UIAlertView *alert = [[UIAlertView alloc]
                        initWithTitle:alertTitle
                        message:alertMessage
                        delegate:nil
                        cancelButtonTitle:@"Ok"
                        otherButtonTitles: nil, nil];
  [alert show];
}

-(void) showErrorDialog
{
  NSString* errorTitle = [NSString stringWithUTF8String:mKiwiApp->loadDatasetErrorTitle().c_str()];
  NSString* errorMessage = [NSString stringWithUTF8String:mKiwiApp->loadDatasetErrorMessage().c_str()];
  [self showAlertDialogWithTitle:errorTitle message:errorMessage];
}

-(void) deleteApp
{
  self->mGestureHandler.kiwiApp = kiwiApp::Ptr();
  if (self->mSceneSettings) {
    self->mSceneSettings.app = kiwiApp::Ptr();
  }
  self->mKiwiApp.reset();
  self->shouldClearApp = false;
  [self clearToolbar];
}

-(void) setApp:(kiwiApp::Ptr) app
{
  self->mGestureHandler.kiwiApp = app;
  if (self->mSceneSettings) {
    self->mSceneSettings.app = app;
  }
  self->mKiwiApp = app;
  [self populateToolbar];
}

-(void) createDefaultApp
{
  [self deleteApp];
  kiwiCloudApp::Ptr app = kiwiCloudApp::Ptr(new kiwiCloudApp);
  [self setApp:app];
  [self setupGL];
}

-(void) onClearScene
{
  [self createDefaultApp];
  [self populateToolbar];
  [self updateSceneStatistics];
}

-(void) onToggleFullScreen
{
  self.toolbar.hidden = !self.toolbar.hidden;
  if (self.toolbar.hidden) {
    MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:self.view animated:YES withDelay:0.0];
    hud.mode = MBProgressHUDModeText;
    hud.labelText = @"Triple tap to exit fullscren.";
    dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, 2.0 * NSEC_PER_SEC);
    dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
      [MBProgressHUD hideHUDForView:self.view animated:YES];
    });
  }

}

-(void)doPVRemote:(NSDictionary*) args
{
  self.paused = YES;
  [self createDefaultApp];
  self->shouldClearApp = true;


  PVRemoteViewController* pvRemoteView = (PVRemoteViewController*)[args objectForKey:@"viewController"];
  NSString* hostText = [[NSUserDefaults standardUserDefaults] stringForKey:@"PVRemoteHost"];
  std::string host = [hostText UTF8String];
  int port = 40000;

  size_t splitPos = host.find(':');
  if (splitPos != std::string::npos) {
    std::string left = host.substr(0, splitPos);
    std::string right = host.substr(splitPos + 1, host.length()-splitPos+1);
    host = left;
    port = atoi(right.c_str());
  }

  MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:pvRemoteView.view animated:YES withDelay:0.5];
  hud.labelText = @"Please Wait...";
  dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{


    bool result = self->mKiwiApp->doPVRemote(host, port);

    dispatch_async(dispatch_get_main_queue(), ^{
      [MBProgressHUD hideHUDForView:pvRemoteView.view animated:YES];

      if (!result) {
        [self showErrorDialog];
      }
      else {
        [pvRemoteView dismissViewControllerAnimated:YES completion:nil];
      }

      [self populateToolbar];
      self.paused = NO;

    });
  });
}

-(void)doPVWeb:(NSDictionary*) args
{
  self.paused = YES;
  [self createDefaultApp];
  self->shouldClearApp = true;


  PVWebViewController* pvWebView = (PVWebViewController*)[args objectForKey:@"viewController"];
  
  NSString* session = [[NSUserDefaults standardUserDefaults] stringForKey:@"PVWebSession"];
  NSString* host = [[NSUserDefaults standardUserDefaults] stringForKey:@"PVWebHost"];


  MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:pvWebView.view animated:YES withDelay:0.5];
  hud.labelText = @"Please Wait...";
  dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{


    bool result =   self->mKiwiApp->doPVWebTest([host UTF8String], [session UTF8String]);

    dispatch_async(dispatch_get_main_queue(), ^{
      [MBProgressHUD hideHUDForView:pvWebView.view animated:YES];

      if (!result) {
        [self showErrorDialog];
      }
      else {
        [pvWebView dismissViewControllerAnimated:YES completion:nil];
      }

      [self populateToolbar];
      self.paused = NO;

    });
  });
}

-(void) doPointCloudStreaming:(NSDictionary*) args
{

  PointCloudStreamingViewController* streamingView = (PointCloudStreamingViewController*)[args objectForKey:@"viewController"];
  NSString* hostText = [[NSUserDefaults standardUserDefaults] stringForKey:@"PointCloudStreamingHost"];
  std::string host = [hostText UTF8String];
  int port = 11111;

  size_t splitPos = host.find(':');
  if (splitPos != std::string::npos) {
    std::string left = host.substr(0, splitPos);
    std::string right = host.substr(splitPos + 1, host.length()-splitPos+1);
    host = left;
    port = atoi(right.c_str());
  }

  self.paused = YES;
  [self deleteApp];

  vesKiwiPointCloudApp::Ptr streamingApp = vesKiwiPointCloudApp::Ptr(new vesKiwiPointCloudApp);
  [self setApp:streamingApp];
  self->shouldClearApp = true;
  [self setupGL];


  MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:streamingView.view animated:YES withDelay:0.5];
  hud.labelText = @"Please Wait...";
  dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{


    bool result = streamingApp->connect(host, port);

    dispatch_async(dispatch_get_main_queue(), ^{
      [MBProgressHUD hideHUDForView:streamingView.view animated:YES];

      if (!result) {
        [self showErrorDialog];
        [self createDefaultApp];
      }
      else {
        [streamingView dismissViewControllerAnimated:YES completion:nil];
      }

      [self populateToolbar];
      self.paused = NO;

    });
  });


}

-(void)handleArgsEnabled
{
  self->handleArgsEnabled = YES;
}

-(void)handleArgsDisabled
{
  self->handleArgsEnabled = NO;
}

-(void) loadDataset:(NSString*) filename
{
  self.paused = YES;

  if ([[NSUserDefaults standardUserDefaults] boolForKey:@"ClearSceneAutomatically"]
       || self->shouldClearApp) {
    [self createDefaultApp];
  }

  // Switch out of 2D image viewer mode, if needed
  vesSharedPtr<kiwiCloudApp> defaultApp = vesDynamicPtrCast<kiwiCloudApp>(self->mKiwiApp);
  if (defaultApp && defaultApp->is2DImageMode()) {
    defaultApp->onCamera3D();
  }


  MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:self.view animated:YES withDelay:0.5];
  hud.labelText = @"Please Wait...";
  [self handleArgsDisabled];
  dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{

    //for (int i = 0; i < 5; ++i) {
    //  NSLog(@"sleep...");
    //  [NSThread sleepForTimeInterval:1.0];
    //}

    bool result = self->mKiwiApp->loadDataset([filename UTF8String]);

    dispatch_async(dispatch_get_main_queue(), ^{
      [MBProgressHUD hideHUDForView:self.view animated:YES];

      if (!result) {
        [self showErrorDialog];
      }

      if (defaultApp && defaultApp->imageRep()) {
        defaultApp->onAction("Z");
      }
      else {
        self->mKiwiApp->resetView();
      }

      [self populateToolbar];
      self.paused = NO;
      [self handleArgsEnabled];


    });
  });

}


-(void) handleArgs:(NSDictionary*) args
{
  if (!self->handleArgsEnabled) {
    return;
  }

  NSString* filename = [args objectForKey:@"FileName"];
  NSString* appName = [args objectForKey:@"AppName"];

  if (filename) {
    [self loadDataset:filename];
  }
  else if (appName) {
    if ([appName isEqualToString:@"ParaView Web"]) {
      [self doPVWeb:args];
    }
    else if ([appName isEqualToString:@"Point Cloud Streaming"]) {
      [self doPointCloudStreaming:args];
    }
    else if ([appName isEqualToString:@"ParaView Mobile Remote"]) {
      [self doPVRemote:args];
    }
  }

}

-(void) initializeGestureHandler
{
  self->mGestureHandler = [[MyGestureHandler alloc] init];
  self->mGestureHandler.view = self.view;
  self->mGestureHandler.kiwiApp = self->mKiwiApp;
  [self->mGestureHandler createGestureRecognizers];
  self->mGestureHandler.receiver = self;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
  [super touchesBegan:touches withEvent:event];
  mKiwiApp->cameraSpinner()->disable();
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

-(void) resizeView
{
  double scale = self.view.contentScaleFactor;
  self->mKiwiApp->resizeView(self.view.bounds.size.width*scale, self.view.bounds.size.height*scale);
}

- (void)setupGL
{
  [EAGLContext setCurrentContext:self.context];  
  self->mKiwiApp->initGL();
  [self resizeView];
  [SceneSettingsController setBackgroundColor:self->mKiwiApp];

  [self onCameraSpinDecelerationChanged];
  [self onCameraProjectionModeChanged];
}

- (void)tearDownGL
{
  [EAGLContext setCurrentContext:self.context];

  // free GL resources
  // ...
}


- (void)viewWillLayoutSubviews
{
  [self resizeView];
}

#pragma mark - GLKView and GLKViewController delegate methods


- (void)update
{
  if (self->mKiwiApp) {
    std::string leftText = self->mKiwiApp->leftText();
    std::string rightText = self->mKiwiApp->rightText();
    self.leftLabel.text = [NSString stringWithUTF8String:leftText.c_str()];
    self.rightLabel.text = [NSString stringWithUTF8String:rightText.c_str()];
  }
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
  /*
  if (!self->mKiwiApp) {
    NSLog(@"Warning: draw called with nil app");
  }
  if (self.paused) {
    NSLog(@"Warning: draw called while paused");
  }
  */

  if (self->mKiwiApp && !self.paused) {
    self->mKiwiApp->render();
  }

}

@end
