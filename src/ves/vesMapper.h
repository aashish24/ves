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

#ifndef __vesMapper_h
#define __vesMapper_h

#include "vesShaderProgram.h"
#include "vsgGeometryNode.h"
#include "vsgBoundedObject.h"
#include "vesGMTL.h"
#include "vesTriangleData.h"

#include "vesSetGet.h"

class vesMapper : public vsgGeometryNode, public vsgBoundedObject
{
public:
  vesMapper();
  ~vesMapper();
  bool Read();
  vesMatrix4x4f Eval();
  void Render(Painter* render);
  void Render(vesShaderProgram *program);
  void SetTriangleData(vesTriangleData* data);
  vesTriangleData* GetTriangleData();
  vesTriangleData* GetData();
  void ComputeBounds();
  void SetColor(float r, float g, float b, float a);
  vesGetMacro(Red, float)
  vesGetMacro(Green, float)
  vesGetMacro(Blue, float)
  vesGetMacro(Alpha, float)
  vesGetMacro(DrawPoints, bool)
  vesSetMacro(DrawPoints, bool)
  private:
    void Normalize();
  vesMatrix4x4f NormalizedMatrix;
protected:
  float Red,Green,Blue,Alpha;
  bool mIsNew;
  vesTriangleData *Data;
  bool m_initialized;
  bool DrawPoints;

  // unsigned int mMapperVBO[2];
};

#endif
