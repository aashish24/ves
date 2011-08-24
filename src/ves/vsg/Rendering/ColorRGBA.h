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
 * This node defines a set of RGBA colours to be used in the fields of another node.
 *
 * RGBA color nodes are only used to specify multiple colours with alpha for a
 * single geometric shape, such as colours for the faces or vertices of an
 * IndexedFaceSet. A Material node is used to specify the overall material
 * parameters of lit geometry. If both a Material node and a ColorRGBA node are
 * specified for a geometric shape, the colours shall replace the diffuse and
 * transparency components of the material.
 *
 * RGB or RGBA textures take precedence over colours; specifying both an RGB or
 * RGBA texture and a ColorRGBA node for geometric shape will result in the
 * ColorRGBA node being ignored. Details on lighting equations can be found in
 * Lighting model.
 */

#ifndef __ColorRGBA_h
#define __ColorRGBA_h

# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgColorNode.h"

namespace vsg {

class ColorRGBAInternal;

class ColorRGBA: public vsgColorNode
{
public:
  ColorRGBA();
  virtual ~ColorRGBA();
  //InOutMF(color, MFColorRGBA)
  bool accept(vsgVisitor *vsgVisitor);

private:
  ColorRGBAInternal *_internal;
};

}

#endif // __ColorRGBA_h
