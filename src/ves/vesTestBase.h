#ifndef VESTESTBASE_H
#define VESTESTBASE_H

#include <vesConfigure.h>

#include "vesRenderer.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#ifdef VES_QNX
  #include "vesQNXTestDriver.h"
  #include <stdio.h>
  #include <stdlib.h>
  #include <errno.h>
  #include <sys/neutrino.h>
  #include <sys/syspage.h>
#elif defined (VES_HOST)
  #include "vesX11TestDriver.h"
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
  #include <X11/keysym.h>
#endif

class vesTestBase
{
public:
  vesTestBase() :
    m_renderer(vesRenderer::Ptr(new vesRenderer()))
  {
    this->m_isTesting = false;
  }


  virtual bool init(int argc, char* argv[])
  {
    if (argc < 2) {
      printf("Usage: %s <path to VES source directory> [path to testing data directory]\n", argv[0]);
      return false;
    }

    this->setSourceDirectory(argv[1]);

    if (argc == 3) {
      this->setDataDirectory(argv[2]);
      this->setTesting(true);
    }

    this->initScene();

    return true;
  }


  virtual bool initScene() = 0;


  virtual void render()
  {
    this->m_renderer->render();
  }


  virtual void resizeView(int winWidth, int winHeight)
  {
    this->m_renderer->resize(winWidth, winHeight, 1.0f);
  }


  virtual void resetView()
  {
    // dolly so that scene fits window
    this->m_renderer->resetCamera();
  }


  std::string sourceDirectory()
  {
    return this->m_sourceDirectory;
  }


  void setSourceDirectory(std::string dir)
  {
    this->m_sourceDirectory = dir;
  }


  std::string dataDirectory()
  {
    return this->m_dataDirectory;
  }


  void setDataDirectory(std::string dir)
  {
    this->m_dataDirectory = dir;
  }


  bool isTesting()
  {
    return this->m_isTesting;
  }


  void setTesting(bool testing)
  {
    this->m_isTesting = testing;
  }


  virtual bool doTesting()
  {
    this->render();

    return true;
  }

  virtual void finalizeTest()
  {
  }


  void handleTwoTouchPanGesture(double x0, double y0, double x1, double y1)
  {
    vesNotUsed(x0);
    vesNotUsed(y0);
    vesNotUsed(x1);
    vesNotUsed(y1);
  }


  virtual void handleSingleTouchPanGesture(double deltaX, double deltaY)
  {
    vesNotUsed(deltaX);
    vesNotUsed(deltaY);
  }


  virtual void handleTwoTouchPinchGesture(double scale)
  {
    vesNotUsed(scale);
  }


  virtual void handleTwoTouchRotationGesture(double rotation)
  {
    vesNotUsed(rotation);
  }


  virtual void handleSingleTouchDown(int displayX, int displayY)
  {
    vesNotUsed(displayX);
    vesNotUsed(displayY);
  }


  virtual void handleSingleTouchUp()
  {
  }


  virtual void handleSingleTouchTap(int displayX, int displayY)
  {
    vesNotUsed(displayX);
    vesNotUsed(displayY);
  }


  virtual void handleDoubleTap(int displayX, int displayY)
  {
    vesNotUsed(displayX);
    vesNotUsed(displayY);
  }

  virtual void handleLongPress(int displayX, int displayY)
  {
    vesNotUsed(displayX);
    vesNotUsed(displayY);
  }


protected:
  vesRenderer::Ptr m_renderer;

  std::string       m_sourceDirectory;
  std::string       m_dataDirectory;
  bool              m_isTesting;
};

#endif // VESTESTBASE_H
