// ============================================================================
/**
 * @file   FloatVertexAttribute.h
 *
 * @section COPYRIGHT
 *
 * Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 * All rights reserved.
 * See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 *
 *   This software is distributed WITHOUT ANY WARRANTY; without even
 *   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *   PURPOSE.  See the above copyright notice for more information.
 *
 * @section DESCRIPTION
 *
 * The FloatVertexAttribute node defines a set of per-vertex single-precision floating point attributes.

The numComponents field specifies how many consecutive floating point values should be grouped together per vertex. The length of the value field shall be a multiple of numComponents.

The value field specifies an arbitrary collection of floating point values that will be passed to the shader as per-vertex information. The specific type mapping to the individual shading language data types is in the appropriate language-specific annex (see Table 31.2).


 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __FloatVertexAttribute_h
#define __FloatVertexAttribute_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Shader/vsgVertexAttributeNode.h"

namespace vsg {
    // -------------------------------------------------------------pre-defines
    class FloatVertexAttributeInternal;

    // -------------------------------------------------------------------class
    class FloatVertexAttribute: public vsgVertexAttributeNode
    {
      public:
      // ........................................................public-methods
      FloatVertexAttribute();
      virtual ~FloatVertexAttribute();
      InOutMF(value,MFFloat)
      bool accept(vsgVisitor *vsgVisitor);
      protected:
      // .......................................................protected-ivars
      SFInt32 _numComponents;
      private:
      // .........................................................private-ivars
      FloatVertexAttributeInternal *_internal;
    };
}
#endif // __FloatVertexAttribute_h
