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

#include "vesPolyDataToTriangleData.h"

#include "vtkCellArray.h"
#include "vesTriangleData.h"
#include "vesGMTL.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"

void vesPolyDataToTriangleData::ConvertTriangles(vtkPolyData* input, vesTriangleData* output)
{
  if (!input || !output)
  {
    return;
  }

  // copy points in place to ves structure
  output->GetPoints().resize(input->GetNumberOfPoints());
  vtkVertex3f* v = &output->GetPoints()[0];
  double inPoint[3];
  for (int i = 0; i < input->GetNumberOfPoints(); ++i, ++v){
    input->GetPoint(i, inPoint);
    v->point[0] = inPoint[0];
    v->point[1] = inPoint[1];
    v->point[2] = inPoint[2];
  }

  // copy triangles in place to ves structure
  vtkCellArray* polys = input->GetPolys();
  vtkIdType num;
  vtkIdType* vertices;
  output->GetTriangles().resize(polys->GetNumberOfCells());
  unsigned short* outIndex = &output->GetTriangles()[0][0];
  for (int i = 0; i < polys->GetNumberOfCells(); ++i)
  {
    // there are 4 elements for each triangle cell in the array (count, i1, i2, i3)
    polys->GetCell(4*i, num, vertices);
    *outIndex++ = vertices[0];
    *outIndex++ = vertices[1];
    *outIndex++ = vertices[2];
  }

  if (input->GetPointData()->GetNormals())
  {
    vtkDataArray* normals = input->GetPointData()->GetNormals();
    v = &output->GetPoints()[0];
    for (int i = 0; i < input->GetNumberOfPoints(); ++i, ++v)
    {
      v->normal[0] = normals->GetTuple(i)[0];
      v->normal[1] = normals->GetTuple(i)[1];
      v->normal[2] = normals->GetTuple(i)[2];
    }
  }
  else
  {
    output->SetHasNormals(false);
    output->ComputeNormals();
  }
  output->SetHasBounds(false);
  output->ComputeBounds();
}

vesTriangleData* vesPolyDataToTriangleData::Convert(vtkPolyData* input)
{
  //cerr << "starting conversion" << endl;
  vesTriangleData* output = new vesTriangleData();
  output->GetPoints().resize(input->GetNumberOfPoints());
  vtkVertex3f* v = &output->GetPoints()[0];
  for (int i = 0; i < input->GetNumberOfPoints(); ++i, ++v){
    v->point[0] = input->GetPoint(i)[0];
    v->point[1] = input->GetPoint(i)[1];
    v->point[2] = input->GetPoint(i)[2];
  }

  vtkCellArray* polys = input->GetPolys();
  vtkIdType num;
  vtkIdType* vertices;
  polys->InitTraversal();
  for (int i = 0; i < polys->GetNumberOfCells(); ++i) {
    polys->GetNextCell(num, vertices);
    if (num == 3) {
      vesVector3us indices;
      indices[0] = vertices[0];
      indices[1] = vertices[1];
      indices[2] = vertices[2];
      output->GetTriangles().push_back(indices);
    }
    else if (num == 4) {
      vesVector3us indices1;
      vesVector3us indices2;
      indices1[0] = vertices[0];
      indices1[1] = vertices[1];
      indices1[2] = vertices[2];
      indices2[0] = vertices[3];
      indices2[1] = vertices[0];
      indices2[2] = vertices[2];
      output->GetTriangles().push_back(indices1);
      output->GetTriangles().push_back(indices2);
    }
  }

  vtkCellArray* strips = input->GetStrips();
  strips->InitTraversal();
  for (int i = 0; i < strips->GetNumberOfCells(); ++i) {
    strips->GetNextCell(num, vertices);
    for (int i = 2; i < num; ++i)
    {
      vesVector3us indices;
      if (i & 1)
      {
        indices[0] = vertices[i-1];
        indices[1] = vertices[i-2];
        indices[2] = vertices[i];
      }
      else
      {
        indices[0] = vertices[i-2];
        indices[1] = vertices[i-1];
        indices[2] = vertices[i];
      }
      output->GetTriangles().push_back(indices);
    }
  }

  vtkCellArray* lines = input->GetLines();
  lines->InitTraversal();
  for (int i = 0; i < lines->GetNumberOfCells(); ++i) {
    lines->GetNextCell(num, vertices);
    for (int i = 1; i < num; ++i)
    {
      vesVector2us indices;
      indices[0] = vertices[i-1];
      indices[1] = vertices[i];
      output->GetLines().push_back(indices);
    }
  }

  if (input->GetPointData()->GetNormals()) {
    vtkDataArray* normals = input->GetPointData()->GetNormals();
    v = &output->GetPoints()[0];
    for (int i = 0; i < input->GetNumberOfPoints(); ++i, ++v) {
      v->normal[0] = normals->GetTuple(i)[0];
      v->normal[1] = normals->GetTuple(i)[1];
      v->normal[2] = normals->GetTuple(i)[2];
    }
  } else {
    output->ComputeNormals();
  }

  // Note- the PDB reader assigns a 3 component 'rgb_colors' array as point scalars,
  // so make sure we check the number of components
  vtkDataArray* scalars = input->GetPointData()->GetScalars();
  if (scalars && scalars->GetNumberOfComponents() == 1)
  {
    for (int i = 0; i < input->GetNumberOfPoints(); ++i)
    {
      output->GetPointScalars().push_back(static_cast<float>(scalars->GetTuple1(i)));
    }
  }

  //cerr << "done with conversion" << endl;
  return output;
}
