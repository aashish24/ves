/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

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
