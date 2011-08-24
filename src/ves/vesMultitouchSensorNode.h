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
// .NAME vesMultitouchSensorNode -
// .SECTION Description
// vesMultitouchSensorNode

#ifndef __vesMultitouchSensorNode_h
#define __vesMultitouchSensorNode_h

#include "vsgSensorNode.h"
#include "vesGMTL.h"
#include "vesSetGet.h"

class vesMultitouchSensorNode : public vsgSensorNode
{
public:
  vesMultitouchSensorNode();
  ~vesMultitouchSensorNode();
  vesSetGetMacro(Translation, vesVector3f)
  vesSetGetMacro(Scale, vesVector3f)
  vesSetGetMacro(Rotation,vesVector4f)
protected:
  vesVector3f Translation;
  vesVector3f Scale;
  vesVector4f Rotation;
};

#endif // __vesMultitouchSensorNode_h
