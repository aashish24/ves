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

#import "DocumentsTableViewController.h"
#import "AppDelegate.h"
#import "MidasLoginViewController.h"
#import "MBProgressHUD.h"

#include <QuartzCore/CALayer.h>

// Dropbox support
#import <DropboxSDK/DropboxSDK.h>


#include <vesKiwiCurlDownloader.h>
#include <vtkNew.h>
#include <vtkDirectory.h>

namespace {
  class MyProgressDelegate : public vesKiwiCurlDownloader::ProgressDelegate {
    public:

      MyProgressDelegate() : shouldAbort(false), totalBytes(0), progressView(nil)
      {
      }

      virtual int downloadProgress(double totalToDownload, double nowDownloaded)
      {

        double progress = nowDownloaded/this->totalBytes;

        dispatch_async(dispatch_get_main_queue(), ^{
          progressView.progress = progress;
        });

        if (shouldAbort) {
          return 1;
        }
        else {
          return 0;
        }
      }

    bool shouldAbort;
    double totalBytes;
    std::string itemId;
    UIProgressView* progressView;
    NSIndexPath* itemPath;

    vesSharedPtr<vesKiwiCurlDownloader> downloader;
  };
}

@interface DocumentsTableViewController () <DBRestClientDelegate> {

  int foldersSection;
  int userFoldersSection;
  int communityFoldersSection;
  int communitiesSection;
  int localFilesSection;
  int appSection;
  int numberOfSections;

  bool gotoMidasDocs;
  bool populateViewComplete;

  std::vector<std::string> folderNames;
  std::vector<std::string> folderIds;
  std::vector<std::string> itemNames;
  std::vector<std::string> itemIds;
  std::vector<size_t> itemBytes;
  std::vector<std::string> communityNames;
  std::vector<std::string> communityIds;
  std::vector<std::string> localFiles;
  std::vector<size_t> localFilesSize;
  std::vector<std::string> localFolders;
  std::vector<std::string> appNames;

  vesSharedPtr<MyProgressDelegate> mMidasDownloader;

  // Dropbox support
  DBRestClient* restClient;
  NSString* pathHash;
  UIProgressView* dropboxDownloadProgress;
  bool dropboxShouldAbort;

  MBProgressHUD* downloadHUD;
}

@end

@implementation DocumentsTableViewController

@synthesize showMidasCommunities;
@synthesize showMidasTopLevelFolders;
@synthesize showDropbox;
@synthesize client;
@synthesize folderId;
@synthesize communityId;
@synthesize localFolder;

- (id)initWithStyle:(UITableViewStyle)style
{
  self = [super initWithStyle:style];
  if (self) {
    self->gotoMidasDocs = NO;
  }
  return self;
}

-(void) dealloc
{
  [self stopSpinner];
}

- (void)listDropboxFiles
{
  if (![[DBSession sharedSession] isLinked]) {
    return;
  }

  [self startSpinner];

  [self.restClient loadMetadata:self.localFolder withHash:self->pathHash];
}

- (void)restClient:(DBRestClient*)client loadedMetadata:(DBMetadata*)metadata
{
  self->pathHash = metadata.hash;


  for (DBMetadata* child in metadata.contents) {
    if (child.isDirectory) {
      self->localFolders.push_back([[child.path lastPathComponent] UTF8String]);
    }
    else {
      self->localFiles.push_back([[child.path lastPathComponent] UTF8String]);
      self->localFilesSize.push_back(child.totalBytes);
    }
  }

  self->numberOfSections = 1;
  self->localFilesSection = 0;
  self->populateViewComplete = true;

  [self.tableView reloadData];
  [self maybeAddLogoutButton];
  [self stopSpinner];

  if ([self.localFolder isEqualToString:@"/"] && !self->localFiles.size() && !self->localFolders.size()) {
    NSString* message = @"Your Dropbox app folder is empty.";
    if (UI_USER_INTERFACE_IDIOM()==UIUserInterfaceIdiomPhone) {
      message = @"You app folder is empty.";
    }
    [self showMessage:message withTimeout:2.0];
  }

}

- (void)restClient:(DBRestClient*)client metadataUnchangedAtPath:(NSString*)path
{
  [self stopSpinner];
}

- (void)restClient:(DBRestClient*)client loadMetadataFailedWithError:(NSError*)error
{
  [self stopSpinner];
  //NSLog(@"restClient:loadMetadataFailedWithError: %@", [error localizedDescription]);

  NSString* errorMessage = [error.userInfo objectForKey:@"error"];
  if (!errorMessage) {
    errorMessage = @"There was an error listing the folder.";
  }
  
  [self showAlertDialogWithTitle:@"Dropbox Request Failed" message:errorMessage];
}

