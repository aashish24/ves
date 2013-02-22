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

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <vesKiwiViewerApp.h>
#include <vesKiwiBaselineImageTester.h>
#include <vesBuiltinShaders.h>
#include <vesKiwiTestHelper.h>

#include <vesMidasClient.h>
#include <vesKiwiCurlDownloader.h>


void doMidasTest(vesKiwiViewerApp::Ptr app)
{

  vesMidasClient midas;
  midas.setHost("http://midas3.kitware.com/midas");

  bool useLogin = false;
  if (useLogin) {
    std::string email = "user@host.com";
    std::string password = "password";
    bool result = midas.login(email, password);
    if (!result) {
      printf("Login error\n");
      return;
    }
  }

  std::vector<std::string> folderNames;
  std::vector<std::string> folderIds;
  std::vector<std::string> itemNames;
  std::vector<std::string> itemIds;

  midas.listCommunities();
  folderNames = midas.folderNames();
  folderIds = midas.folderIds();

  printf("got %lu communities\n", folderNames.size());
  std::string communityId;
  std::string communityName = "KiwiViewer Data";
  for (size_t i = 0; i < folderNames.size(); ++i) {
    if (folderNames[i] == communityName) {
      communityId = folderIds[i];
    }
  }

  if (!communityId.size()) {
    printf("failed to find community: %s\n", communityName.c_str());
    return;
  }

  midas.listCommunityChildren(communityId);
  folderNames = midas.folderNames();
  folderIds = midas.folderIds();

  std::string folderId;
  std::string folderName = "Public";

  for (size_t i = 0; i < folderNames.size(); ++i) {
    if (folderNames[i] == folderName) {
      folderId = folderIds[i];
    }
  }

  if (!folderId.size()) {
    printf("failed to find folder: %s\n", folderName.c_str());
    return;
  }


  midas.listFolderChildren(folderId);
  itemNames = midas.itemNames();
  itemIds = midas.itemIds();

  std::string itemId;
  std::string itemName = "Space Shuttle";

  for (size_t i = 0; i < itemNames.size(); ++i) {
    if (itemNames[i] == itemName) {
      itemId = itemIds[i];
    }
  }

  if (!itemId.size()) {
    printf("failed to find item: %s\n", itemName.c_str());
    return;
  }


  std::string downloadUrl = midas.itemDownloadUrl(itemId);
  printf("downloading url: %s\n", downloadUrl.c_str());

  vesKiwiCurlDownloader downloader;
  std::string destDir = "/tmp";
  std::string downloadedFile = downloader.downloadUrlToDirectory(downloadUrl, destDir);
  if (!downloadedFile.size()) {
    printf("download error: %s\n%s\n", downloader.errorTitle().c_str(), downloader.errorMessage().c_str());
    return;
  }

  printf("downloaded file: %s\n", downloadedFile.c_str());
  bool result = app->loadDataset(downloadedFile);
  if (!result) {
    printf("%s:\n%s\n", app->loadDatasetErrorTitle().c_str(), app->loadDatasetErrorMessage().c_str());
  }
}


int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Usage: %s <path to VES source directory> [path to testing data directory]\n", argv[0]);
    return 1;
  }

  vesKiwiViewerApp::Ptr kiwiApp = vesKiwiViewerApp::Ptr(new vesKiwiViewerApp);
  vesKiwiTestHelper helper;
  helper.setApp(kiwiApp);

  const int windowWidth = 800;
  const int windowHeight = 600;
  helper.init(&argc, argv, windowWidth, windowHeight, "TestKiwiViewer");

  doMidasTest(kiwiApp);

  // render once
  helper.app()->resizeView(windowWidth, windowHeight);
  helper.app()->resetView();
  helper.app()->render();

  // begin the event loop if not in testing mode
  bool testPassed = true;
  if (!helper.isTesting()) {
    helper.startMainLoop();
  }
  else {
    // do testing
  }

  return testPassed ? 0 : 1;
}
