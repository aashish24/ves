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
/// \class vesKiwiX11TestDriver
/// \ingroup ves

#include "vesKiwiTestDriver.h"

#include <vesX11TestDriver.h>

class vesKiwiX11TestDriver : public vesKiwiTestDriver
{
public:
  vesKiwiX11TestDriver(vesKiwiBaseApp* app) : vesKiwiTestDriver(app)
  {
    this->m_x11TestDriver = new vesX11TestDriver<vesKiwiBaseApp>(app);
  }


  virtual int init()
  {
    return this->m_x11TestDriver->init();
  }


  virtual void render()
  {
    this->m_x11TestDriver->render();
  }


  virtual void start()
  {
    this->m_x11TestDriver->start();
  }


  virtual void finalize()
  {
    this->m_x11TestDriver->finalize();
  }


  virtual int width()
  {
    return this->m_x11TestDriver->width;
  }


  virtual int height()
  {
    return this->m_x11TestDriver->height;
  }

public:
  vesX11TestDriver<vesKiwiBaseApp>* m_x11TestDriver;
};