-(void) downloadFileFromDropbox:(NSString*)dropboxPath
{

  if (![[DBSession sharedSession] isLinked]) {
    return;
  }

  self->dropboxDownloadProgress = [self createDownloadHUD];

  NSString* filename = [dropboxPath lastPathComponent];
  NSString* destFile = [[[self appDelegate] createDownloadsDirectory] stringByAppendingPathComponent:filename];
  [[self restClient] loadFile:dropboxPath intoPath:destFile];
}

- (void)restClient:(DBRestClient*)client loadedFile:(NSString*)localPath
{
  [self hideDownloadHUDAnimated:NO];
  self->dropboxDownloadProgress = nil;

  [self openFile:localPath];
}

- (void)restClient:(DBRestClient*)client loadFileFailedWithError:(NSError*)error
{
  [self abortDropboxDownload];

  NSString* errorMessage = [error.userInfo objectForKey:@"error"];
  if (!errorMessage) {
    errorMessage = @"There was an error downloading the file.";
  }
  
  [self showAlertDialogWithTitle:@"Dropbox Request Failed" message:errorMessage];
  
  //NSLog(@"There was an error loading the file - %@", error);
}

- (void)restClient:(DBRestClient*)client loadProgress:(CGFloat)progress forFile:(NSString*)destPath
{
  self->dropboxDownloadProgress.progress = progress;
}

-(void) abortDropboxDownload
{
  [self->restClient cancelAllRequests];
  [self hideDownloadHUDAnimated:YES];
  self->dropboxDownloadProgress = nil;
}

-(void) unlinkDropbox
{
  [[DBSession sharedSession] unlinkAll];
  [self onDropboxUnlinked];
}

-(void) linkDropbox
{
  if (![[DBSession sharedSession] isLinked]) {
    [[DBSession sharedSession] linkFromController:self];
  }
}

-(void) handleDropboxAuthorizationFailure
{
  [self stopSpinner];
  [self unlinkDropbox];
}

- (DBRestClient*)restClient {
  if (self->restClient == nil) {
      self->restClient = [[DBRestClient alloc] initWithSession:[DBSession sharedSession]];
      self->restClient.delegate = self;
  }
  return restClient;
}

-(void) handleDropboxSelected
{
  if ([[DBSession sharedSession] isLinked]) {
    [self handleDropboxFolderSelected:@"/"];
  }
  else {
    [self linkDropbox];
  }
}

-(void)listLocalFiles:(NSString*) baseDir
{
  vtkNew<vtkDirectory> dirList;
  dirList->Open([baseDir UTF8String]);

  self->localFiles.clear();
  self->localFolders.clear();

  for (int i = 0; i < dirList->GetNumberOfFiles(); ++i) {
    std::string entryName = dirList->GetFile(i);

    if (!entryName.size()) {
      continue;
    }
    if (entryName.substr(0,1) == ".") {
      continue;
    }
    if (entryName == "__MACOSX") {
      continue;
    }


    if (dirList->FileIsDirectory(entryName.c_str())) {
      self->localFolders.push_back(entryName);
    }
    else {
      self->localFiles.push_back(entryName);

      NSString* absPath = [baseDir stringByAppendingPathComponent:[NSString stringWithUTF8String:entryName.c_str()]];
      NSDictionary *fileAttributes = [[NSFileManager defaultManager] attributesOfItemAtPath:absPath error:nil];
      self->localFilesSize.push_back([fileAttributes fileSize]);
    }
  }
}

-(NSString*)sampleDataDirectory
{
  return [[[self appDelegate] documentsDirectory] stringByAppendingPathComponent:@"Sample Data"];
}

-(NSString*)copySampleData:(NSString*)filename
{
  NSString* destDir = [self sampleDataDirectory];

  if (![[NSFileManager defaultManager] fileExistsAtPath:destDir]) {
    [[NSFileManager defaultManager] createDirectoryAtPath:destDir withIntermediateDirectories:NO attributes:nil error:nil];
  }

  NSString* srcFile = [[NSBundle mainBundle] pathForResource:filename ofType:nil];
  NSString* destFile = [destDir stringByAppendingPathComponent:filename];
  if (srcFile && [[NSFileManager defaultManager] copyItemAtPath:srcFile toPath:destFile error:nil]) {
    return destFile;
  }

  return nil;
}

-(void) unpackSampleData
{
  bool doUnpack = [[NSUserDefaults standardUserDefaults] boolForKey:@"UnpackSampleData"];
  if (!doUnpack) {
    return;
  }

  std::vector<std::string> sampleData;
  sampleData.push_back("Can Simulation.zip");
  sampleData.push_back("Kinect Point Cloud.pcd");
  sampleData.push_back("Molecule PDB-7WGA.pdb");
  sampleData.push_back("NA-MIC Knee Atlas.vtp");
  sampleData.push_back("NASA Blue Marble.zip");
  sampleData.push_back("NLM Visible Human Project.vti");
  sampleData.push_back("PHASTA Blood Flow Sim.zip");
  sampleData.push_back("SPL-PNL Brain Atlas.zip");
  sampleData.push_back("Velodyne Lidar.zip");

  for (size_t i = 0; i < sampleData.size(); ++i) {
    [self copySampleData:[NSString stringWithUTF8String:sampleData[i].c_str()]];
  }

  [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"UnpackSampleData"];
  [[NSUserDefaults standardUserDefaults] synchronize];
}

