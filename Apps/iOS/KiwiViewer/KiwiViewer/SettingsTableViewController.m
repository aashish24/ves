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

#import "SettingsTableViewController.h"

@interface SettingsTableViewController ()

@end

@implementation SettingsTableViewController

@synthesize midasHostText;
@synthesize pvwebHostText;

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
  [super viewDidLoad];

  self.midasHostText.text = [[NSUserDefaults standardUserDefaults] stringForKey:@"MidasHost"];
  self.pvwebHostText.text = [[NSUserDefaults standardUserDefaults] stringForKey:@"PVWebHost"];

  self.midasHostText.delegate = self;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
  [tableView deselectRowAtIndexPath:indexPath animated:YES];

  if (indexPath.section == 0 || indexPath.section == 1) {

    UITableViewCell* tableCell = [self.tableView cellForRowAtIndexPath:indexPath];
    NSString* text = tableCell.textLabel.text;
    NSString* url = nil;
    
    if ([text isEqualToString:@"License"]) {
      url = @"http://kiwiviewer.org/kiwiviewer/project/license.html";
    }
    else if ([text isEqualToString:@"Participants & Sponsors"]) {
      url = @"http://kiwiviewer.org/kiwiviewer/project/parti.html";
    }
    else if ([text isEqualToString:@"Developer's Guide"]) {
      url = @"http://vtk.org/Wiki/VES";
    }
    else if ([text isEqualToString:@"User's Guide"]) {
      url = @"http://kiwiviewer.org/kiwiviewer/help/documentation.html";
    }
    else if ([text isEqualToString:@"Home"]) {
      url = @"http://kiwiviewer.org";
    }
    else if ([text isEqualToString:@"Kitware, Inc."]) {
      url = @"http://www.kitware.com";
    }


    if (url) {
      [[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];
    }
  }
}

-(BOOL) textFieldShouldReturn:(UITextField *)textField
{
  [textField resignFirstResponder];
  return YES;
}

/*
- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string { 

  if (textField != self.pvRemotePort) {
    return YES;
  }

  NSRegularExpression *numbersOnly = [NSRegularExpression regularExpressionWithPattern:@"[0-9]+"
  options:NSRegularExpressionCaseInsensitive
  error:nil];
  NSInteger numberOfMatches = [numbersOnly numberOfMatchesInString:string options:0 range:NSMakeRange(0, string.length)];

  if (numberOfMatches != 1 && string.length != 0) {
    return NO;
  }
  
  return YES;
}
*/


-(IBAction) onMidasHostChanged:(id)sender
{
  NSString* host = self.midasHostText.text;

  // set default if empty string
  if ([host length] == 0) {
    host = @"http://midas3.kitware.com/midas";
    self.midasHostText.text = host;
  }

  // return if setting is unchanged
  if ([host isEqualToString:[[NSUserDefaults standardUserDefaults] stringForKey:@"MidasHost"]]) {
    return;
  }

  [[NSUserDefaults standardUserDefaults] setValue:host forKey:@"MidasHost"];
  [[NSUserDefaults standardUserDefaults] synchronize];
  [[NSNotificationCenter defaultCenter] postNotificationName:@"MidasHostChanged" object:nil];
}

-(IBAction) onPVWebHostChanged:(id)sender
{
  NSString* host = self.pvwebHostText.text;

  // set default if empty string
  if ([host length] == 0) {
    host = @"http://paraviewweb.kitware.com";
    self.pvwebHostText.text = host;
  }

  [[NSUserDefaults standardUserDefaults] setValue:host forKey:@"PVWebHost"];
  [[NSUserDefaults standardUserDefaults] synchronize];
}

@end
