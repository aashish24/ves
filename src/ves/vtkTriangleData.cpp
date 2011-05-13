/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    vtkTriangleData.h
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#include "vtkTriangleData.h"

vtkTriangleData::vtkTriangleData()
{
  this->HasBounds = false;
  this->Max[0] = this->Max[1] = this->Max[2] = 1;
  this->Min[0] = this->Min[1] = this->Min[2] = 0;
  this->HasNormals = false;
}

vtkVector3f vtkTriangleData::GetMin()
{
  if (!this->HasBounds)
    {
    this->ComputeBounds();
    }
  return this->Min;
}

vtkVector3f vtkTriangleData::GetMax()
{
  if (!this->HasBounds)
    {
    this->ComputeBounds();
    }
  return this->Max;
}

void vtkTriangleData::ComputeBounds()
{
  if (this->HasBounds)
    {
    return;
    }
  for (size_t i = 0; i < this->Points.size(); ++i)
    {
    vtkVertex3f v = this->Points[i];
    for (int j = 0; j < 3; ++j)
      {
      if (i == 0)
        {
        this->Max[j] = this->Min[j] = v.point[j];
        }
      else
        {
        if (v.point[j] > this->Max[j]) this->Max[j] = v.point[j];
        if (v.point[j] < this->Min[j]) this->Min[j] = v.point[j];
        }
      }
    }
  this->HasBounds = true;
}

void vtkTriangleData::ComputeNormals()
{
  if (this->HasNormals)
    {
    return;
    }
  for (size_t i = 0; i < this->Points.size(); ++i)
    {
    vtkVector3f& n = this->Points[i].normal;
    n[0] = n[1] = n[2] = 0;
    }
  for (size_t i = 0; i < this->Triangles.size(); ++i)
    {
    vtkVector3us tri = this->Triangles[i];
    vtkVertex3f& p1 = this->Points[tri[0]];
    vtkVertex3f& p2 = this->Points[tri[1]];
    vtkVertex3f& p3 = this->Points[tri[2]];
    vtkVector3f u = p2.point - p1.point;
    vtkVector3f v = p3.point - p1.point;
    vtkVector3f n;
    n[0] = u[1]*v[2] - u[2]*v[1];
    n[1] = u[2]*v[0] - u[0]*v[2];
    n[2] = u[0]*v[1] - u[1]*v[0];
    p1.normal += n;
    p2.normal += n;
    p3.normal += n;
    }
  for (size_t i = 0; i < this->Points.size(); ++i)
    {
    vtkVector3f& n = this->Points[i].normal;
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