-(void) clearMidasTables
{
  self->folderNames.clear();
  self->folderIds.clear();
  self->communityNames.clear();
  self->communityIds.clear();
  self->itemNames.clear();
  self->itemIds.clear();
  self->itemBytes.clear();

  [self.tableView reloadData];
}

-(void) popluateView
{
  [self startSpinner];

  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{

    int nSections = [self populateViewThread];

    dispatch_async(dispatch_get_main_queue(), ^{
      self->numberOfSections = nSections;
      self->populateViewComplete = true;
      [self.tableView reloadData];
      [self maybeAddLogoutButton];
      [self stopSpinner];
    });
  });
}

-(int) populateViewThread
{

  int nSections = 0;

  if (self.showMidasTopLevelFolders) {

    self->userFoldersSection = nSections;
    ++nSections;

    if (self->client->token().size()) {

      self->client->listUserFolders();
      self->folderNames = self->client->folderNames();
      self->folderIds = self->client->folderIds();
    }
  }
  if (self.showMidasCommunities) {


    self->communitiesSection = nSections;
    ++nSections;

    self->client->listCommunities();
    self->communityNames = self->client->folderNames();
    self->communityIds = self->client->folderIds();
  }
  else if (self.folderId) {

    self->foldersSection = nSections;
    ++nSections;

    self->client->listFolderChildren([self.folderId UTF8String]);
    self->folderNames = self->client->folderNames();
    self->folderIds = self->client->folderIds();
    self->itemNames = self->client->itemNames();
    self->itemIds = self->client->itemIds();
    self->itemBytes = self->client->itemBytes();
  }
  else if (self.communityId) {

    self->communityFoldersSection = nSections;
    ++nSections;

    self->client->listCommunityChildren([self.communityId UTF8String]);
    self->folderNames = self->client->folderNames();
    self->folderIds = self->client->folderIds();
  }
  else if (self.localFolder) {

    self->localFilesSection = nSections;
    ++nSections;
    [self listLocalFiles:self.localFolder];
  }
  else {

    // top level navigation

    self->localFilesSection = nSections;
    ++nSections;
    self->localFolder = [[self appDelegate] documentsDirectory];
    [self unpackSampleData];
    [self listLocalFiles:self.localFolder];

    const bool enableApps = true;
    if (enableApps) {
      self->appSection = nSections;
      ++nSections;
      self->appNames.push_back("Midas");
      self->appNames.push_back("Dropbox");
      self->appNames.push_back("Download URL");
      self->appNames.push_back("ParaView Web");
      self->appNames.push_back("ParaView Mobile Remote");
      self->appNames.push_back("Point Cloud Streaming");
    }
  }

  return nSections;
}


- (void)viewDidLoad
{
  [super viewDidLoad];

  if (!self->client) {
    client = vesMidasClient::Ptr(new vesMidasClient);
    NSString* midasHost = [[NSUserDefaults standardUserDefaults] stringForKey:@"MidasHost"];
    self.client->setHost([midasHost UTF8String]); 
  }


  self->foldersSection = -1;
  self->userFoldersSection = -1;
  self->communityFoldersSection = -1;
  self->communitiesSection = -1;
  self->localFilesSection = -1;
  self->appSection = -1;
  self->numberOfSections = 0;
  self->populateViewComplete = false;
  self->downloadHUD = nil;


  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onMidasHostChanged) name:@"MidasHostChanged" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onDropboxLinked) name:@"DropboxLinked" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onDropboxUnlinked) name:@"DropboxUnlinked" object:nil];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleDropboxAuthorizationFailure) name:@"DropboxAuthorizationFailure" object:nil];

  [self addGestureRecognizers];

  if (self.showDropbox) {
    [self listDropboxFiles];
  }
  else {
    [self popluateView];
  }
}

-(void)addGestureRecognizers
{
  // Long press gesture recognizer to initiate editing
  UILongPressGestureRecognizer *longPress = [[UILongPressGestureRecognizer alloc]
    initWithTarget:self action:@selector(handleLongPress:)];
  longPress.minimumPressDuration = 1.0; //seconds
  longPress.delegate = self;
  [self.tableView addGestureRecognizer:longPress];
}

-(void)onStopEditing
{
  [self setEditing:NO animated:YES];
  self.navigationItem.rightBarButtonItem = nil;
}

