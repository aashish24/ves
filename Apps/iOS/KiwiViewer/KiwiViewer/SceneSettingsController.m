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

#import "SceneSettingsController.h"
#import "MemoryUsageWatcher.h"
#import "GLInfoViewController.h"
#import "ObjectSettings.h"
#import "ColorPickerController.h"
#include "kiwiCloudApp.h"

#include <QuartzCore/CALayer.h>



@interface SceneSettingsController () <ColorPickerDelegate> {


  NSTimer* fpsTimer;
  UIButton* activatedColorButton;
  
}

@end

@implementation SceneSettingsController

@synthesize app;


@synthesize fpsLabel;
@synthesize vertexCountLabel;
@synthesize cellCountLabel;
@synthesize memoryUsageLabel;
@synthesize objectCountLabel;

@synthesize cameraSpinSlider;
@synthesize multisamplingSwitch;
@synthesize clearSceneAutomaticallySwitch;
@synthesize cameraProjectionControl;

@synthesize colorButton;
@synthesize colorButton2;
@synthesize backgroundGradientSwitch;

- (id)initWithCoder:(NSCoder *)coder
{
  self = [super initWithCoder:coder];
  if (self) {

  }
  return self;
}

- (IBAction)onDoneTouched
{
  [self clearTimer];
  [self dismissViewControllerAnimated:YES completion:NULL];
}

- (void)viewDidLoad
{
  [super viewDidLoad];

  self->fpsTimer = [NSTimer scheduledTimerWithTimeInterval:0.5 target:self
            selector:@selector(updateFPS) userInfo:nil repeats:YES];
  
  
  [self.cameraSpinSlider setValue:[[NSUserDefaults standardUserDefaults] doubleForKey:@"CameraSpinDeceleration"] animated:NO];
  [self.multisamplingSwitch setOn:[[NSUserDefaults standardUserDefaults] boolForKey:@"EnableMultisampling"] animated:NO];
  [self.clearSceneAutomaticallySwitch setOn:[[NSUserDefaults standardUserDefaults] boolForKey:@"ClearSceneAutomatically"] animated:NO];
  [self.backgroundGradientSwitch setOn:[[NSUserDefaults standardUserDefaults] boolForKey:@"UseBackgroundGradient"] animated:NO];


  [self.cameraProjectionControl
    setSelectedSegmentIndex:[[NSUserDefaults standardUserDefaults] integerForKey:@"CameraProjectionMode"]];


  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onUpdateMemoryUsage:)
                                        name:@"UpdateMemoryUsage" object:nil];

  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateSceneStatistics)
                                        name:@"UpdateSceneStatistics" object:nil];


  self.colorButton.backgroundColor = [SceneSettingsController backgroundColor];
  self.colorButton.layer.cornerRadius = 6;
  self.colorButton.layer.borderWidth = 1;
  self.colorButton.layer.borderColor = [[UIColor lightGrayColor] CGColor];

  self.colorButton2.backgroundColor = [SceneSettingsController backgroundColor2];
  self.colorButton2.layer.cornerRadius = 6;
  self.colorButton2.layer.borderWidth = 1;
  self.colorButton2.layer.borderColor = [[UIColor lightGrayColor] CGColor];

  [self updateColorButtons];

  MemoryUsageWatcher::notifyMemoryUsage();
}

+(void)setDefaultBackgroundColor
{
  UIColor* backgroundColor = [UIColor colorWithRed:0.267 green:0.267 blue:0.267 alpha:1.0];
  UIColor* backgroundColor2 = [UIColor colorWithRed:0.0 green:0. blue:0.0 alpha:1.0];
  [[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:backgroundColor] forKey:@"BackgroundColor"];
  [[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:backgroundColor2] forKey:@"BackgroundColor2"];
  [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"UseBackgroundGradient"];
}

+(void)initDefaultBackgroundColor
{
  if (![[NSUserDefaults standardUserDefaults] objectForKey:@"BackgroundColor"]) {
    [SceneSettingsController setDefaultBackgroundColor];
  }
}

+(UIColor*)backgroundColor
{
  [SceneSettingsController initDefaultBackgroundColor];
  return [NSKeyedUnarchiver unarchiveObjectWithData:[[NSUserDefaults standardUserDefaults] objectForKey:@"BackgroundColor"]];
}

