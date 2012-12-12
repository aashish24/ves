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


#include <vesKiwiStreamingDataRepresentation.h>
#include <vesKiwiText2DRepresentation.h>


class vesKiwiPointCloudApp : public vesKiwiViewerApp {
public:

  vesTypeMacro(vesKiwiPointCloudApp);
  typedef vesKiwiViewerApp Superclass;

  vesKiwiPointCloudApp() : vesKiwiViewerApp()
  {
    mIsConnected = false;
    mHost = "";
    mPort = 0;
  }

  ~vesKiwiPointCloudApp()
  {
    this->unloadData();
  }

  void unloadData()
  {
    if (mDataRep) {
      mDataRep->removeSelfFromRenderer(this->renderer());
      mDataRep.reset();
    }
  }

  void willRender()
  {
    this->Superclass::willRender();

    if (mIsConnected) {
      this->mDataRep->willRender(this->renderer());
    }
    else {
      this->connect(mHost, mPort);
    }
  }

  void showText(const std::string& textStr)
  {
    if (!mText) {
      mText = this->addTextRepresentation(textStr);
      mText->setDisplayPosition(vesVector2f(10, 10));
    }
    else {
      mText->setText(textStr);
    }
  }

  void setHost(const std::string& host)
  {
    mHost = host;
  }

  void setPort(int port)
  {
    mPort = port;
  }

  bool connect(const std::string& host, int port)
  {
    mIsConnected = false;

    std::stringstream hostPort;
    hostPort << host << ":" << port;
    this->showText("Connecting to " + hostPort.str());

    if (!mDataRep) {
      mDataRep = vesKiwiStreamingDataRepresentation::Ptr(new vesKiwiStreamingDataRepresentation);
    }

    if (!mDataRep->connectToServer(host, port)) {
      this->showText("Connection failed to " + hostPort.str());
      return false;
    }

    this->showText("Connected to " + hostPort.str());
    mIsConnected = true;
    mDataRep->initializeWithShader(this->shaderProgram());
    mDataRep->addSelfToRenderer(this->renderer());
    this->resetView();
    return true;
  }

  bool mIsConnected;
  int mPort;
  std::string mHost;

  vesKiwiStreamingDataRepresentation::Ptr mDataRep;
  vesKiwiText2DRepresentation::Ptr mText;
};



class MyTestHelper : public vesKiwiTestHelper {
public:

  MyTestHelper()
  {
    mKiwiApp = vesKiwiPointCloudApp::Ptr(new vesKiwiPointCloudApp);
    this->setApp(mKiwiApp);
  }


  void handleKeyboard(unsigned char key, int x, int y)
  {
    vesKiwiTestHelper::handleKeyboard(key, x, y);
  }


  vesKiwiPointCloudApp::Ptr mKiwiApp;
};


int main(int argc, char *argv[])
{

  MyTestHelper helper;

  const int windowWidth = 800;
  const int windowHeight = 600;
  helper.init(&argc, argv, windowWidth, windowHeight, "TestStreamingDataRepresentation");
  helper.mKiwiApp->initGL();

  helper.mKiwiApp->setHost("localhost");
  helper.mKiwiApp->setPort(11111);

  helper.startMainLoop();

  return 0;
}
