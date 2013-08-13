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

#import "MyTabBarController.h"
#import "MyGLKViewController.h"
#import "MemoryUsageWatcher.h"

@interface MyTabBarController () {

  NSTimer* timer;
}

@end

@implementation MyTabBarController


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}


-(void) memoryUsageCallback
{
  //MemoryUsageWatcher::logMemoryUsage();
  MemoryUsageWatcher::notifyMemoryUsage();
}

- (void)viewDidLoad
{
  [super viewDidLoad];

  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(switchToDocumentsView) name:@"SwitchToDocumentsView" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(switchToRenderView:) name:@"OpenFile" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onDownloadURL:) name:@"Download URL" object:nil];
  
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onPVRemote:) name:@"ParaView Mobile Remote" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onPVWeb:) name:@"ParaView Web" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onPCLStreaming:) name:@"Point Cloud Streaming" object:nil];

  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(switchToRenderView:) name:@"PVRemoteStart" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(switchToRenderView:) name:@"PVWebStart" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(switchToRenderView:) name:@"PCLStreamingStart" object:nil];

  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onToggleFullScreen)
                                        name:@"TripleTapGesture" object:nil];

  self->timer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self
            selector:@selector(memoryUsageCallback) userInfo:nil repeats:YES];
}

-(void)onToggleFullScreen
{
  UIView *transitionView = [[[self.view.subviews reverseObjectEnumerator] allObjects] lastObject];
  if(transitionView == nil) {
    return;
  }

  bool isHiding = !self.tabBar.hidden;
  
  CGRect tabBarFrame = self.tabBar.frame;
  CGRect containerFrame = transitionView.frame;

  double tabBarHeight = tabBarFrame.size.height;

  if (!isHiding) {
    tabBarHeight = -tabBarHeight;
  }

  tabBarFrame.origin.y += tabBarHeight;
  containerFrame.size.height += tabBarHeight;


  self.tabBar.frame = tabBarFrame;
  transitionView.frame = containerFrame;
  self.tabBar.hidden = isHiding;
}

-(void)switchToDocumentsView
{
  self.selectedIndex = 0;
}

-(void)switchToRenderView:(NSNotification*)notification
{
  NSDictionary* userInfo = notification.userInfo;

  const int glViewIndex = 1;

  self.selectedIndex = glViewIndex;
  MyGLKViewController* glkView = (MyGLKViewController*)self.selectedViewController;
  [glkView handleArgs:userInfo];
}

-(void)onPVRemote:(NSNotification*)notification
{
  [self performSegueWithIdentifier: @"gotoPVRemoteScreen" sender: self];
}

-(void)onPVWeb:(NSNotification*)notification
{
  [self performSegueWithIdentifier: @"gotoPVWebScreen" sender: self];
}

-(void)onPCLStreaming:(NSNotification*)notification
{
  [self performSegueWithIdentifier: @"gotoPointCloudStreamingScreen" sender: self];
}

-(void)onDownloadURL:(NSNotification*)notification
{
  [self performSegueWithIdentifier: @"gotoDownloadURL" sender: self];
}

@end
