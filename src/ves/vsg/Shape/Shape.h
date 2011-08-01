// ============================================================================
/**
 * @file   Shape.h
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
 * The Shape node has two fields, appearance and geometry, that are used to
 * create rendered objects in the world. The appearance field contains an
 * Appearance node that specifies the visual attributes (e.g., material and
 * texture) to be applied to the geometry. The geometry field contains a
 * geometry node. The specified geometry node is rendered with the specified
 * appearance nodes applied. See 12.2 Concepts for more information.
 *
 * 17 Lighting component contains details of the X3D lighting model and the
 * interaction between Appearance nodes and geometry nodes.
 *
 * If the geometry field is NULL, the object is not drawn.
 *
 * The bboxCenter and bboxSize fields specify a bounding box that encloses the
 * Shape node's geometry. This is a hint that may be used for optimization
 * purposes. The results are undefined if the specified bounding box is smaller
 * than the actual bounding box of the geometry at any time. A default bboxSize
 * value, (-1, -1, -1), implies that the bounding box is not specified and, if
 * needed, is calculated by the browser. A description of the bboxCenter and
 * bboxSize fields is contained in 10.2.2 Bounding boxes.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __Shape_h
#define __Shape_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Shape/vsgShapeNode.h"

namespace vsg {
  // -------------------------------------------------------------pre-defines
  class ShapeInternal;

  // -------------------------------------------------------------------class
  class Shape :public vsgShapeNode
  {
  public:
    // ........................................................public-methods
    Shape();
    virtual ~Shape();
    bool accept(vsgVisitor* vsgVisitor);
    bool Read();                // Should be deleted
    void Render(Painter* render); // Should be deleted
    void ComputeBounds();         // Should be deleted
    vesGetMacro(Min, vesVector3f) // Should be deleted
    vesGetMacro(Max, vesVector3f) // Should be deleted

    protected:
    // .......................................................protected-ivars

    private:
    // .........................................................private-ivars
    ShapeInternal *_internal;
  };
}
#endif // __Shape_h
