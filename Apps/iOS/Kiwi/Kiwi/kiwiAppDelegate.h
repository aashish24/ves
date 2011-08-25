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
