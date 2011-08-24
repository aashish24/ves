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
 * The ClipPlane node specifies a single plane equation that will be used to
 * clip the geometry. The plane field specifies a four-component plane equation
 * that describes the inside and outside half space. The first three components
 * are a normalized vector describing the direction of the plane's normal
 * direction.
 */

#ifndef __ClipPlane_h
#define __ClipPlane_h

# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgChildNode.h"

namespace vsg {

class ClipPlaneInternal;

class ClipPlane: public vsgChildNode
{
public:
  ClipPlane();
  virtual ~ClipPlane();
  InOutSF(enabled,SFBool)
  InOutSF(plane,SFVec4f)
  bool accept(vsgVisitor *vsgVisitor);

private:
  ClipPlaneInternal *_internal;
};

}

#endif // __ClipPlane_h
