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
 * This node defines a set of 3D coordinates to be used in the coord field of
 * vertex-based geometry nodes.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */

#ifndef __Coordinate_h
#define __Coordinate_h

#include "vsgMacro.h"
#include "vsgTypes.h"
#include "vsgCoordinateNode.h"

namespace vsg {

class CoordinateInternal;

class Coordinate: public vsgCoordinateNode
{
public:
  Coordinate();
  virtual ~Coordinate();
  //InOutMF(point, MFVec3f)
  bool accept(vsgVisitor *vsgVisitor);

private:
  CoordinateInternal *_internal;
};

}

#endif // __Coordinate_h
