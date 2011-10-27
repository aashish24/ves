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

#ifndef vesViewport_H
#define vesViewport_H

#include "vesObject.h"

// Forward declarations
class vesRenderState;

class vesViewport : vesObject
{
public:
  vesViewport();
  vesViewport(int x, int y, int width, int height);

  inline void setViewport(int x, int y, int width, int height)
  {
    this->m_x = x;
    this->m_y = y;
    this->m_width = width;
    this->m_height = height;
  }

  double aspect() const;
  double inverseAspect() const;

  inline int x() const { return this->m_x; }
  inline int y() const { return this->m_y; }
  inline int width() const { return this->m_width; }
  inline int height() const { return this->m_height; }

  virtual void render(vesRenderState &renderState);

protected:
  virtual ~vesViewport();

  int m_x;
  int m_y;
  int m_width;
  int m_height;

private:
  /// Not implemented.
  vesViewport(const vesViewport&);

  /// Not implemented.
  void operator=(const vesViewport&);
};


#endif // vesViewport_H