-(void)handleLongPress:(UILongPressGestureRecognizer *)gestureRecognizer
{
  CGPoint p = [gestureRecognizer locationInView:self.tableView];

  NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint:p];
  if (indexPath.section == self->localFilesSection && !self.showDropbox) {
    [self setEditing:YES animated:YES];

    // add done button
    UIBarButtonItem *doneButton = [[UIBarButtonItem alloc]
                                   initWithTitle:@"Done"
                                   style:UIBarButtonItemStyleDone
                                   target:self 
                                   action:@selector(onStopEditing)];
    self.navigationItem.rightBarButtonItem = doneButton;
  }

}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(AppDelegate*) appDelegate
{
  return (AppDelegate*)[[UIApplication sharedApplication] delegate];
}

- (void)showAlertDialogWithTitle:(NSString *)alertTitle message:(NSString *)alertMessage;
{

  UIAlertView *alert = [[UIAlertView alloc]
                        initWithTitle:alertTitle
                        message:alertMessage
                        delegate:self
                        cancelButtonTitle:@"Ok"
                        otherButtonTitles: nil, nil];
  [alert show];
}

-(void)showMessage:(NSString*) message withTimeout:(double)timeout
{
  MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:self.view animated:YES withDelay:0.0];
  hud.mode = MBProgressHUDModeText;
  hud.labelText = message;
  dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, timeout * NSEC_PER_SEC);
  dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
    [MBProgressHUD hideHUDForView:self.view animated:YES];
  });
}

- (void)viewWillAppear:(BOOL)animated
{
  [super viewWillAppear:animated];

  if (self->populateViewComplete && self->localFilesSection >= 0 && !self.showDropbox) {
    [self listLocalFiles:self.localFolder];
    [self.tableView reloadData];
  }
}

- (void)viewDidDisappear:(BOOL)animated
{
  [super viewDidDisappear:animated];
}

-(UIView*) viewForHUD
{
  return self.parentViewController.parentViewController.view;
}

-(void)startSpinner
{
  UIView* view = [self viewForHUD];
  if (![MBProgressHUD HUDForView:view]) {
    [MBProgressHUD showHUDAddedTo:view animated:YES withDelay:0.5];
  }
}

-(void)stopSpinner
{
  UIView* view = [self viewForHUD];
  [MBProgressHUD hideHUDForView:view animated:YES];
}

-(void) onMidasLogout
{
  self.client->reset();
  NSString* midasHost = [[NSUserDefaults standardUserDefaults] stringForKey:@"MidasHost"];
  self.client->setHost([midasHost UTF8String]);  
  [self.navigationController popToRootViewControllerAnimated:YES];
}

-(void) onDropboxLinked
{
  [self handleDropboxFolderSelected:@"/"];
}

-(void) onDropboxUnlinked
{
  [self.navigationController popToRootViewControllerAnimated:YES];
}

-(void)maybeAddLogoutButton
{

  if (self.showDropbox) {
    NSString* buttonText = @"Unlink Dropbox";

    UIBarButtonItem *unlinkButton = [[UIBarButtonItem alloc]
                                   initWithTitle:buttonText
                                   style:UIBarButtonItemStyleBordered 
                                   target:self 
                                   action:@selector(unlinkDropbox)];
    self.navigationItem.rightBarButtonItem = unlinkButton;
  }
  
  if (self->userFoldersSection == -1 && self->foldersSection == -1 && self->communitiesSection == -1 && self->communityFoldersSection == -1) {
    return;
  }

  if (!self->client->token().size()) {
    return;
  }

  NSString* buttonText = @"Sign Out";

  UIBarButtonItem *logoutButton = [[UIBarButtonItem alloc]
                                 initWithTitle:buttonText
                                 style:UIBarButtonItemStyleBordered 
                                 target:self 
                                 action:@selector(onMidasLogout)];
  self.navigationItem.rightBarButtonItem = logoutButton;
}

-(void)onMidasHostChanged
{
  [self onMidasLogout];
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
  return self->numberOfSections;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
  if (section == self->userFoldersSection) {
    return @"Midas Documents";
  }
  else if (section == self->foldersSection) {
    return @"Midas Documents";
  }
  else if (section == self->communityFoldersSection) {
    return @"Community Folders";
  }
  else if (section == self->communitiesSection) {
    return @"Midas Communities";
  }
  else if (section == self->localFilesSection && self.showDropbox) {
    return @"Dropbox Documents";
  }
  else if (section == self->localFilesSection) {
    return @"Local Documents";
  }
  else if (section == self->appSection) {
    return @"Network Data Sources";
  }

  return nil;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
  if (section == self->userFoldersSection) {
    if (!self->client->token().size()) {
      return 1;
    }
    return self->folderNames.size();
  }
  else if (section == self->foldersSection) {
    return self->folderNames.size() + self->itemNames.size();
  }
  else if (section == self->communityFoldersSection) {
    return self->folderNames.size();
  }
  else if (section == self->communitiesSection) {
    return self->communityNames.size();
  }
  else if (section == self->localFilesSection) {
    return self->localFolders.size() + self->localFiles.size();
  }
  else if (section == self->appSection) {
    return self->appNames.size();
  }
  return 0;
}

