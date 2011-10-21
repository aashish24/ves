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
 * This is the base node type for all Shape nodes.
 */

#ifndef __vsgShapeNode_h
#define __vsgShapeNode_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Core/vsgChildNode.h"
#include "vsg/Grouping/vsgBoundedObject.h"

class vsgShapeNode: public vsgChildNode, public vsgBoundedObject
{
public:
  vsgShapeNode();
  virtual ~vsgShapeNode();
  InOutSF(appearance,SFNode)
  InOutSF(geometry,SFNode)

};

#endif // __vsgShapeNode_h
