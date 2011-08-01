// ============================================================================
/**
 * @file   Normal.h
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
 * This node defines a set of 3D surface normal vectors to be used in the vector
 * field of some geometry nodes (EXAMPLE IndexedFaceSet and ElevationGrid). This
 * node contains one multiple-valued field that contains the normal
 * vectors. Normals shall be of unit length.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __Normal_h
#define __Normal_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgNormalNode.h"

namespace vsg {
    // -------------------------------------------------------------pre-defines
    class NormalInternal;

    // -------------------------------------------------------------------class
    class Normal : public vsgNormalNode
    {
      public:
      // ........................................................public-methods
      Normal();
      virtual ~Normal();
      //InOutMF(vector, MFVec3f)
      bool accept(vsgVisitor *vsgVisitor);

      protected:
      // .......................................................protected-ivars

      private:
      // .........................................................private-ivars
      NormalInternal *_internal;
    };
}
#endif // __Normal_h
