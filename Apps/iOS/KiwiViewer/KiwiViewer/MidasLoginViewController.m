//
//  MidasLoginViewController.m
//  CloudAppTab
//
//  Created by Pat Marion on 9/29/12.
//  Copyright (c) 2012 Pat Marion. All rights reserved.
//

#import "MidasLoginViewController.h"
#import "MyGLKViewController.h"
#import "DocumentsTableViewController.h"
#import "MBProgressHUD.h"

#include <vesMidasClient.h>

@interface MidasLoginViewController () {

}

@end

@implementation MidasLoginViewController

@synthesize client;
@synthesize userText;
@synthesize passwordText;

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

  self.userText.text = [[NSUserDefaults standardUserDefaults] stringForKey:@"Username"];
  self.passwordText.text = [[NSUserDefaults standardUserDefaults] stringForKey:@"Password"];
}

-(BOOL)textFieldShouldReturn:(UITextField *)textField
{
  if (textField == userText) {
    [passwordText becomeFirstResponder];
  }
  else if (textField == passwordText) {
    [textField resignFirstResponder];
    [self onLoginTouched:nil];
  }
  else {
    [textField resignFirstResponder];
  }
  return YES;
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

-(IBAction)onCreateAccount:(id)sender
{
  NSString* midasHost = [[NSUserDefaults standardUserDefaults] stringForKey:@"MidasHost"];
  NSURL * url = [NSURL URLWithString:midasHost];
  if (![[url scheme] length]) {
    url = [NSURL URLWithString:[@"http://" stringByAppendingString:midasHost]];
  }

  [[UIApplication sharedApplication] openURL:url];
}

-(IBAction) onLoginTouched: (id) sender {


  [self.userText resignFirstResponder];
  [self.passwordText resignFirstResponder];

  NSString* username = self.userText.text;
  NSString* password = self.passwordText.text;

  if (!username.length) {
    [self showAlertDialogWithTitle:@"Missing email" message:@"Please enter your email."];
    return;
  }
  if (!password.length) {
    [self showAlertDialogWithTitle:@"Missing password" message:@"Please enter your password."];
    return;
  }

  [[NSUserDefaults standardUserDefaults] setObject:username forKey:@"Username"];
  [[NSUserDefaults standardUserDefaults] setObject:password forKey:@"Password"];
  [[NSUserDefaults standardUserDefaults] synchronize];

  NSString* midasHost = [[NSUserDefaults standardUserDefaults] stringForKey:@"MidasHost"];
  self.client->reset();
  self.client->setHost([midasHost UTF8String]);


  MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:self.view animated:YES withDelay:0.5];
  hud.labelText = @"Contacting Midas...";
  dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{

    bool loginResult = self.client->login([username UTF8String], [password UTF8String]);

    dispatch_async(dispatch_get_main_queue(), ^{
      [MBProgressHUD hideHUDForView:self.view animated:YES];
      if (loginResult) {
        [self performSegueWithIdentifier: @"midasLoginReturn" sender: self];
      }
      else {

        NSString* title = @"Login Failed";
        NSString* message = @"The login to Midas failed.";
        [self showAlertDialogWithTitle:title message:message];
      }

    });
  });




}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
