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

#include "vesKiwiDataRepresentation.h"
#include "vesActor.h"

#include <vtkTransform.h>

//----------------------------------------------------------------------------
void vesKiwiDataRepresentation::willRender(vesSharedPtr<vesRenderer> renderer)
{
  vesNotUsed(renderer);
}

//----------------------------------------------------------------------------
void vesKiwiDataRepresentation::setTransformOnActor(vesSharedPtr<vesActor> actor, vtkTransform* transform)
{
  assert(actor);
  assert(transform);

  vesVector4f angleAxis;
  vesVector3f translation;
  vesVector3f scale;
  transform->GetPosition(translation.data());
  transform->GetOrientationWXYZ(angleAxis.data());
  transform->GetScale(scale.data());
  angleAxis[0] *= M_PI/180.0;

  //reorder so that it is x,y,z,angle
  double tempAngle = angleAxis[0];
  angleAxis[0] = angleAxis[1];
  angleAxis[1] = angleAxis[2];
  angleAxis[2] = angleAxis[3];
  angleAxis[3] = tempAngle;

  actor->setTranslation(translation);
  actor->setRotation(angleAxis);
  actor->setScale(scale);
}