-(void)setFileSizeSubtitle:(long long)fileSizeInBytes forCell:(UITableViewCell*)cell
{
  double itemKilobytes = fileSizeInBytes/1024.0;
  double itemMegabytes = itemKilobytes/1024.0;
  if (itemMegabytes < 1.0) {
    cell.detailTextLabel.text = [NSString stringWithFormat:@"%.02f KB", itemKilobytes];
  }
  else {
    cell.detailTextLabel.text = [NSString stringWithFormat:@"%.02f MB", itemMegabytes];
  }
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{

  NSString *cellIdentifier = @"DocumentsTableCell";

  UITableViewCell *cell;


  if (indexPath.section == self->userFoldersSection && !self->client->token().size()) {

    cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellIdentifier];
    cell.textLabel.text = @"Sign in to Midas";
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;

  }
  else if (indexPath.section == self->foldersSection || indexPath.section == self->userFoldersSection) {

    int row = indexPath.row;
    int numberOfFolders = self->folderNames.size();

    if (row < numberOfFolders) {
      std::string cellLabel = self->folderNames[row];
      cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellIdentifier];
      cell.textLabel.text = [NSString stringWithUTF8String:cellLabel.c_str()];
      cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    }
    else {
      int itemIndex = row - numberOfFolders;
      std::string cellLabel = self->itemNames[itemIndex];
      cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:cellIdentifier];
      cell.textLabel.text = [NSString stringWithUTF8String:cellLabel.c_str()];
      cell.accessoryType = UITableViewCellAccessoryNone;
      [self setFileSizeSubtitle:self->itemBytes[itemIndex] forCell:cell];
    }
    
  }
  else if (indexPath.section == self->communityFoldersSection) {
    int row = indexPath.row;
    std::string cellLabel = self->folderNames[row];
    cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellIdentifier];
    cell.textLabel.text = [NSString stringWithUTF8String:cellLabel.c_str()];
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
  }
  else if (indexPath.section == self->communitiesSection) {
    int row = indexPath.row;
    std::string cellLabel = self->communityNames[row];
    cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellIdentifier];
    cell.textLabel.text = [NSString stringWithUTF8String:cellLabel.c_str()];
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
  }
  else if (indexPath.section == self->localFilesSection) {

    int row = indexPath.row;
    int numberOfFolders = self->localFolders.size();
  
    if (row < numberOfFolders) {
      std::string cellLabel = self->localFolders[row];
      cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellIdentifier];
      cell.textLabel.text = [NSString stringWithUTF8String:cellLabel.c_str()];
      cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    }
    else {

      int itemIndex = row - numberOfFolders;
      std::string cellLabel = self->localFiles[itemIndex];
      cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:cellIdentifier];
      cell.textLabel.text = [NSString stringWithUTF8String:cellLabel.c_str()];
      cell.accessoryType = UITableViewCellAccessoryNone;
      [self setFileSizeSubtitle:self->localFilesSize[itemIndex] forCell:cell];
    }

  }
  else if (indexPath.section == self->appSection) {
    int row = indexPath.row;
    std::string cellLabel = self->appNames[row];
    cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellIdentifier];
    cell.textLabel.text = [NSString stringWithUTF8String:cellLabel.c_str()];
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
  }
  
  return cell;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
  if (indexPath.section == self->localFilesSection && !self.showDropbox) {

    // Delete local document
    NSString* documentPath = [self pathForLocalDocument:indexPath];
    BOOL success = [[NSFileManager defaultManager] removeItemAtPath:documentPath error:nil];
    if (success) {
      [self listLocalFiles:self->localFolder];
      [self.tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationFade];
    }
    
    if (!self->localFiles.size() && !self->localFolders.size()) {
      [self onStopEditing];
    }
  }

}

- (BOOL)tableView:(UITableView *)tableView shouldIndentWhileEditingRowAtIndexPath:(NSIndexPath *)indexPath
{
  return NO;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
  if (indexPath.section == self->localFilesSection && !self.showDropbox) {
    return YES;
  }
  return NO;
}

-(BOOL)tableView:(UITableView *)tableView shouldHighlightRowAtIndexPath:(NSIndexPath *)indexPath
{
  return YES;
}

-(NSString*) maybeCopyDataFile:(NSString*)filename
{
  bool copyFile = false;
  NSString* inboxDir = [[[self appDelegate] documentsDirectory] stringByAppendingPathComponent:@"Inbox"];
  
  
  // if file is in the Sample Data directory and it ends with .zip, then copy it to the
  // Downloads directory so that archive will not extract into the Sample Data dir.
  if ([filename hasPrefix:[self sampleDataDirectory]] && [filename hasSuffix:@".zip"]) {
    copyFile = true;
  }
  else if ([filename hasPrefix:inboxDir]) {
    copyFile = true;
  }

  if (copyFile) {
    NSString* baseName = [filename lastPathComponent];
    NSString* downloadDir =  [[self appDelegate] createDownloadsDirectory];
    NSString* destFile = [downloadDir stringByAppendingPathComponent:baseName];
    [[NSFileManager defaultManager] copyItemAtPath:filename toPath:destFile error:nil];
    return destFile;
  }
  return filename;
}

