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
class vesKiwiDataRepresentation;
class vesKiwiPolyDataRepresentation;

@interface ObjectSettings : UITableViewController

@property vesSharedPtr<kiwiApp> app;
@property vesSharedPtr<vesKiwiDataRepresentation> dataRepresentation;

@end


@interface WidgetCell : UITableViewCell
  @property vesSharedPtr<vesKiwiPolyDataRepresentation> rep;
  -(void) initWidget;
@end

@interface StepperCell : WidgetCell
  @property IBOutlet UIStepper* stepper;
  @property IBOutlet UILabel* valueLabel;

  -(double) currentValue;
  -(void) setValue:(double)value;
@end

@interface VisibilityCell : WidgetCell
  @property IBOutlet UISwitch* switchWidget;

  -(bool) currentValue;
  -(void) setValue:(bool)value;
@end

@interface OpacityCell : StepperCell
@end

@interface PointSizeCell : StepperCell
@end

@interface LineWidthCell : StepperCell
@end

@interface ColorCell : WidgetCell
  @property IBOutlet UIButton* colorButton;
  @property IBOutlet UILabel* colorTextLabel;

@end
