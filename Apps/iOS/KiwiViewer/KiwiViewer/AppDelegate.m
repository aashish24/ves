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

#import "AppDelegate.h"

#import <DropboxSDK/DropboxSDK.h>
#import "DropboxConfig.h"

@interface AppDelegate ()  <DBSessionDelegate, DBNetworkRequestDelegate>  {

	NSString *relinkUserId;

}
@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{

  NSString* defaultSettingsFile = [[NSBundle mainBundle] pathForResource:@"DefaultSettings" ofType:@"plist"];
  NSDictionary* defaultSettings = [NSDictionary dictionaryWithContentsOfFile:defaultSettingsFile];
  [[NSUserDefaults standardUserDefaults] registerDefaults:defaultSettings];

  [self initDropbox];
  
  NSURL *url = (NSURL *)[launchOptions valueForKey:UIApplicationLaunchOptionsURLKey];
  [self handleURL:url];
  return YES;
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
  [self handleURL:url];
  return YES;
}

-(NSString*) maybeCopyDataFile:(NSString*)filename
{
  bool copyFile = false;

  NSFileManager* fileManager = [NSFileManager defaultManager];
  if (![fileManager isWritableFileAtPath:[filename stringByDeletingLastPathComponent]]) {
    copyFile = true;
  }

  if (copyFile) {
    NSString* baseName = [filename lastPathComponent];
    NSString* downloadDir =  [self createDownloadsDirectory];
    NSString* destFile = [downloadDir stringByAppendingPathComponent:baseName];
    [fileManager copyItemAtPath:filename toPath:destFile error:nil];
    return destFile;
  }
  return filename;
}

-(void)handleURL:(NSURL*)url
{
  if (!url) {
    return;
  }

  if ([url isFileURL]) {
    NSString* filename = [url path];
    filename = [self maybeCopyDataFile:filename];
    NSDictionary* args = [NSDictionary dictionaryWithObjectsAndKeys:filename, @"FileName", nil];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"OpenFile" object:nil userInfo:args];
  }
  else if ([url.scheme hasPrefix:@"http"]) {
    [[NSUserDefaults standardUserDefaults] setObject:[url absoluteString] forKey:@"DownloadURL"];
    [[NSUserDefaults standardUserDefaults] synchronize];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"Download URL" object:nil userInfo:nil];
  }
  else if ([url.scheme isEqualToString:@"kiwiviewer"]) {
    NSString* downloadUrl = [[url absoluteString] stringByReplacingOccurrencesOfString:@"kiwiviewer://" withString:@"http://"];
    [[NSUserDefaults standardUserDefaults] setObject:downloadUrl forKey:@"DownloadURL"];
    [[NSUserDefaults standardUserDefaults] synchronize];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"Download URL" object:nil userInfo:nil];
  }
  
  if ([[DBSession sharedSession] handleOpenURL:url]) {
		if ([[DBSession sharedSession] isLinked]) {
      [[NSNotificationCenter defaultCenter] postNotificationName:@"DropboxLinked" object:nil userInfo:nil];
		}
    else {
      [[NSNotificationCenter defaultCenter] postNotificationName:@"DropboxUnlinked" object:nil userInfo:nil];
    }
	}

}


-(NSString*) documentsDirectory
{
  NSArray* documentDirectories = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  return [documentDirectories objectAtIndex:0];
}

-(NSString*)downloadsDirectory
{
  return [[self documentsDirectory] stringByAppendingPathComponent:@"Downloads"];
}

-(NSString*)createDownloadsDirectory
{
  NSString* downloadsDir = [self downloadsDirectory];
  
  if (![[NSFileManager defaultManager] fileExistsAtPath:downloadsDir]) {

    if ([[NSFileManager defaultManager] createDirectoryAtPath:downloadsDir
        withIntermediateDirectories:NO attributes:nil error:nil]) {

      BOOL success = [[NSURL fileURLWithPath:downloadsDir isDirectory:YES] setResourceValue:[NSNumber numberWithBool:YES]
                      forKey: NSURLIsExcludedFromBackupKey error:nil];
      if (!success) {
        NSLog(@"Error setting NSURLIsExcludedFromBackupKey attribute.");
      }
    }
    else {
      NSLog(@"Error creating Downloads directory.");
    }
  }

  return downloadsDir;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}



-(void) initDropbox
{
  NSString* appKey =  KIWIVIEWER_DROPBOX_APP_KEY;
	NSString* appSecret = KIWIVIEWER_DROPBOX_APP_SECRET;
	NSString *root = kDBRootAppFolder; // or kDBRootDropbox

	DBSession* session = [[DBSession alloc] initWithAppKey:appKey appSecret:appSecret root:root];
	session.delegate = self;
	[DBSession setSharedSession:session];
	
	[DBRequest setNetworkRequestDelegate:self];
}

#pragma mark -
#pragma mark DBSessionDelegate methods

- (void)sessionDidReceiveAuthorizationFailure:(DBSession*)session userId:(NSString *)userId
{
  [[NSNotificationCenter defaultCenter] postNotificationName:@"DropboxAuthorizationFailure" object:nil userInfo:nil];
}


static int outstandingRequests;

- (void)networkRequestStarted
{
	outstandingRequests++;
	if (outstandingRequests == 1) {
		[[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:YES];
	}
}

- (void)networkRequestStopped
{
	outstandingRequests--;
	if (outstandingRequests == 0) {
		[[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO];
	}
}


@end
