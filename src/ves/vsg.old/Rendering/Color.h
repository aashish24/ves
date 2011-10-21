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
 *
 * @section DESCRIPTION
 *
 * This node defines a set of RGB colours to be used in the fields of another
 * node.
 *
 * Color nodes are only used to specify multiple colours for a single geometric
 * shape, such as colours for the faces or vertices of an IndexedFaceSet. A
 * Material node is used to specify the overall material parameters of lit
 * geometry. If both a Material node and a Color node are specified for a
 * geometric shape, the colours shall replace the diffuse component of the
 * material.
 *
 * RGB or RGBA textures take precedence over colours; specifying both an RGB or
 * RGBA texture and a Color node for geometric shape will result in the Color
 * node being ignored. Details on lighting equations can be found in Lighting
 * model.
 */

#ifndef __Color_h
#define __Color_h

# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgColorNode.h"

namespace vsg {

class ColorInternal;

class Color: public vsgColorNode
{
public:
  Color();
  virtual ~Color();
  //InOutMF(color,MFColor)
  bool accept(vsgVisitor *vsgVisitor);

private:
  ColorInternal *_internal;
};

}

#endif // __Color_h
