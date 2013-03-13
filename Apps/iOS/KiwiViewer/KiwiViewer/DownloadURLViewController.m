//
//  DownloadURLViewController.m
//  KiwiMidas
//
//  Created by Pat Marion on 12/5/12.
//  Copyright (c) 2012 Pat Marion. All rights reserved.
//

#import "DownloadURLViewController.h"
#import "AppDelegate.h"
#import "MBProgressHUD.h"
#include <vesKiwiCurlDownloader.h>

@interface DownloadURLViewController ()

@end

@implementation DownloadURLViewController

@synthesize urlText;

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
  self.urlText.text = [[NSUserDefaults standardUserDefaults] stringForKey:@"DownloadURL"];
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
  if (textField == self.urlText) {
    [textField resignFirstResponder];
    [self onDownloadTouched:nil];
  }
  return YES;
}

-(void)showError
{
  NSString* title = @"Download Failed";
  NSString* message = @"Failed to download URL.";
  [self showAlertDialogWithTitle:title message:message];
}

-(IBAction)onHelpTouched:(id)sender
{
  NSString* docPage = @"http://www.kiwiviewer.org/kiwiviewer/help/documentation.html";
  [[UIApplication sharedApplication] openURL:[NSURL URLWithString:docPage]];
}

-(IBAction)onCancelTouched:(id)sender
{
  [self dismissViewControllerAnimated:YES completion:nil];
}

-(AppDelegate*) appDelegate
{
  return (AppDelegate*)[[UIApplication sharedApplication] delegate];
}

-(IBAction) onDownloadTouched: (id) sender {

  NSString* url = self.urlText.text;
  if (!url.length) {
    [self showAlertDialogWithTitle:@"Missing URL" message:@"Please enter a URL."];
    return;
  }

  [[NSUserDefaults standardUserDefaults] setObject:url forKey:@"DownloadURL"];
  [[NSUserDefaults standardUserDefaults] synchronize];

  NSString* destDir = [[self appDelegate] createDownloadsDirectory];

  MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:self.view animated:YES withDelay:0.5];
  hud.labelText = @"Downloading URL...";
  dispatch_async(dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{

    //NSLog(@"Downloading: %@", url);
    vesKiwiCurlDownloader downloader;
    std::string result = downloader.downloadUrlToDirectory([url UTF8String], [destDir UTF8String]);
    std::string errorTitle = downloader.errorTitle();
    std::string errorMessage = downloader.errorMessage();

    dispatch_async(dispatch_get_main_queue(), ^{
      [MBProgressHUD hideHUDForView:self.view animated:YES];
      if (result.size()) {
        NSDictionary* args = [NSDictionary dictionaryWithObjectsAndKeys:[NSString stringWithUTF8String:result.c_str()], @"FileName", nil];
        [[NSNotificationCenter defaultCenter] postNotificationName:@"OpenFile" object:nil userInfo:args];
        [self onCancelTouched:nil];

      }
      else {
        [self showAlertDialogWithTitle:[NSString stringWithUTF8String:errorTitle.c_str()] message:[NSString stringWithUTF8String:errorMessage.c_str()]];
      }

    });
  });

}

@end
