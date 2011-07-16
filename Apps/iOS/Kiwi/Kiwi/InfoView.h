//
//  InfoView.h
//  kiwi
//
//  Created by kitware on 4/21/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface InfoView : UIViewController {
  UILabel *modelInfoLabel;
}
@property (nonatomic, retain) IBOutlet UILabel *modelInfoLabel;
-(void)updateModelInfoLabelWithNumFacets:(int)numFacets 
                            withNumLines:(int)numLines
                         withNumVertices:(int)numVertices;
-(IBAction)kitwareDotCom:(UIButton*)sender;

@end