-(void)openFile:(NSString*)filename
{
  filename = [self maybeCopyDataFile:filename];

  NSDictionary* args = [NSDictionary dictionaryWithObjectsAndKeys:filename, @"FileName", nil];
  [[NSNotificationCenter defaultCenter] postNotificationName:@"OpenFile" object:nil userInfo:args];
}

-(void)handleDownloadedFile:(NSString*)filename downloader:(vesSharedPtr<MyProgressDelegate>) downloader
{
  bool didAbort = downloader->shouldAbort;

  [self hideDownloadHUDAnimated:didAbort];
  self->mMidasDownloader.reset();

  if (didAbort) {
    return;
  }
  
  if (!filename.length) {
    [self showAlertDialogWithTitle:[NSString stringWithUTF8String:downloader->downloader->errorTitle().c_str()]
                           message:[NSString stringWithUTF8String:downloader->downloader->errorMessage().c_str()]];
    return;
  }

  [self openFile:filename];
}

-(void)onAbortDownload
{
  if (self->mMidasDownloader) {
    self->mMidasDownloader->shouldAbort = true;
  }
  else {
    [self abortDropboxDownload];
  }
}

-(DocumentsTableViewController*) newDocumentsViewController
{
  UIStoryboard*  sb =  self.storyboard;
  DocumentsTableViewController* tableViewController = [sb instantiateViewControllerWithIdentifier:@"DocumentsTableViewController"];
  tableViewController.folderId = nil;
  tableViewController.communityId = nil;
  tableViewController.localFolder = nil;
  return tableViewController;
}

-(void)handleFolderSelected:(int)folderIndex
{
  std::string selectedFolderName = self->folderNames[folderIndex];
  std::string selectedFolderId = self->folderIds[folderIndex];

  DocumentsTableViewController* tableViewController = [self newDocumentsViewController];
  tableViewController.folderId = [NSString stringWithUTF8String:selectedFolderId.c_str()];
  tableViewController.title = [NSString stringWithUTF8String:selectedFolderName.c_str()];
  tableViewController.client = self.client;
  [self.navigationController pushViewController:tableViewController animated:YES];
}

-(void)handleCommunitySelected:(int)communityIndex
{
  std::string selectedCommunityName = self->communityNames[communityIndex];
  std::string selectedCommunityId = self->communityIds[communityIndex];

  DocumentsTableViewController* tableViewController = [self newDocumentsViewController];
  tableViewController.communityId = [NSString stringWithUTF8String:selectedCommunityId.c_str()];
  tableViewController.title = [NSString stringWithUTF8String:selectedCommunityName.c_str()];
  tableViewController.client = self.client;
  [self.navigationController pushViewController:tableViewController animated:YES];
}

