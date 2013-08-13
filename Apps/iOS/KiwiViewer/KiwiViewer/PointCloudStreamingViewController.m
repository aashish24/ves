//
//  PointCloudStreamingViewController.m
//  KiwiMidas
//
//  Created by Pat Marion on 12/5/12.
//  Copyright (c) 2012 Pat Marion. All rights reserved.
//

#import "PointCloudStreamingViewController.h"

@interface PointCloudStreamingViewController ()

@end

@implementation PointCloudStreamingViewController

@synthesize hostText;

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
  self.hostText.text = [[NSUserDefaults standardUserDefaults] stringForKey:@"PointCloudStreamingHost"];
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
  if (textField == self.hostText) {
    [textField resignFirstResponder];
    [self onConnectTouched:nil];
  }
  return YES;
}

-(void)showError
{
  NSString* title = @"Connection Failed";
  NSString* message = @"Failed to connect to the server.";
  [self showAlertDialogWithTitle:title message:message];
}

-(IBAction)onHelpTouched:(id)sender
{
  NSString* docPage = @"http://www.vtk.org/Wiki/VES/Point_Cloud_Streaming";
  [[UIApplication sharedApplication] openURL:[NSURL URLWithString:docPage]];
}


-(IBAction)onCancelTouched:(id)sender
{
  [[NSNotificationCenter defaultCenter] postNotificationName:@"SwitchToDocumentsView" object:nil];
  [self dismissViewControllerAnimated:YES completion:nil];
}

-(IBAction) onConnectTouched: (id) sender
{
  NSString* host = self.hostText.text;

  if (!host.length) {
    [self showAlertDialogWithTitle:@"Missing host" message:@"Please enter the host name."];
    return;
  }

  [[NSUserDefaults standardUserDefaults] setObject:host forKey:@"PointCloudStreamingHost"];
  [[NSUserDefaults standardUserDefaults] synchronize];

  NSDictionary* args = [NSDictionary dictionaryWithObjectsAndKeys:@"Point Cloud Streaming", @"AppName", self, @"viewController", nil];
  [[NSNotificationCenter defaultCenter] postNotificationName:@"PCLStreamingStart" object:nil userInfo:args];
}

@end
