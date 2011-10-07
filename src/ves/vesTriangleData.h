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

#ifndef vesTriangleData_H
#define vesTriangleData_H

// VES includes
#include "vesGMTL.h"

// C++ includes
#include <vector>

// Small struct packing a point and normal together in a vertex
// Memory layout is 3 floats for a point followed by 3 floats for a normal
struct vtkVertex3f
{
  vesVector3f point;
  vesVector3f normal;
};

class vesTriangleData
{
public:
  vesTriangleData();
  std::vector<vtkVertex3f>& GetPoints() { return this->Points; }
  std::vector<vesVector3us>& GetTriangles() { return this->Triangles; }
  std::vector<vesVector2us>& GetLines() { return this->Lines; }
  std::vector<vesVector3f>& GetVertexColors() { return this->VertexColors; }
  std::vector<vesVector2f>& GetTextureCoordinates() { return this->TextureCoordinates; }
  vesVector3f GetMin();
  vesVector3f GetMax();
  void SetHasNormals(bool b) { this->HasNormals = b; }
  void ComputeNormals();
  void SetHasBounds(bool b) { this->HasBounds = b; }
  void ComputeBounds();
  std::vector<float>& GetPointScalars() { return this->PointScalars; }
  vesVector2f GetPointScalarRange();
protected:
  std::vector<vtkVertex3f> Points;
  std::vector<vesVector3us> Triangles;
  std::vector<vesVector2us> Lines;
  std::vector<vesVector3f> VertexColors;
  std::vector<vesVector2f> TextureCoordinates;
  std::vector<float> PointScalars;
  vesVector3f Min;
  vesVector3f Max;
  bool HasBounds;
  bool HasNormals;
};

#endif
