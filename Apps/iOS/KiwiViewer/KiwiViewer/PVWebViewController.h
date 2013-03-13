//
//  PVWebViewController.h
//  KiwiMidas
//
//  Created by Pat Marion on 12/5/12.
//  Copyright (c) 2012 Pat Marion. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface PVWebViewController : UIViewController <UITextFieldDelegate>


-(IBAction) onHelpTouched: (id) sender;
-(IBAction) onConnectTouched: (id) sender;
-(IBAction) onCancelTouched: (id) sender;

-(void) showError;

@property IBOutlet UITextField *sessionText;

@end
