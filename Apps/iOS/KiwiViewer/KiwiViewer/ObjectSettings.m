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

#import "ObjectSettings.h"
#import "ColorPickerController.h"


#include "kiwiCloudApp.h"
#include "vesKiwiDataRepresentation.h"
#include "vesKiwiPolyDataRepresentation.h"
#include "vesActor.h"

#include <QuartzCore/CALayer.h>


@implementation WidgetCell

  @synthesize rep;

  -(void)initWidget
  {
  }

@end


@implementation ColorCell
  @synthesize colorButton;
  @synthesize colorTextLabel;

  -(void)setValue:(UIColor*) value
  {
    vesVector4f color = self.rep->color();
    [value getRed:&color[0] green:&color[1] blue:&color[2] alpha:&color[3]];
    self.rep->setColor(color[0], color[1], color[2], self.rep->opacity());
    self.colorButton.backgroundColor = value;
  }

  -(UIColor*)currentValue
  {
    vesVector4f repColor = self.rep->color();
    return [UIColor colorWithRed:repColor[0] green:repColor[1] blue:repColor[2] alpha:1.0];
  }


  -(void)initWidget
  {

    self.accessoryType = UITableViewCellAccessoryCheckmark;
    self.colorButton.backgroundColor = [self currentValue];
    self.colorButton.layer.cornerRadius = 6;
    self.colorButton.layer.borderWidth = 1;
    self.colorButton.layer.borderColor = [[UIColor lightGrayColor] CGColor];
  }

@end

@implementation StepperCell
  @synthesize stepper;
  @synthesize valueLabel;

  -(void)setValue:(double) value
  {
  }

  -(double)currentValue
  {
    return 0.0;
  }

  -(void)stepperValueChanged
  {
    self.valueLabel.text = [NSString stringWithFormat:@"%0.1f", self.stepper.value];
    [self setValue:self.stepper.value];
  }

  -(void)initWidget
  {
    double value = [self currentValue];
    [self.stepper setValue:value];
    self.valueLabel.text = [NSString stringWithFormat:@"%0.1f", value];
    [self.stepper addTarget:self action:@selector(stepperValueChanged) forControlEvents:UIControlEventValueChanged];
  }

@end


@implementation OpacityCell

  -(void)setValue:(double)value
  {
    self.rep->setOpacity(value);
  }

  -(double)currentValue
  {
    return self.rep->opacity();
  }

@end


@implementation PointSizeCell

  -(void)setValue:(double)value
  {
    self.rep->setPointSize(value);
  }

  -(double)currentValue
  {
    return self.rep->pointSize();
  }

@end


@implementation LineWidthCell

  -(void)setValue:(double)value
  {
    self.rep->setLineWidth(value);
  }

  -(double)currentValue
  {
    return self.rep->lineWidth();
  }

@end

@implementation VisibilityCell
  @synthesize switchWidget;

  -(void)setValue:(bool) value
  {
    self.rep->actor()->setVisible(value);
  }

  -(bool)currentValue
  {
    return self.rep->actor()->isVisible();
  }

  -(void)switchValueChanged
  {
    [self setValue:self.switchWidget.on];
  }

  -(void)initWidget
  {
    bool value = [self currentValue];
    [self.switchWidget setOn:value];
    [self.switchWidget addTarget:self action:@selector(switchValueChanged) forControlEvents:UIControlEventValueChanged];
  }

@end


@interface ObjectSettings () <ColorPickerDelegate> {

  int numberOfSections;
  int objectsSection;
  int opacitySection;
  int geometryModeSection;
  int colorBySection;
  int notSupportedSection;

}

@end

@implementation ObjectSettings

@synthesize app;
@synthesize dataRepresentation;

-(id)initWithCoder:(NSCoder *)coder
{
  self = [super initWithCoder:coder];
  if (self) {

  }
  return self;
}

- (void)viewDidLoad
{
  [super viewDidLoad];

  self->numberOfSections = 0;
  self->objectsSection = -1;
  self->opacitySection = -1;
  self->geometryModeSection = -1;
  self->colorBySection = -1;

  if (vesDynamicPtrCast<vesKiwiPolyDataRepresentation>(self->dataRepresentation)) {
    self->opacitySection = self->numberOfSections++;
    self->geometryModeSection = self->numberOfSections++;
    self->colorBySection = self->numberOfSections++;
  }
  else if (self->app && !self->dataRepresentation) {
    self->objectsSection = self->numberOfSections++;
  }
  else {
    self->notSupportedSection = self->numberOfSections++;
  }
  
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
  return self->numberOfSections;
}


- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
  if (section == self->objectsSection) {
    return @"Objects";
  }
  else if (section == self->opacitySection) {
    return @"Appearance";
  }
  else if (section == self->geometryModeSection) {
    return @"Style";
  }
  else if (section == self->colorBySection) {
    return @"Color By";
  }
  else if (section == self->notSupportedSection) {
    return @"Settings not available.";
  }

  return @"";
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
  vesKiwiPolyDataRepresentation::Ptr rep = vesDynamicPtrCast<vesKiwiPolyDataRepresentation>(self->dataRepresentation);
  
  if (section == self->objectsSection) {
    return self->app->dataRepresentations().size();
  }
  else if (section == self->opacitySection) {
    return 4;
  }
  else if (section == self->geometryModeSection) {
    return 4;
  }
  else if (section == self->colorBySection) {
    return rep->colorModes().size();
  }
  else if (section == self->notSupportedSection) {
    return 0;
  }
  return 0;
}


-(ColorCell*) colorCell
{
  return (ColorCell*)[self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:self->colorBySection]];
}

- (void)colorPickerSaved:(ColorPickerController *)controller
{
  UIColor* newColor = [controller selectedColor];
  [[self colorCell] setValue:newColor];
  [self.navigationController popViewControllerAnimated:YES];
}

- (void)colorPickerCancelled:(ColorPickerController *)controller
{
  [self.navigationController popViewControllerAnimated:YES];
}

-(void) onColorButtonTouched
{

  ColorCell* cell = [self colorCell];
  
  UIColor* currentColor = [cell currentValue];

  ColorPickerController *colorPicker = [[ColorPickerController alloc] initWithColor:currentColor andTitle:@"Choose Color"];
  [colorPicker setContentSizeForViewInPopover:self.view.frame.size];
  colorPicker.delegate = self;

  [self.navigationController pushViewController:colorPicker animated:YES];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
  if (indexPath.section == self->objectsSection) {
    UITableViewCell *cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"ObjectNameCell"];
    cell.textLabel.text = [NSString stringWithFormat:@"Object %d", indexPath.row + 1];
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    return cell;
  }
  else if (indexPath.section == self->opacitySection) {
    static NSString *cellIdentifier;

    if (indexPath.row == 0) {
      cellIdentifier = @"VisibilityCell";
    }
    else if (indexPath.row == 1) {
      cellIdentifier = @"OpacityCell";
    }
    else if (indexPath.row == 2) {
      cellIdentifier = @"PointSizeCell";
    }
    else if (indexPath.row == 3) {
      cellIdentifier = @"LineWidthCell";
    }
    
    WidgetCell *cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier forIndexPath:indexPath];
    cell.rep = vesDynamicPtrCast<vesKiwiPolyDataRepresentation>(self->dataRepresentation);
    [cell initWidget];

    return cell;
  }
  else if (indexPath.section == self->geometryModeSection) {
    UITableViewCell *cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"GeometryModeCell"];
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    if (indexPath.row == 0) {
      cell.textLabel.text = @"Surface";
    }
    else if (indexPath.row == 1) {
      cell.textLabel.text = @"Surface with edges";
    }
    else if (indexPath.row == 2) {
      cell.textLabel.text = @"Wireframe";
    }
    else if (indexPath.row == 3) {
      cell.textLabel.text = @"Points";
    }

    [self updateCheckmarkInCell:cell isSelected:(indexPath.row == [self currentGeometryMode])];

    return cell;
  }
  else if (indexPath.section == self->colorBySection) {

    vesKiwiPolyDataRepresentation::Ptr rep = vesDynamicPtrCast<vesKiwiPolyDataRepresentation>(self->dataRepresentation);
    std::string colorMode = rep->colorModes()[indexPath.row];


    UITableViewCell* cell;

    if (colorMode == "Solid Color") {
      ColorCell *colorCell = [tableView dequeueReusableCellWithIdentifier:@"ColorCell" forIndexPath:indexPath];
      colorCell.rep = vesDynamicPtrCast<vesKiwiPolyDataRepresentation>(self->dataRepresentation);
      [colorCell initWidget];
      [colorCell.colorButton addTarget:self action:@selector(onColorButtonTouched) forControlEvents:UIControlEventTouchUpInside];
      cell = colorCell;
    }
    else {
       cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"ColorByCell"];
       cell.textLabel.text = [NSString stringWithUTF8String:colorMode.c_str()];
    }

    cell.selectionStyle = UITableViewCellSelectionStyleNone;

    [self updateCheckmarkInCell:cell isSelected:(indexPath.row == [self currentColorByMode])];

    return cell;
  }

  return nil;
}

