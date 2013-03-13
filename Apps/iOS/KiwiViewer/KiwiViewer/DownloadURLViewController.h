//
//  DownloadURLViewController.h
//  KiwiMidas
//
//  Created by Pat Marion on 12/5/12.
//  Copyright (c) 2012 Pat Marion. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface DownloadURLViewController : UIViewController <UITextFieldDelegate>


-(IBAction) onHelpTouched: (id) sender;
-(IBAction) onDownloadTouched: (id) sender;
-(IBAction) onCancelTouched: (id) sender;

-(void) showError;

@property IBOutlet UITextField *urlText;

@end
