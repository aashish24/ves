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

#import <UIKit/UIKit.h>

#include <vesSharedPtr.h>
class kiwiApp;

@interface SceneSettingsController : UITableViewController


- (IBAction) onDoneTouched;
- (IBAction) onToggleMultisampling:(id) sender;
- (IBAction) onToggleClearSceneAutomatically:(id) sender;
- (IBAction) onCameaSpinChanged:(id) sender;
- (IBAction) onCameraProjectionModeChanged;
- (IBAction) onUseBackgroundGradientChanged;

- (IBAction) onColorButtonTouched:(id) sender;

-(void)clearTimer;

+(void)setBackgroundColor:(vesSharedPtr<kiwiApp>)app;

  @property vesSharedPtr<kiwiApp> app;


  @property IBOutlet UILabel *vertexCountLabel;
  @property IBOutlet UILabel *cellCountLabel;
  @property IBOutlet UILabel *objectCountLabel;
  @property IBOutlet UITableViewCell *objectSettingsCell;


  @property IBOutlet UILabel *fpsLabel;
  @property IBOutlet UILabel *memoryUsageLabel;

  @property IBOutlet UISegmentedControl *cameraProjectionControl;

  @property IBOutlet UISwitch *multisamplingSwitch;
  @property IBOutlet UISwitch *clearSceneAutomaticallySwitch;
  @property IBOutlet UISlider *cameraSpinSlider;

  @property IBOutlet UIButton *colorButton;
  @property IBOutlet UIButton *colorButton2;
  @property IBOutlet UISwitch *backgroundGradientSwitch;



@end