-(int)currentGeometryMode
{
  vesKiwiPolyDataRepresentation::Ptr rep = vesDynamicPtrCast<vesKiwiPolyDataRepresentation>(self->dataRepresentation);
  if (!rep) {
    return -1;
  }

  return rep->geometryMode();
}

-(int)currentColorByMode
{
  vesKiwiPolyDataRepresentation::Ptr rep = vesDynamicPtrCast<vesKiwiPolyDataRepresentation>(self->dataRepresentation);
  if (!rep) {
    return -1;
  }

  std::vector<std::string> colorModes = rep->colorModes();
  std::vector<std::string>::const_iterator itr = std::find(colorModes.begin(), colorModes.end(), rep->colorMode());
  if (itr == colorModes.end()) {
    return -1;
  }

  int colorMode = itr - colorModes.begin();
  return colorMode;
}

-(void)updateCheckmarkInCell:(UITableViewCell*)cell isSelected:(bool)selected
{
  cell.accessoryType = selected ? UITableViewCellAccessoryCheckmark : UITableViewCellAccessoryNone;

  UILabel* label;

  if ([cell isKindOfClass:[ColorCell class]]) {
    label = ((ColorCell*)cell).colorTextLabel;
  }
  else {
    label = cell.textLabel;
  }
  cell.textLabel.textColor = selected ? [UIColor colorWithRed:56/255.0 green:84/255.0 blue:135/255.0 alpha:1.0] : [UIColor blackColor];
}

-(void)updateGeometryModeCells
{
  int geometryMode = [self currentGeometryMode];
  for (int i = 0; i < 4; ++i) {
    UITableViewCell* cell = [self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:i  inSection:self->geometryModeSection]];
    [self updateCheckmarkInCell:cell isSelected:(i == geometryMode)];
  }
}

-(void)updateColorByCells:(int)colorBy
{
  for (int i = 0; i < [self.tableView numberOfRowsInSection:self->colorBySection]; ++i) {
    UITableViewCell* cell = [self.tableView cellForRowAtIndexPath:[NSIndexPath indexPathForRow:i  inSection:self->colorBySection]];
    [self updateCheckmarkInCell:cell isSelected:(i == colorBy)];
  }
}

-(void)handleGeometryModeSelected:(int)geometryMode
{
  vesKiwiPolyDataRepresentation::Ptr rep = vesDynamicPtrCast<vesKiwiPolyDataRepresentation>(self->dataRepresentation);
  if (!rep) {
    return;
  }

  if (geometryMode == rep->geometryMode()) {
    return;
  }

  if (geometryMode == 0) {
    rep->surfaceOn();
  }
  else if (geometryMode == 1) {
    rep->surfaceWithEdgesOn();
  }
  else if (geometryMode == 2) {
    rep->wireframeOn();
  }
  else if (geometryMode == 3) {
    rep->pointsOn();
  }

  [self updateGeometryModeCells];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
  if (indexPath.section == self->objectsSection) {

    int row = indexPath.row;
    if (row < self->app->dataRepresentations().size()) {
      UIStoryboard*  sb =  self.storyboard;
      ObjectSettings* settings = [sb instantiateViewControllerWithIdentifier:@"ObjectSettings"];
      settings.dataRepresentation = self->app->dataRepresentations()[row];
      [self.navigationController pushViewController:settings animated:YES];
  
    }
  }
  else if (indexPath.section == self->geometryModeSection) {
    int geometryMode = indexPath.row;
    [self handleGeometryModeSelected:geometryMode];
  }
  else if (indexPath.section == self->colorBySection) {

    vesKiwiPolyDataRepresentation::Ptr rep = vesDynamicPtrCast<vesKiwiPolyDataRepresentation>(self->dataRepresentation);
    std::string colorMode = rep->colorModes()[indexPath.row];
    rep->setColorMode(colorMode);
    [self updateGeometryModeCells];
    [self updateColorByCells:indexPath.row];

  }
}

@end
