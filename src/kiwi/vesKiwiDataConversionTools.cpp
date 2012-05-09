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

#include "vesKiwiDataConversionTools.h"

// VES includes
#include "vtkCellArray.h"
#include "vtkDiscretizableColorTransferFunction.h"
#include "vesGeometryData.h"
#include "vesGLTypes.h"
#include "vtkLookupTable.h"
#include "vesMath.h"
#include "vtkNew.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vesTexture.h"
#include "vtkUnsignedCharArray.h"

// C/C++ includes
#include <cassert>

//----------------------------------------------------------------------------
vtkDataArray* vesKiwiDataConversionTools::FindScalarsArray(vtkDataSet* dataSet)
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
vtkUnsignedCharArray* vesKiwiDataConversionTools::FindRGBColorsArray(vtkDataSet* dataSet)
{
  vtkUnsignedCharArray* colors = vtkUnsignedCharArray::SafeDownCast(dataSet->GetPointData()->GetArray("rgb_colors"));
  if (colors && colors->GetNumberOfComponents() == 3)
    {
    return colors;
    }
  return 0;
}

//----------------------------------------------------------------------------
vtkDataArray* vesKiwiDataConversionTools::FindTextureCoordinatesArray(vtkDataSet* dataSet)
{
  vtkDataArray* tcoords = dataSet->GetPointData()->GetArray("tcoords");
  if (tcoords && tcoords->GetNumberOfComponents() == 2)
    {
    return tcoords;
    }
  return 0;
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkDiscretizableColorTransferFunction> vesKiwiDataConversionTools::GetBlackBodyRadiationColorMap(double scalarRange[2])
{
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
vtkSmartPointer<vtkLookupTable> vesKiwiDataConversionTools::GetRedToBlueLookupTable(double scalarRange[2])
{
  vtkSmartPointer<vtkLookupTable> table = vtkSmartPointer<vtkLookupTable>::New();
  table->SetRange(scalarRange);
  table->SetHueRange(0, 0.666);
  table->Build();
  return table;
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkLookupTable> vesKiwiDataConversionTools::GetGrayscaleLookupTable(double scalarRange[2])
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
void vesKiwiDataConversionTools::SetVertexColors(
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
    colorSourceData->pushBack(color);
    }

  geometryData->addSource(colorSourceData);
}

//----------------------------------------------------------------------------
void vesKiwiDataConversionTools::SetVertexColors(vtkDataArray* scalars,
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
    colorSourceData->pushBack(color);
    }

  geometryData->addSource(colorSourceData);
}

//----------------------------------------------------------------------------
void vesKiwiDataConversionTools::SetTextureCoordinates(vtkDataArray* tcoords,
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
    texCoordSourceData->pushBack(textureCoordinate);
    }

  geometryData->addSource(texCoordSourceData);
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkUnsignedCharArray> vesKiwiDataConversionTools::MapScalars(
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
void vesKiwiDataConversionTools::SetTextureData(vtkUnsignedCharArray* pixels,
  vesSharedPtr<vesTexture> texture, int width, int height)
{
  assert(texture);
  assert(pixels);
  assert(pixels->GetNumberOfTuples() == width*height);

  vesImage::Ptr image (new vesImage());
  image->setWidth(width);
  image->setHeight(height);
  image->setPixelFormat(  pixels->GetNumberOfComponents() == 4 ? vesColorDataType::RGBA
                        : pixels->GetNumberOfComponents() == 3 ? vesColorDataType::RGB
                        : vesColorDataType::Luminance);
  image->setPixelDataType(vesColorDataType::UnsignedByte);
  image->setData(pixels->WriteVoidPointer(0, 0), pixels->GetSize());

  texture->setImage(image);
}

//----------------------------------------------------------------------------
void vesKiwiDataConversionTools::ConvertTriangles(
  vtkPolyData* input, vesSharedPtr<vesGeometryData> output)
{
  if (!input || !output) {
    return;
  }

  const vtkIdType maximumNumberOfPoints = 65536;

  if (input->GetNumberOfPoints() > maximumNumberOfPoints) {
    vesKiwiDataConversionTools::GenericConvertTriangles<unsigned int>(input, output);
  }
  else {
    vesKiwiDataConversionTools::GenericConvertTriangles<unsigned short>(input, output);
  }
}

//----------------------------------------------------------------------------
template<typename T>
void vesKiwiDataConversionTools::GenericConvertTriangles(vtkPolyData* input,
  vesSharedPtr<vesGeometryData> output)
{
  vesSourceDataP3N3f::Ptr sourceData (new vesSourceDataP3N3f());

  double inPoint[3];
  for (int i = 0; i < input->GetNumberOfPoints(); ++i){
    input->GetPoint(i, inPoint);

    vesVertexDataP3N3f vertexData;
    vertexData.m_position = vesVector3f(inPoint[0], inPoint[1], inPoint[2]);
    sourceData->pushBack(vertexData);
  }

  // copy triangles in place to ves structure
  vtkCellArray* polys = input->GetPolys();
  vtkIdType num;
  vtkIdType* vertices;

  vesSharedPtr< vesIndices<T> > indicesObj =
    std::tr1::static_pointer_cast< vesIndices<T> >
    (output->triangles()->getVesIndices());

  typename vesIndices<T>::Indices* triangleIndices
    = indicesObj->indices();

  triangleIndices->clear();
  triangleIndices->resize(polys->GetNumberOfCells());

  T* outIndex = &triangleIndices->front();
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
      sourceData->arrayReference()[i].m_normal[0] = normals->GetTuple(i)[0];
      sourceData->arrayReference()[i].m_normal[1] = normals->GetTuple(i)[1];
      sourceData->arrayReference()[i].m_normal[2] = normals->GetTuple(i)[2];
    }
  }
  else
  {
    output->computeNormals<T>();
  }

  output->computeBounds();
  output->addSource(sourceData);
}