-(void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{

  if ([segue.identifier isEqualToString:@"gotoMidasLogin"]) {
    MidasLoginViewController* loginView = segue.destinationViewController;
    loginView.client = self.client;
  }
}

- (IBAction)onSegueReturned:(UIStoryboardSegue *)segue {
  if ([segue.identifier isEqualToString:@"midasLoginReturn"]) {
    if (self.client->token().size()) {
      self->gotoMidasDocs = YES;
    }
  }
}

- (void) viewDidAppear:(BOOL) animated {
  [super viewDidAppear:animated];

  if (self->gotoMidasDocs) {
    self->gotoMidasDocs = NO;

    [self startSpinner];
    [self clearMidasTables];
    [self popluateView];
  }
}

-(NSString*) pathForLocalDocument:(NSIndexPath*)indexPath
{
  if (indexPath.section != self->localFilesSection || self.showDropbox) {
    return nil;
  }

  int row = indexPath.row;
  int numberOfFolders = self->localFolders.size();

  NSString* localDocumentName;
  if (row < numberOfFolders) {
    localDocumentName = [NSString stringWithUTF8String:self->localFolders[row].c_str()];
  }
  else {
    int fileIndex = row - numberOfFolders;
    localDocumentName = [NSString stringWithUTF8String:self->localFiles[fileIndex].c_str()];
  }

  return [self.localFolder stringByAppendingPathComponent:localDocumentName];
}

-(NSString*) pathForDropboxDocument:(NSIndexPath*)indexPath
{
  if (indexPath.section != self->localFilesSection || !self.showDropbox) {
    return nil;
  }

  int row = indexPath.row;
  int numberOfFolders = self->localFolders.size();

  NSString* localDocumentName;
  if (row < numberOfFolders) {
    localDocumentName = [NSString stringWithUTF8String:self->localFolders[row].c_str()];
  }
  else {
    int fileIndex = row - numberOfFolders;
    localDocumentName = [NSString stringWithUTF8String:self->localFiles[fileIndex].c_str()];
  }

  return [self.localFolder stringByAppendingPathComponent:localDocumentName];
}

-(void)handleLocalDocumentSelected:(NSIndexPath*)indexPath
{
  int row = indexPath.row;
  int numberOfFolders = self->localFolders.size();

  NSString* documentPath = [self pathForLocalDocument:indexPath];

  if (row < numberOfFolders) {
    std::string selectedFolderName = self->localFolders[row];
    DocumentsTableViewController* tableViewController = [self newDocumentsViewController];
    tableViewController.localFolder = documentPath;
    tableViewController.title = [NSString stringWithUTF8String:selectedFolderName.c_str()];
    tableViewController.client = self.client;
    [self.navigationController pushViewController:tableViewController animated:YES];
  }
  else {
    [self.tableView deselectRowAtIndexPath:indexPath animated:YES];
    [self openFile:documentPath];
  }
}

-(void)handleDropboxFolderSelected:(NSString*)path
{
  DocumentsTableViewController* tableViewController = [self newDocumentsViewController];
  tableViewController.showDropbox = YES;
  tableViewController.localFolder = path;
  tableViewController.title = [path lastPathComponent];

  if ([tableViewController.title isEqualToString:@"/"]) {
    tableViewController.title = @"Top";
  }
  tableViewController.client = self.client;
  [self.navigationController pushViewController:tableViewController animated:YES];
}

-(void)handleDropboxPathSelected:(NSIndexPath*)indexPath
{
  int row = indexPath.row;
  int numberOfFolders = self->localFolders.size();

  NSString* documentPath = [self pathForDropboxDocument:indexPath];

  if (row < numberOfFolders) {
    [self handleDropboxFolderSelected:documentPath];
  }
  else {
    [self.tableView deselectRowAtIndexPath:indexPath animated:YES];
    [self downloadFileFromDropbox:documentPath];
  }
}

-(void)handleMidasSelected
{
  DocumentsTableViewController* tableViewController = [self newDocumentsViewController];
  tableViewController.showMidasTopLevelFolders = YES;
  tableViewController.showMidasCommunities = YES;
  tableViewController.title = @"Midas";
  tableViewController.client = self.client;
  [self.navigationController pushViewController:tableViewController animated:YES];

}

-(void)handleAppSelected:(NSIndexPath*)indexPath
{
  [self.tableView deselectRowAtIndexPath:indexPath animated:YES];
  std::string selectedDemoApp = self->appNames[indexPath.row];


  if (selectedDemoApp == "Dropbox") {
    [self handleDropboxSelected];
  }
  else if (selectedDemoApp == "Midas") {
    [self handleMidasSelected];
  }
  else {
    [[NSNotificationCenter defaultCenter] postNotificationName:[NSString stringWithUTF8String:selectedDemoApp.c_str()] object:nil];
  }
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
  if (self->downloadHUD) {
    self->downloadHUD.dimBackground = NO;
    [self->downloadHUD setNeedsDisplay];
  }
}

-(void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
  if (self->downloadHUD) {
    self->downloadHUD.dimBackground = YES;
    [self->downloadHUD setNeedsDisplay];
  }
}

-(void)hideDownloadHUDAnimated:(bool)animated
{
  if (self->downloadHUD) {
    self->downloadHUD.removeFromSuperViewOnHide = YES;
    [self->downloadHUD hide:animated];
    self->downloadHUD = nil;
  }
}

-(UIProgressView*)createDownloadHUD
{
  UIButton *cancelButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
  [cancelButton addTarget:self
                action:@selector(onAbortDownload)
                forControlEvents:UIControlEventTouchUpInside];
  
  [cancelButton setTitle:@"Cancel" forState:UIControlStateNormal];
  [cancelButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
  cancelButton.titleLabel.font = [UIFont boldSystemFontOfSize:12];
  cancelButton.titleLabel.shadowColor = [UIColor lightGrayColor];
  cancelButton.titleLabel.shadowOffset = CGSizeMake(0, -1);
  [cancelButton setBackgroundImage:[[UIImage imageNamed:@"delete_button"]
                                             stretchableImageWithLeftCapWidth:8.0f
                                             topCapHeight:0.0f]
                                             forState:UIControlStateNormal];

  UIProgressView *progressView = [UIProgressView new];
  progressView.progress = 0.0;


  double hudWidth = 300;

  if (UI_USER_INTERFACE_IDIOM()==UIUserInterfaceIdiomPhone) {
    hudWidth = 240;
  }

  double sidePadding = 0;
  UIView* widgetsView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, hudWidth, 55)];

  double progressBarWidth = hudWidth - 2*sidePadding;
  double progressBarHeight = progressView.frame.size.height;
  progressView.frame = CGRectMake(sidePadding, 46, progressBarWidth, progressBarHeight);
  

  UILabel* label = [[UILabel alloc] init];
  label.text = @"Downloading...";
  label.backgroundColor = [UIColor clearColor];
  label.textColor = [UIColor whiteColor];
  label.opaque = NO;
  label.font = [UIFont boldSystemFontOfSize:16.f];
  label.adjustsFontSizeToFitWidth = NO;
	CGSize labelSize = [label.text sizeWithFont:label.font];

  label.frame = CGRectMake(sidePadding, 6, labelSize.width, labelSize.height);

  double buttonWidth = 60;
  double buttonHeight = 30;
  cancelButton.frame = CGRectMake(hudWidth - (buttonWidth+sidePadding), 2,
                           buttonWidth, buttonHeight);

  [widgetsView addSubview:progressView];
  [widgetsView addSubview:cancelButton];
  [widgetsView addSubview:label];

  MBProgressHUD *hud = [MBProgressHUD showHUDAddedTo:[self viewForHUD] animated:YES withDelay:0.0];

  hud.mode = MBProgressHUDModeCustomView;
  hud.customView = widgetsView;
  hud.dimBackground = YES;
  self->downloadHUD = hud;

  return progressView;
}

