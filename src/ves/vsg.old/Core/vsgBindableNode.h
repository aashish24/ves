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
 * X3DBindableNode is the abstract base type for all bindable children nodes,
 * including Background, TextureBackground, Fog, NavigationInfo and
 * Viewpoint. For complete discussion of bindable behaviors, see 7.2.2 Bindable
 * children nodes.
 */
#ifndef __vsgBindableNode_h
#define __vsgBindableNode_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Core/vsgChildNode.h"

class vsgBindableNode: public vsgChildNode
{
public:
  vsgBindableNode();
  virtual ~vsgBindableNode();
  InOnlySF(bind, SFBool)
  OutOnlySF(bindTime,SFTime);
  SFBool isBound();
};
#endif // __vsgBindableNode_h
