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

// .NAME vtkTextureCoordinateNode - abstract base type
// .SECTION Description
// vtkTextureCoordinateNode is the base type for all node types which specify
// texture coordinates

#ifndef __vtkTextureCoordinateNode_h
#define __vtkTextureCoordinateNode_h

#include "vsgGeometricPropertyNode.h"

class vtkTextureCoordinateNode : public vsgGeometricPropertyNode
{
public:
  vtkTextureCoordinateNode();
  ~vtkTextureCoordinateNode();
};

#endif // __vtkTextureCoordinateNode_h