-(void)handleItemSelected:(int) itemIndex indexPath:(NSIndexPath*)indexPath
{
  [self.tableView deselectRowAtIndexPath:indexPath animated:YES];

  UIProgressView* progressView = [self createDownloadHUD];

  std::string destDir = [[[self appDelegate] createDownloadsDirectory] UTF8String];

  if (self->client->token().size()) {
    self->client->renewToken();

    // todo token handle error here
  }

  double totalBytes = self->itemBytes[itemIndex];
  std::string selectedItemId = self->itemIds[itemIndex];
  std::string downloadUrl = self->client->itemDownloadUrl(selectedItemId);

  vesSharedPtr<vesKiwiCurlDownloader> downloader = vesSharedPtr<vesKiwiCurlDownloader>(new vesKiwiCurlDownloader);
  vesSharedPtr<MyProgressDelegate> progressDelegate = vesSharedPtr<MyProgressDelegate>(new MyProgressDelegate);
  progressDelegate->totalBytes = totalBytes;
  progressDelegate->progressView = progressView;
  progressDelegate->downloader = downloader;
  progressDelegate->itemPath = indexPath;
  progressDelegate->itemId = selectedItemId;
  downloader->setProgressDelegate(progressDelegate);
  self->mMidasDownloader = progressDelegate;

  dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{

    std::string downloadedFile = downloader->downloadUrlToDirectory(downloadUrl, destDir);

    dispatch_async(dispatch_get_main_queue(), ^{
      [self handleDownloadedFile:[NSString stringWithUTF8String:downloadedFile.c_str()] downloader:progressDelegate];
    });
  });

}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{

  if (indexPath.section == userFoldersSection && !self->client->token().size()) {
    [self performSegueWithIdentifier: @"gotoMidasLogin" sender: self];
    return;
  }
  
  if (indexPath.section == self->foldersSection || indexPath.section == userFoldersSection) {


    int row = indexPath.row;
    int numberOfFolders = self->folderNames.size();

    if (row < numberOfFolders) {
      [self handleFolderSelected:row];
    }
    else {
      int itemIndex = row - numberOfFolders;
      [self handleItemSelected:itemIndex indexPath:indexPath];
    }
  }
  if (indexPath.section == self->communityFoldersSection) {
    [self handleFolderSelected:indexPath.row];
  }
  else if (indexPath.section == self->communitiesSection) {
    [self handleCommunitySelected:indexPath.row];
  }
  else if (indexPath.section == self->localFilesSection && self.showDropbox) {
    [self handleDropboxPathSelected:indexPath];
  }
  else if (indexPath.section == self->localFilesSection) {
    [self handleLocalDocumentSelected:indexPath];
  }
  else if (indexPath.section == self->appSection) {
    [self handleAppSelected:indexPath];
  }

}

- (void)tableView:(UITableView *)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath
{


}

- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section
{
  /*
  if (section == self->numberOfSections - 1) {

    UIView *containerView = [[UIView alloc]
            initWithFrame:CGRectMake(0, 0, self.tableView.frame.size.width, 32)];
    
    UIImage* image = [UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"kitware_logo" ofType:@"png"]];
    UIImageView* imageView = [[UIImageView alloc] initWithImage:image];
    imageView.frame = CGRectMake((self.tableView.frame.size.width) - 80 - 12, 4, 80, 24);
      [imageView.layer setMinificationFilter:kCAFilterTrilinear];

    [containerView addSubview:imageView];
    return containerView;
  }
  */
  return nil;
}

@end
