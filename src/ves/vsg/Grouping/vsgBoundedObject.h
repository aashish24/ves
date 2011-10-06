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

/**
 * @section DESCRIPTION
 *
 * Specifies bounds of an object
 */

#ifndef __vsgBoundedObject_h
#define __vsgBoundedObject_h

#include "vsgMacro.h"
#include "vsgTypes.h"

class vsgBoundedObject
{
public:
           vsgBoundedObject();
  virtual ~vsgBoundedObject();

  virtual void computeBounds()=0;

  const SFVec3f& boundsMinimum() const {return this->m_boundsMinimum;}
  const SFVec3f& boundsMaximum() const {return this->m_boundsMaximum;}

  const SFVec3f& boundsCenter() const {return this->m_boundsCenter;}
  const SFVec3f& boundsSize()   const {return this->m_boundsSize;}

  void setBoundsCenter(const vesVector3f& center);
  void setBoundsSize  (const vesVector3f& size);

  void  setBounds(vesVector3f min, vesVector3f max);

  float boundsRadius();

protected:

  SFVec3f m_boundsCenter;
  SFVec3f m_boundsSize;

  SFVec3f m_boundsMinimum;
  SFVec3f m_boundsMaximum;
};

#endif // __vsgBoundedObject_h
