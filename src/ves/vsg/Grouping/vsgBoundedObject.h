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
  ~vsgBoundedObject();
  virtual void computeBounds()=0;
  InOutSF(BBoxCenter,SFVec3f)
  InOutSF(BBoxSize, SFVec3f)
  InOutSF(min,SFVec3f)
  InOutSF(max,SFVec3f)

  void set_BBoxCenter(vesVector3f min, vesVector3f max);
  void set_BBoxSize(vesVector3f min, vesVector3f max);
  virtual float GetBBoxRadius();
};

#endif // __vsgBoundedObject_h
