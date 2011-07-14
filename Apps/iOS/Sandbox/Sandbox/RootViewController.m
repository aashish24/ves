//
//  RootViewController.m
//  Sandbox
//
//  Created by pat on 7/13/11.
//  Copyright 2011 Kitware. All rights reserved.
//

#import "RootViewController.h"

#import "ESRenderViewController.h"
#import "SceneController.h"


@implementation RootViewController

@synthesize renderViewController;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)dealloc
{
  [self->scene release];
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
  [super viewDidLoad];
  
  self->scene = [SceneController new];
  self->scene.renderView = self.renderViewController.renderView;
  [self->scene initializeScene];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
	return YES;
}

-(IBAction)onResetViewTouched:(id)sender
{
  [self->scene resetView];
}

@end
