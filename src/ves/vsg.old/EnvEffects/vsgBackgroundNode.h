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
 * X3DBackgroundNode is the abstract type from which all backgrounds
 * inherit. X3DBackgroundNode is a bindable node that, when bound, defines the
 * panoramic background for the scene. For complete information on backgrounds,
 * see 24.2.1 Backgrounds.
 */

#ifndef __vsgBackgroundNode_h
#define __vsgBackgroundNode_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Core/vsgBindableNode.h"

class vsgBackgroundNode: public vsgBindableNode
{
public:
  vsgBackgroundNode();
  virtual ~vsgBackgroundNode();
  InOutMF(groundAngle,MFFloat)
  InOutMF(groundColor,MFColor)
  InOutMF(skyAngle,MFFloat)
  InOutMF(skyColor,MFColor)
  InOutSF(transparency,SFFloat)
};

#endif // __vsgBackgroundNode_h
