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

#include "vesTriangleData.h"

vesTriangleData::vesTriangleData()
{
  this->HasBounds = false;
  this->Max[0] = this->Max[1] = this->Max[2] = 1;
  this->Min[0] = this->Min[1] = this->Min[2] = 0;
  this->HasNormals = false;
}

vesVector3f vesTriangleData::GetMin()
{
  if (!this->HasBounds)
    this->ComputeBounds();

  return this->Min;
}

vesVector3f vesTriangleData::GetMax()
{
  if (!this->HasBounds)
    this->ComputeBounds();

  return this->Max;
}

vesVector2f vesTriangleData::GetPointScalarRange()
{
  vesVector2f range(0, 1);
  for (size_t i = 0; i < this->PointScalars.size(); ++i)
  {
    float v = this->PointScalars[i];
    if (i == 0)
    {
      range[0] = v;
      range[1] = v;
    }
    if (v < range[0]) range[0] = v;
    if (v > range[1]) range[1] = v;
  }
  return range;
}

void vesTriangleData::ComputeBounds()
{
  if (this->HasBounds)
    return;

  for (size_t i = 0; i < this->Points.size(); ++i) {
    vtkVertex3f v = this->Points[i];
    for (int j = 0; j < 3; ++j) {
      if (i == 0) {
        this->Max[j] = this->Min[j] = v.point[j];
      }
      else {
        if (v.point[j] > this->Max[j])
          this->Max[j] = v.point[j];
        if (v.point[j] < this->Min[j])
          this->Min[j] = v.point[j];
      }
    }
  }
  this->HasBounds = true;
}

void vesTriangleData::ComputeNormals()
{
  if (this->HasNormals)
  {
    return;
  }
  for (size_t i = 0; i < this->Points.size(); ++i)
  {
    vesVector3f& n = this->Points[i].normal;
    n[0] = n[1] = n[2] = 0;
  }
  for (size_t i = 0; i < this->Triangles.size(); ++i)
  {
    vesVector3us tri = this->Triangles[i];
    vtkVertex3f& p1 = this->Points[tri[0]];
    vtkVertex3f& p2 = this->Points[tri[1]];
    vtkVertex3f& p3 = this->Points[tri[2]];
    vesVector3f u = p2.point - p1.point;
    vesVector3f v = p3.point - p1.point;
    vesVector3f n;
    n[0] = u[1]*v[2] - u[2]*v[1];
    n[1] = u[2]*v[0] - u[0]*v[2];
    n[2] = u[0]*v[1] - u[1]*v[0];
    p1.normal += n;
    p2.normal += n;
    p3.normal += n;
  }
  for (size_t i = 0; i < this->Points.size(); ++i)
  {
    vesVector3f& n = this->Points[i].normal;
    float len = n[0]*n[0] + n[1]*n[1] + n[2]*n[2];
    if (len > 0)
    {
      n /= sqrt(len);
    }
    else
    {
      n[0] = 0;
      n[1] = 0;
      n[2] = 1;
    }
  }
  this->HasNormals = true;
}
