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
/// \class vesQNXTestDriver
/// \ingroup ves

#ifndef VESKIWIQNXTESTDRIVER_H
#define VESKIWIQNXTESTDRIVER_H

#include "vesKiwiTestDriver.h"

#include <vesQNXTestDriver.h>
#include <vesKiwiBaseApp.h>

#include <GLES/gl.h>
#include <GLES/egl.h>

#include <gf/gf.h>
#include <gf/gf3d.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>

#include <iostream>

class  vesKiwiQNXTestDriver : public vesKiwiTestDriver
{
public:
  vesQNXTestDriver<vesKiwiBaseApp>* m_qnxTestDriver;

public:
  vesKiwiQNXTestDriver(vesKiwiBaseApp* test) : vesKiwiTestDriver(test) {
    this->m_qnxTestDriver = new vesQNXTestDriver<vesKiwiBaseApp>(test);
  }

  int init()
  {
    return this->m_qnxTestDriver->init();
  }


  void render()
  {
    this->m_qnxTestDriver->render();
  }


  void start()
  {
    this->m_qnxTestDriver->start();
  }


  void finalize()
  {
    this->m_qnxTestDriver->finalize();
  }


  virtual int width()
  {
    return this->m_qnxTestDriver->width();
  }


  virtual int height()
  {
    return this->m_qnxTestDriver->height();
  }

};

#endif // VESKIWIQNXTESTDRIVER_H
