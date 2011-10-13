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

#ifndef __Transform_h
#define __Transform_h

#include "vsgMacro.h"
#include "vsgTypes.h"
#include "vsgGroupingNode.h"

namespace vsg{

class TransformInternal;

class Transform : public vsgGroupingNode
{
public:
  Transform();
  virtual ~Transform();
  InOutSF(center,SFVec3f)
  InOutSF(rotation,SFRotation)
  InOutSF(scale,SFVec3f)
  InOutSF(scaleOrientation,SFVec4f)
  InOutSF(translation,SFVec3f)
  SFMatrix4f eval();

private:
  TransformInternal *_internal;
  void setInternals();
  SFMatrix4f computeTransform();
};

}

#endif // __Transform_h
