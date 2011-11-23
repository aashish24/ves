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

#include "vesDataConversionTools.h"
#include "vesTexture.h"

#include "vtkCellArray.h"
#include "vesGeometryData.h"
#include "vesGMTL.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkNew.h"
#include "vtkUnsignedCharArray.h"
#include "vtkLookupTable.h"
#include "vtkDiscretizableColorTransferFunction.h"

#include <cassert>


//----------------------------------------------------------------------------
vtkDataArray* vesDataConversionTools::FindScalarsArray(vtkDataSet* dataSet)
{
  vtkDataArray* scalars = dataSet->GetPointData()->GetScalars();
  if (scalars && scalars->GetNumberOfComponents() == 1)
    {
    return scalars;
    }

  for (vtkIdType i = 0; i < dataSet->GetPointData()->GetNumberOfArrays(); ++i)
    {
    scalars = dataSet->GetPointData()->GetArray(i);
    if (scalars && scalars->GetNumberOfComponents() == 1)
      {
      return scalars;
      }
    }

  return 0;
}

//----------------------------------------------------------------------------
vtkUnsignedCharArray* vesDataConversionTools::FindRGBColorsArray(vtkDataSet* dataSet)
{
  vtkUnsignedCharArray* colors = vtkUnsignedCharArray::SafeDownCast(dataSet->GetPointData()->GetArray("rgb_colors"));
  if (colors && colors->GetNumberOfComponents() == 3)
    {
    return colors;
    }
  return 0;
}

