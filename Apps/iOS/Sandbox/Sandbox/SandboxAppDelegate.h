//
//  SandboxAppDelegate.h
//  Sandbox
//
//  Created by pat on 7/12/11.
//  Copyright 2011 Kitware. All rights reserved.
//

#import <UIKit/UIKit.h>

@class RootViewController;

@interface SandboxAppDelegate : NSObject <UIApplicationDelegate> {

}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@property (nonatomic, retain) IBOutlet RootViewController *viewController;

@end
