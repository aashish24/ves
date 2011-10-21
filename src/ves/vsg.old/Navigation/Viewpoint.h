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
 */

#ifndef __Viewpoint_h
#define __Viewpoint_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Navigation/vsgViewpointNode.h"

namespace vsg {

class ViewpointInternal;

class Viewpoint: public vsgViewpointNode
{
public:
  Viewpoint();
  virtual ~Viewpoint();
  InOutSF(fieldOfView,SFFloat)

private:
  ViewpointInternal *_internal;
};

}
#endif // __Viewpoint_h
