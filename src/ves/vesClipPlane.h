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
/// \class vesClipPlane
/// \ingroup ves
/// \see vesMaterialAttribute vesMaterial

#ifndef VESCLIPPLANE_H
#define VESCLIPPLANE_H

#include "vesMaterial.h"

// VES includes
#include "vesSetGet.h"

class vesClipPlane : public vesMaterialAttribute
{
public:
  vesTypeMacro(vesClipPlane);

  vesClipPlane();
  virtual ~vesClipPlane();

  void setPlaneEquation(float a, float b, float c, float d);
  void getPlaneEquation(float& a, float& b, float& c, float& d);

  void setPlaneNumber(int i);
  int getPlaneNumber();

  virtual void bind (const vesRenderState &renderState);
  virtual void unbind (const vesRenderState &renderState);

protected:
#if 0
  bool m_wasEnabled;
#endif
  int m_planeNumber;
  float m_planeEquation[4];
//  bool m_previousDepthWriteMask;
//  bool m_depthWriteMask;
};

#endif // VESCLIPPLANE_H
