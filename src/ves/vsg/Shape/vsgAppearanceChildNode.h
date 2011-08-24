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
 * This is the base node type for the child nodes of the X3DAppearanceNode type.
 */

#ifndef __vsgAppearanceChildNode_h
#define __vsgAppearanceChildNode_h

#include "vsgMacro.h"
#include "vsgTypes.h"
#include "vsgNode.h"

class vsgAppearanceChildNode: public vsgNode
{
public:
  vsgAppearanceChildNode();
  virtual ~vsgAppearanceChildNode();
};

#endif // __vsgAppearanceChildNode_h
