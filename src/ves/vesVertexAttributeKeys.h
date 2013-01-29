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

#ifndef VESVERTEXATTRIBUTEKEYS_H
#define VESVERTEXATTRIBUTEKEYS_H

#include "vesMaterialAttribute.h"

struct vesVertexAttributeKeys
{
  enum Key {
    Position            = 0,
    Normal              = 1,
    TextureCoordinate   = 2,
    Color               = 3,
    Scalar              = 4,
    CountAttributeIndex = 5,
    Undefined           = 1000
  };

  static int convertMatrerialVertexAttribute(int matVertAttr) {
    switch (matVertAttr)
    {
      case vesMaterialAttribute::VertexPositionAttribute:
        return Position;
      case vesMaterialAttribute::VertexNormalAttribute:
        return Normal;
      case vesMaterialAttribute::VertexTexCoordAttribute:
        return TextureCoordinate;
      case vesMaterialAttribute::VertexColorAttribute:
        return Color;
      default:
        return Undefined;
    }
  }
};

#endif // VESVERTEXATTRIBUTEKEYS_H