//----------------------------------------------------------------------------
vtkDataArray* vesDataConversionTools::FindTextureCoordinatesArray(vtkDataSet* dataSet)
{
  vtkDataArray* tcoords = dataSet->GetPointData()->GetArray("tcoords");
  if (tcoords && tcoords->GetNumberOfComponents() == 2)
    {
    return tcoords;
    }
  return 0;
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkDiscretizableColorTransferFunction> GetBlackBodyRadiationColorMap(double scalarRange[2])
{
  //double range[2] = {-500, 6000};
  double length = scalarRange[1] - scalarRange[0];
  double points[4] = {0.0, 0.4, 0.75, 1.0};

  vtkSmartPointer<vtkDiscretizableColorTransferFunction> function
    = vtkSmartPointer<vtkDiscretizableColorTransferFunction>::New();
  function->DiscretizeOn();
  function->SetColorSpaceToRGB();
  function->SetNumberOfValues(256);
  function->AddRGBPoint(scalarRange[0] + points[0]*length, 0, 0, 0);
  function->AddRGBPoint(scalarRange[0] + points[1]*length, 1, 0, 0);
  function->AddRGBPoint(scalarRange[0] + points[2]*length, 1, 1, 0);
  function->AddRGBPoint(scalarRange[0] + points[3]*length, 1, 1, 1);
  function->Build();
  return function;
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkLookupTable> vesDataConversionTools::GetRedToBlueLookupTable(double scalarRange[2])
{
  vtkSmartPointer<vtkLookupTable> table = vtkSmartPointer<vtkLookupTable>::New();
  table->SetRange(scalarRange);
  table->SetHueRange(0, 0.666);
  table->Build();
  return table;
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkLookupTable> vesDataConversionTools::GetGrayscaleLookupTable(double scalarRange[2])
{
  vtkSmartPointer<vtkLookupTable> table = vtkSmartPointer<vtkLookupTable>::New();
  table->SetRange(scalarRange);
  table->SetValueRange(0.0, 1.0);
  table->SetSaturationRange(0.0, 0.0);
  table->SetHueRange(0.0, 0.0);
  table->Build();
  return table;
}

//----------------------------------------------------------------------------
void vesDataConversionTools::SetVertexColors(
  vtkUnsignedCharArray* colors, vesSharedPtr<vesGeometryData> geometryData)
{
  assert(geometryData);
  assert(colors);
  assert(colors->GetNumberOfComponents() == 3);

  unsigned char rgb[3];
  const size_t nTuples = colors->GetNumberOfTuples();

  vesSourceDataC3f::Ptr colorSourceData (new vesSourceDataC3f());
  for (size_t i = 0; i < nTuples; ++i)
    {
    colors->GetTupleValue(i, rgb);
    vesVertexDataC3f color;
    color.m_color = vesVector3f(rgb[0]/255.0, rgb[1]/255.0, rgb[2]/255.0);
    colorSourceData->m_data.push_back(color);
    }

  geometryData->m_sources.push_back(colorSourceData);
}

//----------------------------------------------------------------------------
void vesDataConversionTools::SetVertexColors(vtkDataArray* scalars,
  vtkScalarsToColors* scalarsToColors, vesSharedPtr<vesGeometryData> geometryData)
{
  assert(scalars);
  assert(scalars->GetNumberOfComponents() == 1);
  assert(geometryData);

  double rgb[3];
  const size_t nTuples = scalars->GetNumberOfTuples();

  vesSourceDataC3f::Ptr colorSourceData (new vesSourceDataC3f());

  for (size_t i = 0; i < nTuples; ++i)
    {
    scalarsToColors->GetColor(scalars->GetComponent(i, 0), rgb);
    vesVertexDataC3f color;
    color.m_color = vesVector3f(rgb[0], rgb[1], rgb[2]);
    colorSourceData->m_data.push_back(color);
    }

  geometryData->m_sources.push_back(colorSourceData);
}

//----------------------------------------------------------------------------
void vesDataConversionTools::SetTextureCoordinates(vtkDataArray* tcoords,
  vesSharedPtr<vesGeometryData> geometryData)
{
  assert(tcoords);
  assert(tcoords->GetNumberOfComponents() == 2);
  assert(geometryData);

  const size_t nTuples = tcoords->GetNumberOfTuples();

  vesSourceDataT2f::Ptr texCoordSourceData (new vesSourceDataT2f());

  for (size_t i = 0; i < nTuples; ++i)
    {
    double* values = tcoords->GetTuple(i);
    vesVertexDataT2f textureCoordinate;
    textureCoordinate.m_textureCoordinate = vesVector2f(values[0], values[1]);
    texCoordSourceData->m_data.push_back(textureCoordinate);
    }

  geometryData->m_sources.push_back(texCoordSourceData);
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkUnsignedCharArray> vesDataConversionTools::MapScalars(
vtkDataArray* scalars, vtkScalarsToColors* scalarsToColors)
{
  assert(scalars->GetNumberOfComponents() == 1);

  vtkSmartPointer<vtkUnsignedCharArray> colors
    = vtkSmartPointer<vtkUnsignedCharArray>::New();
  colors->SetNumberOfComponents(4);
  colors->SetNumberOfTuples(scalars->GetNumberOfTuples());

  double rgb[3];
  const size_t nTuples = scalars->GetNumberOfTuples();
  for (size_t i = 0; i < nTuples; ++i)
    {
    scalarsToColors->GetColor(scalars->GetComponent(i, 0), rgb);
    colors->SetTuple4(i, rgb[0]*255, rgb[1]*255, rgb[2]*255, 255);
    }
  return colors;
}

//----------------------------------------------------------------------------
void vesDataConversionTools::SetTextureData(vtkUnsignedCharArray* pixels,
  vesSharedPtr<vesTexture> texture, int width, int height)
{
  assert(pixels);
  assert(pixels->GetNumberOfComponents() == 4);
  assert(pixels->GetNumberOfTuples() == width*height);

  vesImage image;
  image.m_width = width;
  image.m_height = height;
  image.m_pixelFormat = vesColorDataType::RGBA;
  image.m_pixelDataType = vesColorDataType::UnsignedByte;
  image.m_data = pixels->WriteVoidPointer(0, 0);

  texture->setImage(image);
}

//----------------------------------------------------------------------------
void vesDataConversionTools::ConvertTriangles(
  vtkPolyData* input, vesSharedPtr<vesGeometryData> output)
{
  if (!input || !output)
  {
    return;
  }

  vesSourceDataP3N3f::Ptr sourceData (new vesSourceDataP3N3f());

  double inPoint[3];
  for (int i = 0; i < input->GetNumberOfPoints(); ++i){
    input->GetPoint(i, inPoint);

    vesVertexDataP3N3f vertexData;
    vertexData.m_position = vesVector3f(inPoint[0], inPoint[1], inPoint[2]);
    sourceData->m_data.push_back(vertexData);
  }

  // copy triangles in place to ves structure
  vtkCellArray* polys = input->GetPolys();
  vtkIdType num;
  vtkIdType* vertices;

  vesPrimitive::Indices* triangleIndices
    = output->triangles()->indices();

  triangleIndices->clear();
  triangleIndices->resize(polys->GetNumberOfCells());

  unsigned short* outIndex = &triangleIndices->front();
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
    for (int i = 0; i < input->GetNumberOfPoints(); ++i)
    {
      sourceData->m_data[i].m_normal[0] = normals->GetTuple(i)[0];
      sourceData->m_data[i].m_normal[1] = normals->GetTuple(i)[1];
      sourceData->m_data[i].m_normal[2] = normals->GetTuple(i)[2];
    }
  }
  else
  {
    output->computeNormals();
  }

  output->computeBounds();
  output->m_sources.push_back(sourceData);
}

vesSharedPtr<vesGeometryData> vesDataConversionTools::Convert(vtkPolyData* input)
{
  vesPrimitive::Ptr triangles(new vesPrimitive());
  vesPrimitive::Ptr triangleStrips(new vesPrimitive());
  vesPrimitive::Ptr lines(new vesPrimitive());

  vesSharedPtr<vesGeometryData> output =
    vesSharedPtr<vesGeometryData>(new vesGeometryData());
  vesSourceDataP3N3f::Ptr sourceData (new vesSourceDataP3N3f());

  vesVertexDataP3N3f vertexData;
  for (int i = 0; i < input->GetNumberOfPoints(); ++i){
    vertexData.m_position[0] = input->GetPoint(i)[0];
    vertexData.m_position[1] = input->GetPoint(i)[1];
    vertexData.m_position[2] = input->GetPoint(i)[2];
    sourceData->m_data.push_back(vertexData);
  }

  output->m_sources.push_back(sourceData);
  output->m_name = "PolyData";

  vtkCellArray* polys = input->GetPolys();
  vtkIdType num;
  vtkIdType* vertices;
  polys->InitTraversal();
  for (int i = 0; i < polys->GetNumberOfCells(); ++i) {
    polys->GetNextCell(num, vertices);
    if (num == 3) {
      triangles->pushBackIndices(vertices[0], vertices[1], vertices[2]);
    }
    else if (num == 4) {
      triangles->pushBackIndices(vertices[0], vertices[1], vertices[2]);
      triangles->pushBackIndices(vertices[3], vertices[0], vertices[2]);
    }
  }

  triangles->setIndexCount(3);
  triangles->setPrimitiveType(GL_TRIANGLES);

  vtkCellArray* strips = input->GetStrips();
  strips->InitTraversal();
  for (int i = 0; i < strips->GetNumberOfCells(); ++i) {
    strips->GetNextCell(num, vertices);
    for (int i = 2; i < num; ++i)
    {
      if (i & 1)
      {
        triangleStrips->pushBackIndices(vertices[i-1], vertices[i-2], vertices[i]);
      }
      else
      {
        triangleStrips->pushBackIndices(vertices[i-2], vertices[i-1], vertices[i]);
      }
    }
  }

  triangleStrips->setIndexCount(1);
  triangleStrips->setPrimitiveType(GL_TRIANGLE_STRIP);

  vtkCellArray* vtklines = input->GetLines();
  vtklines->InitTraversal();
  for (int i = 0; i < vtklines->GetNumberOfCells(); ++i) {
    vtklines->GetNextCell(num, vertices);
    for (int i = 1; i < num; ++i)
    {
      lines->pushBackIndices(vertices[i-1], vertices[i]);
    }
  }

  lines->setIndexCount(2);
  lines->setPrimitiveType(GL_LINES);

  output->m_primitives.push_back(triangles);
  output->m_primitives.push_back(triangleStrips);
  output->m_primitives.push_back(lines);

  if (input->GetPointData()->GetNormals()) {
    vtkDataArray* normals = input->GetPointData()->GetNormals();
    for (int i = 0; i < input->GetNumberOfPoints(); ++i) {
      sourceData->m_data[i].m_normal[0] = normals->GetTuple(i)[0];
      sourceData->m_data[i].m_normal[1] = normals->GetTuple(i)[1];
      sourceData->m_data[i].m_normal[2] = normals->GetTuple(i)[2];
    }
  }
  else
  {
    output->computeNormals();
  }

#if 0
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
#endif
  return output;
}