//-----------------------------------------------------------------------------
vesSharedPtr<vesGeometryData> vesKiwiDataConversionTools::ConvertPoints(vtkPolyData* input)
{
  vesSharedPtr<vesGeometryData> output(new vesGeometryData());
  vesSourceDataP3f::Ptr sourceData(new vesSourceDataP3f());

  vesVertexDataP3f vertexData;
  for (int i = 0; i < input->GetNumberOfPoints(); ++i){
    vertexData.m_position[0] = input->GetPoint(i)[0];
    vertexData.m_position[1] = input->GetPoint(i)[1];
    vertexData.m_position[2] = input->GetPoint(i)[2];
    sourceData->pushBack(vertexData);
  }

  output->addSource(sourceData);
  output->setName("PolyData");

  // Add point primitive
  vesPrimitive::Ptr pointPrimitive (new vesPrimitive());
  pointPrimitive->setPrimitiveType(vesPrimitiveRenderType::Points);
  pointPrimitive->setIndexCount(1);
  output->addPrimitive(pointPrimitive);

  return output;
}

//-----------------------------------------------------------------------------
vesSharedPtr<vesGeometryData> vesKiwiDataConversionTools::Convert(
  vtkPolyData* input)
{
  const vtkIdType maximumNumberOfPoints = 65536;

  if (input->GetNumberOfPoints() > maximumNumberOfPoints) {
    return vesKiwiDataConversionTools::GenericConvert<unsigned int>(input);
  }
  else {
    return vesKiwiDataConversionTools::GenericConvert<unsigned short>(input);
  }
}

