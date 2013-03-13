//
//  MidasLoginViewController.h
//  CloudAppTab
//
//  Created by Pat Marion on 9/29/12.
//  Copyright (c) 2012 Pat Marion. All rights reserved.
//

#import <UIKit/UIKit.h>

#include <vesMidasClient.h>

@interface MidasLoginViewController : UIViewController <UITextFieldDelegate> {


}


-(IBAction) onLoginTouched: (id) sender;
-(IBAction) onCreateAccount: (id) sender;

@property (assign) vesMidasClient::Ptr client;
@property (nonatomic, retain) IBOutlet UITextField *userText;
@property (nonatomic, retain) IBOutlet UITextField *passwordText;


@end
