//
//  PVWebViewController.m
//  KiwiMidas
//
//  Created by Pat Marion on 12/5/12.
//  Copyright (c) 2012 Pat Marion. All rights reserved.
//

#import "PVWebViewController.h"

@interface PVWebViewController ()

@end

@implementation PVWebViewController

@synthesize sessionText;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
  [super viewDidLoad];
  self.sessionText.text = [[NSUserDefaults standardUserDefaults] stringForKey:@"PVWebSession"];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)showAlertDialogWithTitle:(NSString *)alertTitle message:(NSString *)alertMessage;
{

  UIAlertView *alert = [[UIAlertView alloc]
                        initWithTitle:alertTitle
                        message:alertMessage
                        delegate:self
                        cancelButtonTitle:@"Ok"
                        otherButtonTitles: nil, nil];
  [alert show];
}

- (NSUInteger)supportedInterfaceOrientations
{
  if (UI_USER_INTERFACE_IDIOM()==UIUserInterfaceIdiomPhone) {
    return UIInterfaceOrientationMaskPortrait | UIInterfaceOrientationMaskPortraitUpsideDown;
  }
  return UIInterfaceOrientationMaskAll;
}

-(BOOL)textFieldShouldReturn:(UITextField *)textField
{
  if (textField == self.sessionText) {
    [textField resignFirstResponder];
    [self onConnectTouched:nil];
  }
  return YES;
}

-(void)showError
{
  NSString* title = @"Connection Failed";
  NSString* message = @"Failed to connect to the ParaViewWeb server.";
  [self showAlertDialogWithTitle:title message:message];
}

-(IBAction)onHelpTouched:(id)sender
{
  NSString* docPage = @"http://www.vtk.org/Wiki/VES/Connect_To_ParaViewWeb";
  [[UIApplication sharedApplication] openURL:[NSURL URLWithString:docPage]];
}


-(IBAction)onCancelTouched:(id)sender
{
  [[NSNotificationCenter defaultCenter] postNotificationName:@"SwitchToDocumentsView" object:nil];
  [self dismissViewControllerAnimated:YES completion:nil];
}

-(IBAction) onConnectTouched: (id) sender
{  
  NSString* session = self.sessionText.text;

  //if (!session.length) {
  //  [self showAlertDialogWithTitle:@"Missing session" message:@"Please enter the session name."];
  //  return;
  //}

  [[NSUserDefaults standardUserDefaults] setObject:session forKey:@"PVWebSession"];
  [[NSUserDefaults standardUserDefaults] synchronize];

  NSDictionary* args = [NSDictionary dictionaryWithObjectsAndKeys:@"ParaView Web", @"AppName", self, @"viewController", nil];
  [[NSNotificationCenter defaultCenter] postNotificationName:@"PVWebStart" object:nil userInfo:args];
}

@end
