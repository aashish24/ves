/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    kiwiAppDelegate.m
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#import "kiwiAppDelegate.h"
#import "EAGLView.h"
#import "InfoView.h"

@implementation kiwiAppDelegate

@synthesize window;
@synthesize glView;
@synthesize dataLoader = _dataLoader;
@synthesize loadDataPopover = _loadDataPopover;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions   
{
	NSURL *url = (NSURL *)[launchOptions valueForKey:UIApplicationLaunchOptionsURLKey];
	[self handleCustomURLScheme:url];
  return YES;
}

- (void)dealloc
{
  self.dataLoader = nil;
  self.loadDataPopover = nil;
  
  [window release];
  [glView release];  
  [super dealloc];
}

- (void)applicationWillResignActive:(UIApplication *)application
{
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
}

- (void)applicationWillTerminate:(UIApplication *)application
{
}

-(IBAction)reset:(UIButton*)sender
{
  [glView resetView];
}


-(IBAction)information:(UIButton*)sender
{
  InfoView *infoView = [[InfoView alloc] init];
  UIPopoverController *popover = [[UIPopoverController alloc] initWithContentViewController:infoView];
  
  // need to get the info from the renderer
  [infoView updateModelInfoLabelWithNumFacets:[self.glView getNumberOfFacetsForCurrentModel]
                                 withNumLines:[self.glView getNumberOfLinesForCurrentModel]
                              withNumVertices:[self.glView getNumberOfVerticesForCurrentModel]
                                withCurrentRefreshRate:[self.glView currentRefreshRate]];
  [popover presentPopoverFromRect:CGRectMake(570,785,300,200) 
                           inView:self.glView 
         permittedArrowDirections:(//UIPopoverArrowDirectionUp
                                   //UIPopoverArrowDirectionLeft
                                   //&
                                   UIPopoverArrowDirectionDown)
//                                   &UIPopoverArrowDirectionRight) 
                         animated:NO];
 // [popover  setPopoverContentSize:<#(CGSize)#> animated:<#(BOOL)#>
  //[popover  dismissPopoverAnimated:YES];
	[popover setPopoverContentSize:CGSizeMake(300,200) animated:NO];
  //[popover setContentViewController:infoView animated:NO];
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url
{
	if (url != nil)
	{
		isHandlingCustomURLVTKDownload = YES;
	}
	// Handle the VTKs custom URL scheme
	[self handleCustomURLScheme:url];
	return YES;
}

- (BOOL)handleCustomURLScheme:(NSURL *)url;
{
  //
  // if viewer is not available: bail out
  if (!glView)
  {
    return NO;
  }
  
  //
  // no url; go with the default surface
  if (!url)
  {
    NSLog(@"Null url; opening default data file");
    [glView setFilePath:[[NSBundle mainBundle] pathForResource:@"current" ofType:@"stl"]];
    return YES;
  }
  
  NSLog(@"Opening URL: %@", url);
  
  //
  // we already have a file on the device (e.g., from dropbox); use it
  if ([url isFileURL])
  {
    [glView setFilePath:[url path]];
    return YES;
  }
  
  isHandlingCustomURLVTKDownload = YES;
  
  downloadAlert = [[[UIAlertView alloc] initWithTitle:@"Downloading File\nPlease Wait..." message:nil delegate:self cancelButtonTitle:nil otherButtonTitles: nil] autorelease];
  [downloadAlert show];
  UIActivityIndicatorView *indicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
  // Adjust the indicator so it is up a few pixels from the bottom of the alert
  indicator.center = CGPointMake(downloadAlert.bounds.size.width / 2, downloadAlert.bounds.size.height - 50);
  [indicator startAnimating];
  [downloadAlert addSubview:indicator];
  [indicator release];
  
  NSString *pathComponentForCustomURL = [[url host] stringByAppendingString:[url path]];
  NSString *locationOfRemoteVTKFile = [NSString stringWithFormat:@"http://%@", pathComponentForCustomURL];
  //nameOfDownloadedVTK = @"current.vtk";//[[pathComponentForCustomURL lastPathComponent] retain];
  nameOfDownloadedVTK = [[pathComponentForCustomURL lastPathComponent] retain];
  
  // Check to make sure that the file has not already been downloaded, if so, just switch to it
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSString *documentsDirectory = [paths objectAtIndex:0];
  
  NSLog(@"path = %@",pathComponentForCustomURL);
  NSLog(@"loc = %@",locationOfRemoteVTKFile);
  NSLog(@"name = %@",nameOfDownloadedVTK);
  NSLog(@"paths = %@",paths);
  NSLog(@"docDir = %@",documentsDirectory);
  
  downloadCancelled = NO;
  
  // Start download of new file
  [self showDownloadIndicator];
  
  [[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:YES];
  //filename = [documentsDirectory stringByAppendingPathComponent:nameOfDownloadedVTK];
  
  NSURLRequest *theRequest=[NSURLRequest requestWithURL:[NSURL URLWithString:locationOfRemoteVTKFile]
                                            cachePolicy:NSURLRequestUseProtocolCachePolicy
                                        timeoutInterval:60.0f];
  
  
  downloadConnection = [[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
  if (downloadConnection)
  {
    downloadedFileContents = [[NSMutableData data] retain];
  }
  else
    {
    // inform the user that the download could not be made
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:NSLocalizedStringFromTable(@"Could not open connection", @"Localized", nil) message:[NSString stringWithFormat:NSLocalizedStringFromTable(@"Could not open connection to: %@", @"Localized", nil), nameOfDownloadedVTK]
      delegate:self cancelButtonTitle:NSLocalizedStringFromTable(@"OK", @"Localized", nil) otherButtonTitles: nil, nil];
    [alert show];
    [alert release];		
    return NO;
    }
  
	return YES;
}

#pragma mark -
#pragma mark URL connection delegate methods

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error;
{
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:NSLocalizedStringFromTable(@"Connection failed", @"Localized", nil) message:NSLocalizedStringFromTable(@"Could not download file", @"Localized", nil)
    delegate:self cancelButtonTitle:NSLocalizedStringFromTable(@"OK", @"Localized", nil) otherButtonTitles: nil, nil];
	[alert show];
	[alert release];
	
	[self downloadCompleted];
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data;
{
	// Concatenate the new data with the existing data to build up the downloaded file
	// Update the status of the download
	
	if (downloadCancelled)
	{
		[connection cancel];
		[self downloadCompleted];
		downloadCancelled = NO;
		return;
	}
	[downloadedFileContents appendData:data];
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response;
{
  // This extra check was causing some files from the web not to be loaded.
  /*
	// Stop the spinning wheel and start the status bar for download
	if ([response textEncodingName] != nil)
	{
		UIAlertView *alert = [[UIAlertView alloc] initWithTitle:NSLocalizedStringFromTable(@"Could not find file", @"Localized", nil) message:[NSString stringWithFormat:NSLocalizedStringFromTable(@"No such file exists on the server: %@", @"Localized", nil), nameOfDownloadedVTK]
                                                   delegate:self cancelButtonTitle:NSLocalizedStringFromTable(@"OK", @"Localized", nil) otherButtonTitles: nil, nil];
		[alert show];
		[alert release];		
		[connection cancel];
		[self downloadCompleted];
		return;
	}
  */
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection;
{
	// Close off the file and write it to disk
	[self saveFileWithData:downloadedFileContents toFilename:nameOfDownloadedVTK];
	[self downloadCompleted];
}

- (void)saveFileWithData:(NSData *)fileData toFilename:(NSString *)filename;
{
	if (fileData != nil)
    {
    // Add the new protein to the list by gunzipping the data and pulling out the title
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
		
    NSError *error = nil;
    BOOL writeStatus;
    writeStatus = [fileData writeToFile:[documentsDirectory stringByAppendingPathComponent:filename] options:NSAtomicWrite error:&error];
		
    if (!writeStatus)
      {
      UIAlertView *alert = [[UIAlertView alloc] initWithTitle:NSLocalizedStringFromTable(@"Could not write file", @"Localized", nil) message:[NSString stringWithFormat:NSLocalizedStringFromTable(@"Could not write file: %@", @"Localized", nil), nameOfDownloadedVTK]
        delegate:self cancelButtonTitle:NSLocalizedStringFromTable(@"OK", @"Localized", nil) otherButtonTitles: nil, nil];
      [alert show];
      [alert release];		
      return;
      }
		
    NSString *filePath = [documentsDirectory stringByAppendingPathComponent:filename];
    if(glView)
      {
      [glView setFilePath:filePath];
      }
    }
}

- (void)downloadCompleted;
{
	[downloadConnection release];
	downloadConnection = nil;
	[[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO];
	
	[downloadedFileContents release];
	downloadedFileContents = nil;
	[self hideStatusIndicator];
  [downloadAlert dismissWithClickedButtonIndex:0 animated:YES];
	[nameOfDownloadedVTK release];
	nameOfDownloadedVTK = nil;
}

#pragma mark -
#pragma mark Status update methods

- (void)showStatusIndicator;
{
	[[NSNotificationCenter defaultCenter] postNotificationName:@"FileLoadingStarted" object:NSLocalizedStringFromTable(@"Initializing database...", @"Localized", nil)];
}

- (void)showDownloadIndicator;
{
	[[NSNotificationCenter defaultCenter] postNotificationName:@"FileLoadingStarted" object:NSLocalizedStringFromTable(@"Downloading vtk file...", @"Localized", nil)];
}

- (void)updateStatusIndicator;
{
	
}

- (void)hideStatusIndicator;
{
	[[NSNotificationCenter defaultCenter] postNotificationName:@"FileLoadingEnded" object:nil];
}

-(void)dataSelected:(NSURL*)url {
  [self handleCustomURLScheme:url];
  [self.loadDataPopover dismissPopoverAnimated:YES];
}

-(IBAction)setLoadDataButtonTapped:(id)sender {
  if (_dataLoader == nil) {
    self.dataLoader = [[[LoadDataController alloc]
                        initWithStyle:UITableViewStyleGrouped] autorelease];
    _dataLoader.delegate = self;
    self.loadDataPopover = [[[UIPopoverController alloc]
                             initWithContentViewController:_dataLoader] autorelease];
  }
  [self.loadDataPopover presentPopoverFromRect:CGRectMake(515,960,300,200) 
                                        inView:self.glView permittedArrowDirections:UIPopoverArrowDirectionDown animated:YES];
}
@end
