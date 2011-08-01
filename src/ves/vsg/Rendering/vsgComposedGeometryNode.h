// ============================================================================
/**
 * @file   vsgComposedGeometryNode.h
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
 *
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgComposedGeometryNode_h
#define __vsgComposedGeometryNode_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgGeometryNode.h"

    // -------------------------------------------------------------pre-defines

    // -------------------------------------------------------------------class
    class vsgComposedGeometryNode: public vsgGeometryNode
    {
      public:
      // ........................................................public-methods
      vsgComposedGeometryNode(SFBool ccw,
                              SFBool colorPerVertex,
                              SFBool normalPerVertex,
                              SFBool solid);
      virtual ~vsgComposedGeometryNode();
      InOutMF(attrib,MFNode)
      InOutSF(color,SFNode)
      InOutSF(coord,SFNode)
      InOutSF(normal,SFNode)
      InOutSF(texCoord,SFNode)
      protected:
      // .......................................................protected-ivars
      SFBool _ccw;
      SFBool _colorPerVertex;
      SFBool _normalPerVertex;
      SFBool _solid;
    };
#endif // __vsgComposedGeometryNode_h
