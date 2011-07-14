//
//  RootViewController.h
//  Sandbox
//
//  Created by pat on 7/13/11.
//  Copyright 2011 Kitware. All rights reserved.
//

#import <UIKit/UIKit.h>

@class ESRenderViewController;
@class SceneController;

@interface RootViewController : UIViewController {

  SceneController* scene;
}

@property (nonatomic, retain) IBOutlet ESRenderViewController *renderViewController;

-(IBAction)onResetViewTouched:(id)sender;

@end