+(UIColor*)backgroundColor2
{
  [SceneSettingsController initDefaultBackgroundColor];
  return [NSKeyedUnarchiver unarchiveObjectWithData:[[NSUserDefaults standardUserDefaults] objectForKey:@"BackgroundColor2"]];
}

-(void)clearTimer
{
  [self->fpsTimer invalidate];
  self->fpsTimer = nil;
}

-(void)viewWillAppear:(BOOL)animated
{
  [super viewWillAppear:animated];
  
  [self updateSceneStatistics];
}

-(void)updateFPS
{
  int fps = 0;
  if (self->app) {
    fps = self->app->fpsCounter().averageFPS();
    fps = static_cast<int>(floor(fps + 0.5));
  }

  self.fpsLabel.text = [[NSNumber numberWithInt:fps] stringValue];
}

-(void)updateSceneStatistics
{
  int vertexCount = 0;
  int cellCount = 0;

  if (self->app) {
    vertexCount = self->app->numberOfModelVertices();
    cellCount = self->app->numberOfModelFacets() + self->app->numberOfModelLines();
  }

  self.vertexCountLabel.text = [[NSNumber numberWithInt:vertexCount] stringValue];
  self.cellCountLabel.text = [[NSNumber numberWithInt:cellCount] stringValue];

  [self updateNumberOfObjects];
  [self updateFPS];
}

-(void)updateNumberOfObjects
{
  int numberOfObjects = 0;
  if (self.app) {
    numberOfObjects = self->app->dataRepresentations().size();
  }

  if (numberOfObjects) {
    self.objectCountLabel.text = [NSString stringWithFormat:@"%d object%@", numberOfObjects, (numberOfObjects > 1 ? @"s" : @"")];
    self.objectSettingsCell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    self.objectSettingsCell.selectionStyle = UITableViewCellSelectionStyleBlue;
  }
  else {
    self.objectCountLabel.text = @"No objects";
    self.objectSettingsCell.accessoryType = UITableViewCellAccessoryNone;
    self.objectSettingsCell.selectionStyle = UITableViewCellSelectionStyleNone;
  }
}

-(IBAction) onCameaSpinChanged:(id) sender
{
  double spinDeceleration = self.cameraSpinSlider.value;
  [[NSUserDefaults standardUserDefaults] setDouble:spinDeceleration forKey:@"CameraSpinDeceleration"];
  [[NSUserDefaults standardUserDefaults] synchronize];
  [[NSNotificationCenter defaultCenter] postNotificationName:@"CameraSpinDecelerationChanged" object:nil];
}

-(IBAction) onCameraProjectionModeChanged
{
  int projectionMode = self.cameraProjectionControl.selectedSegmentIndex;
  [[NSUserDefaults standardUserDefaults] setInteger:projectionMode forKey:@"CameraProjectionMode"];
  [[NSUserDefaults standardUserDefaults] synchronize];
  [[NSNotificationCenter defaultCenter] postNotificationName:@"CameraProjectionModeChanged" object:nil];
}

-(IBAction) onToggleMultisampling:(id)sender
{
  [[NSUserDefaults standardUserDefaults] setBool:self.multisamplingSwitch.on forKey:@"EnableMultisampling"];
  [[NSUserDefaults standardUserDefaults] synchronize];
  [[NSNotificationCenter defaultCenter] postNotificationName:@"EnableMultisamplingChanged" object:nil];
}

-(IBAction) onToggleClearSceneAutomatically:(id)sender
{
  [[NSUserDefaults standardUserDefaults] setBool:self.clearSceneAutomaticallySwitch.on forKey:@"ClearSceneAutomatically"];
  [[NSUserDefaults standardUserDefaults] synchronize];
  [[NSNotificationCenter defaultCenter] postNotificationName:@"ClearSceneAutomaticallyChanged" object:nil];
}

-(IBAction) onUseBackgroundGradientChanged
{
  [[NSUserDefaults standardUserDefaults] setBool:self.backgroundGradientSwitch.on forKey:@"UseBackgroundGradient"];
  [[NSUserDefaults standardUserDefaults] synchronize];
  [SceneSettingsController setBackgroundColor:self.app];
  [self updateColorButtons];
}

