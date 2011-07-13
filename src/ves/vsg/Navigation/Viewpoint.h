// ============================================================================
/**
 * @file   Viewpoint.h
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
 * The Viewpoint node defines a viewpoint that provides a perspective view of
 * the scene. A perspective view is one in which all projectors coalesce at
 * position.
 *
 * The fieldOfView field specifies a preferred minimum viewing angle from this
 * viewpoint in radians. A small field of view roughly corresponds to a
 * telephoto lens; a large field of view roughly corresponds to a wide-angle
 * lens. The field of view shall be greater than zero and smaller than π. The
 * value of fieldOfView represents the minimum viewing angle in any direction
 * axis perpendicular to the view.
 *
 * A browser with a rectangular viewing projection has the following
 * relationship:
 *
 * display width    tan(FOVhorizontal/2)
 * -------------- = -------------------
 * display height   tan(FOVvertical/2)
 *
 * where the smaller of display width or display height determines which angle
 * equals the fieldOfView (the larger angle is computed using the relationship
 * described above). The larger angle shall not exceed π and may force the
 * smaller angle to be less than fieldOfView in order to sustain the aspect
 * ratio.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __Viewpoint_h
#define __Viewpoint_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Navigation/vsgViewpointNode.h"

namespace vsg {
  // -------------------------------------------------------------pre-defines
  class ViewpointInternal;

  // -------------------------------------------------------------------class
  class Viewpoint: public vsgViewpointNode
  {
  public:
    // ........................................................public-methods
    Viewpoint();
    virtual ~Viewpoint();
    InOutSF(fieldOfView,SFFloat)
  protected:
    // .......................................................protected-ivars

  private:
    // .........................................................private-ivars
    ViewpointInternal *_internal;
  };
}
#endif // __Viewpoint_h
