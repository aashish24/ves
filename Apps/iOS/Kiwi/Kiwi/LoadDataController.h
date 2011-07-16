//
//  LoadDataController.h
//  kiwi
//
//  Created by BRADLEY DAVIS on 5/19/11.
//  Copyright 2011 Kitware, Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol LoadDataDelegate
-(void)dataSelected:(NSURL*) url;
@end

@interface LoadDataController : UITableViewController {
  NSMutableArray *_exampleData;
  NSMutableArray *_helpStrings;
  id<LoadDataDelegate> _delegate;
}

@property (nonatomic, retain) NSMutableArray *exampleData;
@property (nonatomic, retain) NSMutableArray *helpStrings;
@property (nonatomic, assign) id<LoadDataDelegate> delegate;
@end