-(void)updateColorButtons
{
  self.colorButton.backgroundColor = [SceneSettingsController backgroundColor];
  self.colorButton2.backgroundColor = [SceneSettingsController backgroundColor2];

  bool useGradient = [[NSUserDefaults standardUserDefaults] boolForKey:@"UseBackgroundGradient"];
  self.backgroundGradientSwitch.on = useGradient;
  self.colorButton2.hidden = !useGradient;
}

+(void)setBackgroundColor:(vesSharedPtr<kiwiApp>)app
{
  if (!app) {
    return;
  }
  
  UIColor *backgroundColor = [SceneSettingsController backgroundColor];
  UIColor *backgroundColor2 = [SceneSettingsController backgroundColor2];
  bool useGradient = [[NSUserDefaults standardUserDefaults] boolForKey:@"UseBackgroundGradient"];

  vesVector3f topColor;
  vesVector3f bottomColor;

  [backgroundColor getRed:&bottomColor[0] green:&bottomColor[1] blue:&bottomColor[2] alpha:NULL];

  if (useGradient) {
    [backgroundColor2 getRed:&topColor[0] green:&topColor[1] blue:&topColor[2] alpha:NULL];
  }
  else {
    topColor = bottomColor;
  }

  vesBackground::Ptr background = app->renderer()->background();
  background->setGradientColor(topColor, bottomColor);
}

- (void)colorPickerSaved:(ColorPickerController *)controller
{
  UIColor* newColor = [controller selectedColor];
  self->activatedColorButton.backgroundColor = newColor;
  NSString* backgroundColorName = @"BackgroundColor";
  if (self->activatedColorButton == self.colorButton2) {
    backgroundColorName = @"BackgroundColor2";
  }
  [self.navigationController popViewControllerAnimated:YES];

  [[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:newColor] forKey:backgroundColorName];
  
  [SceneSettingsController setBackgroundColor:self.app];
}

- (void)colorPickerCancelled:(ColorPickerController *)controller
{
  [self.navigationController popViewControllerAnimated:YES];
}

-(IBAction) onColorButtonTouched:(id)sender
{
  if (sender == self.colorButton) {
    self->activatedColorButton = self.colorButton;
  }
  else {
    self->activatedColorButton = self.colorButton2;
  }

  UIColor* currentColor = self->activatedColorButton.backgroundColor;

  ColorPickerController *colorPicker = [[ColorPickerController alloc] initWithColor:currentColor andTitle:@"Choose Color"];
  [colorPicker setContentSizeForViewInPopover:self.view.frame.size];
  colorPicker.delegate = self;

  [self.navigationController pushViewController:colorPicker animated:YES];
}

-(void)onDefaultBackgroundColor
{
  [SceneSettingsController setDefaultBackgroundColor];
  [self updateColorButtons];
  [SceneSettingsController setBackgroundColor:self.app];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
  if (indexPath.section == 1) {
    if (self.objectSettingsCell.selectionStyle != UITableViewCellSelectionStyleNone) {
      [self performSegueWithIdentifier:@"gotoObjectSettings" sender:self];
    }
  }
  else if (indexPath.section == 2) {
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"ClearScene" object:nil];
  }
  else if (indexPath.section == 3 && indexPath.row == 2) {
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    [self onDefaultBackgroundColor];
  }
  else if (indexPath.section == 5 && indexPath.row == 0) {

    GLInfoViewController* infoView = [[GLInfoViewController alloc] initWithStyle:UITableViewStyleGrouped];
    [infoView setContentSizeForViewInPopover:self.view.frame.size];
    [self.navigationController pushViewController:infoView animated:YES];
  }

}

-(void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
  ObjectSettings* settings = segue.destinationViewController;
  settings.app = self.app;

  if (self->app && self->app->dataRepresentations().size() == 1) {
    settings.dataRepresentation = self->app->dataRepresentations()[0];
  }
}

-(void) onUpdateMemoryUsage:(NSNotification*)notification
{
  NSDictionary* args = notification.userInfo;
  NSNumber* memoryUsage = [args objectForKey:@"MemoryUsage"];
  self.memoryUsageLabel.text = [NSString stringWithFormat:@"%.1f MB", [memoryUsage doubleValue]];
}

-(void) onUpdateFPS:(NSNotification*)notification
{
  NSDictionary* args = notification.userInfo;
  NSNumber* fps = [args objectForKey:@"FPS"];
  self.fpsLabel.text = [fps stringValue];
}

@end