//-----------------------------------------------------------------------------
template<typename T>
  vesSharedPtr<vesGeometryData> vesKiwiDataConversionTools::GenericConvert(
  vtkPolyData* input)
{
  unsigned int indicesValuesType;
  if(typeid(T) == typeid(unsigned int)) {
    indicesValuesType = vesPrimitiveIndicesValueType::UnsignedInt;
  }
  // Default
  else {
    indicesValuesType = vesPrimitiveIndicesValueType::UnsignedShort;
  }

  vesPrimitive::Ptr trianglesPrimitive;
  vesSharedPtr< vesIndices<T> > triangleIndices;
  vesPrimitive::Ptr triangleStripsPrimitive;
  vesSharedPtr< vesIndices<T> > triangleStripsIndices;
  vesPrimitive::Ptr linesPrimitive;
  vesSharedPtr< vesIndices<T> > linesIndices;
  vesPrimitive::Ptr verticesPrimitive;
  vesSharedPtr< vesIndices<T> > verticesIndices;

  vesSharedPtr<vesGeometryData> output =
    vesSharedPtr<vesGeometryData>(new vesGeometryData());
  vesSourceDataP3N3f::Ptr sourceData (new vesSourceDataP3N3f());

  vesVertexDataP3N3f vertexData;
  for (int i = 0; i < input->GetNumberOfPoints(); ++i){
    vertexData.m_position[0] = input->GetPoint(i)[0];
    vertexData.m_position[1] = input->GetPoint(i)[1];
    vertexData.m_position[2] = input->GetPoint(i)[2];
    vertexData.m_normal[0] = 1.0f;
    vertexData.m_normal[1] = 0.0f;
    vertexData.m_normal[2] = 0.0f;
    sourceData->pushBack(vertexData);
  }

  output->addSource(sourceData);
  output->setName("PolyData");

  vtkIdType num;
  vtkIdType* vertices;

  // Add triangles
  vtkCellArray* polys = input->GetPolys();
  polys->InitTraversal();

  if (polys->GetNumberOfCells() > 0) {
    triangleIndices = vesSharedPtr< vesIndices<T> >(new vesIndices<T>());
    trianglesPrimitive = vesPrimitive::Ptr(new vesPrimitive());
    trianglesPrimitive->setIndexCount(3);
    trianglesPrimitive->setIndicesValueType(indicesValuesType);
    trianglesPrimitive->setPrimitiveType(vesPrimitiveRenderType::Triangles);
    trianglesPrimitive->setVesIndices(triangleIndices);

    output->addPrimitive(trianglesPrimitive);

    for (int i = 0; i < polys->GetNumberOfCells(); ++i) {
      polys->GetNextCell(num, vertices);
      if (num == 3) {
        triangleIndices->pushBackIndices(vertices[0], vertices[1], vertices[2]);
      }
      else if (num == 4) {
        triangleIndices->pushBackIndices(vertices[0], vertices[1], vertices[2]);
        triangleIndices->pushBackIndices(vertices[3], vertices[0], vertices[2]);
      }
    }
  }

  // Add triangle strips
  vtkCellArray* strips = input->GetStrips();
  strips->InitTraversal();

  if (strips->GetNumberOfCells() > 0) {
    triangleStripsIndices
      = vesSharedPtr< vesIndices<T> >( new vesIndices<T>() );
    triangleStripsPrimitive = vesPrimitive::Ptr(new vesPrimitive());
    triangleStripsPrimitive->setIndexCount(1);
    triangleStripsPrimitive->setIndicesValueType(indicesValuesType);
    triangleStripsPrimitive->setPrimitiveType(vesPrimitiveRenderType::TriangleStrip);
    triangleStripsPrimitive->setVesIndices(triangleIndices);

    output->addPrimitive(triangleStripsPrimitive);

    for (int i = 0; i < strips->GetNumberOfCells(); ++i) {
      strips->GetNextCell(num, vertices);
      for (int i = 2; i < num; ++i)
      {
        if (i & 1)
        {
          triangleStripsIndices->pushBackIndices(vertices[i-1], vertices[i-2], vertices[i]);
        }
        else
        {
          triangleStripsIndices->pushBackIndices(vertices[i-2], vertices[i-1], vertices[i]);
        }
      }
    }
  }

  // Add lines
  vtkCellArray* lines = input->GetLines();
  lines->InitTraversal();

  if (lines->GetNumberOfCells() > 0) {
    linesIndices = vesSharedPtr< vesIndices<T> >(new vesIndices<T>());
    linesPrimitive = vesPrimitive::Ptr(new vesPrimitive());
    linesPrimitive->setIndexCount(2);
    linesPrimitive->setIndicesValueType(indicesValuesType);
    linesPrimitive->setPrimitiveType(vesPrimitiveRenderType::Lines);
    linesPrimitive->setVesIndices(linesIndices);

    output->addPrimitive(linesPrimitive);

    for (int i = 0; i < lines->GetNumberOfCells(); ++i) {
      lines->GetNextCell(num, vertices);
      for (int i = 1; i < num; ++i)
      {
        linesIndices->pushBackIndices(vertices[i-1], vertices[i]);
      }
    }
  }

  // Add verts
  vtkCellArray* verts = input->GetVerts();
  verts->InitTraversal();

  if (verts->GetNumberOfCells() > 0) {
    verticesIndices = vesSharedPtr< vesIndices<T> >(new vesIndices<T>());
    verticesPrimitive = vesPrimitive::Ptr(new vesPrimitive());
    verticesPrimitive->setIndexCount(1);
    verticesPrimitive->setIndicesValueType(indicesValuesType);
    verticesPrimitive->setPrimitiveType(vesPrimitiveRenderType::Points);
    verticesPrimitive->setVesIndices(verticesIndices);

    output->addPrimitive(verticesPrimitive);

    for (int i = 0; i < verts->GetNumberOfCells() && i < 65000; ++i) {
      verts->GetNextCell(num, vertices);
      verticesIndices->pushBackIndices(vertices[0]);
    }
  }

  if (input->GetPointData()->GetNormals()) {
    vtkDataArray* normals = input->GetPointData()->GetNormals();
    for (int i = 0; i < input->GetNumberOfPoints(); ++i) {
      sourceData->arrayReference()[i].m_normal[0] = normals->GetTuple(i)[0];
      sourceData->arrayReference()[i].m_normal[1] = normals->GetTuple(i)[1];
      sourceData->arrayReference()[i].m_normal[2] = normals->GetTuple(i)[2];
    }
  }
  else
  {
    output->computeNormals<T>();
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
