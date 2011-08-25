
//
//  InfoView.m
//  kiwi
//
//  Created by kitware on 4/21/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "InfoView.h"


@implementation InfoView

@synthesize modelInfoLabel;
@synthesize hideButton;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

-(IBAction)kitwareDotCom:(UIButton*)sender
{
  [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"http://www.kitware.com"]];
  
}

- (void)updateModelInfoLabelWithNumFacets:(int)numFacets withNumLines:(int)numLines withNumVertices:(int)numVertices withCurrentRefreshRate:(int)refreshRate
{
  modelInfoLabel.text = [NSString stringWithFormat:@"Current Mesh:\n   Triangles: %i\n   Lines: %i\n   Vertices: %i\n   Drawing @ %dHz", numFacets, numLines, numVertices, refreshRate];
}

- (void)dealloc
{
  [modelInfoLabel release];
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
    // Do any additional setup after loading the view from its nib.
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
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (IBAction) hideView
{
  self.view.window.rootViewController = nil;
}

@end
