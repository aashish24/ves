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
 * This node defines a set of 3D surface normal vectors to be used in the vector
 * field of some geometry nodes (EXAMPLE IndexedFaceSet and ElevationGrid). This
 * node contains one multiple-valued field that contains the normal
 * vectors. Normals shall be of unit length.
 */

#ifndef __Normal_h
#define __Normal_h

#include "vsgMacro.h"
#include "vsgTypes.h"
#include "vsgNormalNode.h"

namespace vsg {

class NormalInternal;

class Normal : public vsgNormalNode
{
public:
  Normal();
  virtual ~Normal();
  //InOutMF(vector, MFVec3f)
  bool accept(vsgVisitor *vsgVisitor);

private:
  NormalInternal *_internal;
};

}

#endif // __Normal_h
