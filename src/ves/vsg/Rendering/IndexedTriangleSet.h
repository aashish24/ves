// ============================================================================
/**
 * @file   IndexedTriangleSet.h
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
 * The IndexedTriangleSet node represents a 3D shape composed of a collection of
 * individual triangles. IndexedTriangleSet uses the indices in its index field
 * to specify the vertices of each triangle from the coord field. Each triangle
 * is formed from a set of three vertices of the Coordinate node identified by
 * three consecutive indices from the index field. If the index field does not
 * contain a multiple of three coordinate values, the remaining vertices shall
 * be ignored.
 *
 * The IndexedTriangleSet node is specified in the local coordinate system and
 * is affected by the transformations of its ancestors. Descriptions of the
 * color, coord, normal, and texCoord fields are provided in the Color,
 * ColorRGBA, Coordinate, Normal, and TextureCoordinate nodes, respectively. If
 * values are provided for the color, normal and texCoord fields, the values are
 * applied in the same manner as the values from the coord field and there shall
 * be at least as many values as are present in the coord field. The value of
 * the colorPerVertex field is ignored and always treated as TRUE. If the normal
 * field is not supplied, normals shall be generated as follows:
 *
 * - If normalPerVertex is TRUE, the normal at each vertex shall be the average
     of the normals for all triangles that share that vertex.
 * - If normalPerVertex is FALSE, the normal at each vertex shall be
     perpendicular to the face for that triangle.
 *
 * The solid field determines whether the IndexedTriangleSet is visible when
 * viewed from the backside. Common geometry fields provides a complete
 * description of the solid field.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __IndexedTriangleSet_h
#define __IndexedTriangleSet_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgComposedGeometryNode.h"

namespace vsg {
    // -------------------------------------------------------------pre-defines
    class IndexedTriangleSetInternal;

  // Small struct packing a point and normal together in a vertex
  // Memory layout is 3 floats for a point followed by 3 floats for a normal
  struct vsgVertex3f
  {
    vesVector3f point;
    vesVector3f normal;
  };

    // -------------------------------------------------------------------class
    class IndexedTriangleSet: public vsgComposedGeometryNode
    {
      public:
      // ........................................................public-methods
      IndexedTriangleSet( SFBool ccw =true,
                          SFBool colorPerVertex =true,
                          SFBool normalPerVertex = true,
                          SFBool solid =true);
      virtual ~IndexedTriangleSet();
      //InOnlyMF(index,MFInt32)
      unsigned int * _index;
      void set_index(unsigned int* index)
      {
        _index = index;
      }
      bool accept(vsgVisitor *vsgVisitor);
      virtual std::vector<vsgVertex3f>& GetPoints() =0;
      virtual std::vector<vesVector3us>& GetTriangles()=0;
      protected:
      // .......................................................protected-ivars

      private:
      // .........................................................private-ivars
      IndexedTriangleSetInternal *_internal;
    };
}
#endif // __IndexedTriangleSet_h
