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
#import <GLKit/GLKit.h>

#include <vesSharedPtr.h>

class kiwiApp;

@interface MyGLKViewController : GLKViewController <UIPopoverControllerDelegate> 


  -(void) handleArgs:(NSDictionary*) args;

  @property (nonatomic, retain) IBOutlet UIBarButtonItem *settingsButton;
  @property (nonatomic, retain) IBOutlet UIBarButtonItem *zoomToFitButton;
  @property (nonatomic, retain) IBOutlet UIToolbar *toolbar;

  @property (nonatomic, retain) IBOutlet UILabel *leftLabel;
  @property (nonatomic, retain) IBOutlet UILabel *rightLabel;

  -(vesSharedPtr<kiwiApp>) currentApp;

@end
