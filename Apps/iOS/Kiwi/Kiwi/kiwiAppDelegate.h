/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    kiwiAppDelegate.h
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#import <UIKit/UIKit.h>
#import "LoadDataController.h"

@class EAGLView;

@interface kiwiAppDelegate : NSObject <UIApplicationDelegate,LoadDataDelegate> {
  UIWindow *window;
  EAGLView *glView;

  LoadDataController *_dataLoader;
  UIPopoverController *_loadDataPopover;
  
	BOOL isHandlingCustomURLVTKDownload;
  NSURLConnection *downloadConnection;
  NSMutableData *downloadedFileContents;
  BOOL downloadCancelled;
  NSString *nameOfDownloadedVTK;
	NSFileHandle *file;
  UIAlertView *downloadAlert;
}
@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet EAGLView *glView;
@property (nonatomic, retain) LoadDataController *dataLoader;
@property (nonatomic, retain) UIPopoverController *loadDataPopover;

-(IBAction)reset:(UIButton*)sender;
-(IBAction)information:(UIButton*)sender;
-(IBAction)setLoadDataButtonTapped:(id)sender;

// Custom file download methods
- (BOOL)handleCustomURLScheme:(NSURL *)url;
- (void)downloadCompleted;
- (void)saveFileWithData:(NSData *)fileData toFilename:(NSString *)filename;

// Status update methods
- (void)showStatusIndicator;
- (void)showDownloadIndicator;
- (void)updateStatusIndicator;
- (void)hideStatusIndicator;
@end

