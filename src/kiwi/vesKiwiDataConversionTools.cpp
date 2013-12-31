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
#include "vesGeometryData.h"
#include "vesGLTypes.h"
#include "vesImage.h"
#include "vesTexture.h"
#include "vesMath.h"

#include "vesKiwiOptions.h"
#ifdef VES_USE_CURL
#  include "vesPVWebDataSet.h"
#endif // VES_USE_CURL

#include "vtkCellArray.h"
#include "vtkDiscretizableColorTransferFunction.h"
#include "vtkNew.h"
#include "vtkImageData.h"
#include "vtkLookupTable.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkUnsignedCharArray.h"

#include "vtkTriangleFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkShrinkPolyData.h"

#include <cassert>
#include <vector>

//----------------------------------------------------------------------------
vtkDataArray* vesKiwiDataConversionTools::FindScalarsArray(vtkDataSet* dataSet)
{
  std::vector<vtkDataArray*> arrays = FindScalarArrays(dataSet);
  if (arrays.size()) {
    return arrays[0];
  }

  return 0;
}

//----------------------------------------------------------------------------
std::vector<vtkDataArray*> vesKiwiDataConversionTools::FindScalarArrays(vtkDataSet* dataSet)
{
  std::vector<vtkDataArray*> arrays;
  if (!dataSet) {
    return arrays;
  }

  vtkDataArray* mainScalars = dataSet->GetPointData()->GetScalars();
  vtkDataArray* tcoords = vesKiwiDataConversionTools::FindTextureCoordinatesArray(dataSet);
  vtkDataArray* normals = dataSet->GetPointData()->GetNormals();

  if (mainScalars && mainScalars != tcoords && mainScalars != normals) {
    arrays.push_back(mainScalars);
  }

  for (vtkIdType i = 0; i < dataSet->GetPointData()->GetNumberOfArrays(); ++i) {
    vtkDataArray* scalars = dataSet->GetPointData()->GetArray(i);
    if (scalars && (scalars != mainScalars) && (scalars != tcoords) && (scalars != normals)) {
      arrays.push_back(scalars);
    }
  }

  return arrays;
}

//----------------------------------------------------------------------------
vtkUnsignedCharArray* vesKiwiDataConversionTools::FindRGBColorsArray(vtkDataSet* dataSet)
{
  for (int i = 0; i < dataSet->GetPointData()->GetNumberOfArrays(); ++i) {
    vtkUnsignedCharArray* colors = vtkUnsignedCharArray::SafeDownCast(dataSet->GetPointData()->GetArray(i));
    if (colors && (colors->GetNumberOfComponents() == 3
                   ||  colors->GetNumberOfComponents() == 4)) {
      return colors;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
vtkDataArray* vesKiwiDataConversionTools::FindTextureCoordinatesArray(vtkDataSet* dataSet)
{
  vtkDataArray* tcoords = dataSet->GetPointData()->GetTCoords();
  if (!tcoords) {
    tcoords = dataSet->GetPointData()->GetArray("tcoords");

    if (!tcoords) {
      tcoords = dataSet->GetPointData()->GetArray("TextureCoordinates");
    }
  }

  if (tcoords && tcoords->GetNumberOfComponents() == 2) {
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
vtkSmartPointer<vtkDiscretizableColorTransferFunction> vesKiwiDataConversionTools::GetCoolToWarmLookupTable(double scalarRange[2])
{
  double length = scalarRange[1] - scalarRange[0];
  double points[4] = {0.0, 1.0};

  vtkSmartPointer<vtkDiscretizableColorTransferFunction> function
    = vtkSmartPointer<vtkDiscretizableColorTransferFunction>::New();
  function->DiscretizeOn();
  function->SetColorSpaceToDiverging();
  function->SetNumberOfValues(256);
  function->AddRGBPoint(scalarRange[0] + points[0]*length,  0.23, 0.299, 0.754);
  function->AddRGBPoint(scalarRange[0] + points[1]*length, 0.706, 0.016, 0.15);
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
vtkSmartPointer<vtkLookupTable> vesKiwiDataConversionTools::GetBlueToRedLookupTable(double scalarRange[2])
{
  vtkSmartPointer<vtkLookupTable> table = vtkSmartPointer<vtkLookupTable>::New();
  table->SetRange(scalarRange);
  table->SetHueRange(0.666, 0.0);
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
vtkSmartPointer<vtkScalarsToColors> vesKiwiDataConversionTools::GetPresetColorMap(const std::string& colorMapName, double scalarRange[2])
{
  if (colorMapName == "CoolToWarm") {
    return GetCoolToWarmLookupTable(scalarRange);
  }
  else if (colorMapName == "BlueToRed") {
    return GetBlueToRedLookupTable(scalarRange);
  }
  else if (colorMapName == "RedToBlue") {
    return GetRedToBlueLookupTable(scalarRange);
  }
  else if (colorMapName == "Grayscale") {
    return GetGrayscaleLookupTable(scalarRange);
  }
  else if (colorMapName == "BlackBodyRadiation") {
    return GetBlackBodyRadiationColorMap(scalarRange);
  }
  return 0;
}

//----------------------------------------------------------------------------
vesSourceData::Ptr vesKiwiDataConversionTools::ConvertColors(vtkUnsignedCharArray* colors)
{
  if (colors && colors->GetNumberOfComponents() == 3) {
    unsigned char rgb[3];
    const size_t nTuples = colors->GetNumberOfTuples();

    vesSourceDataC3f::Ptr colorSourceData(new vesSourceDataC3f());
    for (size_t i = 0; i < nTuples; ++i) {
      colors->GetTupleValue(i, rgb);
      vesVertexDataC3f color;
      color.m_color = vesVector3f(rgb[0]/255.0, rgb[1]/255.0, rgb[2]/255.0);
      colorSourceData->pushBack(color);
    }
    return colorSourceData;
  }
  else if (colors && colors->GetNumberOfComponents() == 4) {

    // Ignore alpha for now.
    // Currently, the shaders expect a 3 component rgb array

    unsigned char rgb[4];
    const size_t nTuples = colors->GetNumberOfTuples();

    vesSourceDataC3f::Ptr colorSourceData(new vesSourceDataC3f());
    for (size_t i = 0; i < nTuples; ++i) {
      colors->GetTupleValue(i, rgb);
      vesVertexDataC3f color;
      color.m_color = vesVector3f(rgb[0]/255.0, rgb[1]/255.0, rgb[2]/255.0);
      colorSourceData->pushBack(color);
    }
    return colorSourceData;
  }

  return vesSourceDataC3f::Ptr();
}

//----------------------------------------------------------------------------
vesSourceData::Ptr vesKiwiDataConversionTools::ConvertScalarsToColors(vtkDataArray* scalars, vtkScalarsToColors* scalarsToColors)
{
  if (!scalars || !scalarsToColors) {
    return vesSourceDataC3f::Ptr();
  }

  scalarsToColors->SetVectorModeToMagnitude();

  vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::Take(
    scalarsToColors->MapScalars(scalars, VTK_COLOR_MODE_MAP_SCALARS, -1));

  unsigned char rgb[4];
  const size_t nTuples = colors->GetNumberOfTuples();

  vesSourceDataC3f::Ptr colorSourceData(new vesSourceDataC3f());
  for (size_t i = 0; i < nTuples; ++i) {
    colors->GetTupleValue(i, rgb);
    vesVertexDataC3f color;
    color.m_color = vesVector3f(rgb[0]/255.0, rgb[1]/255.0, rgb[2]/255.0);
    colorSourceData->pushBack(color);
  }
  return colorSourceData;
}

//----------------------------------------------------------------------------
vesSourceDataT2f::Ptr vesKiwiDataConversionTools::ConvertTCoords(vtkDataArray* tcoords)
{
  if (!tcoords || tcoords->GetNumberOfComponents() != 2) {
    return vesSourceDataT2f::Ptr();
  }

  const size_t nTuples = tcoords->GetNumberOfTuples();
  vesSourceDataT2f::Ptr texCoordSourceData(new vesSourceDataT2f());

  for (size_t i = 0; i < nTuples; ++i) {
    double* values = tcoords->GetTuple(i);
    vesVertexDataT2f textureCoordinate;
    textureCoordinate.m_textureCoordinate = vesVector2f(values[0], values[1]);
    texCoordSourceData->pushBack(textureCoordinate);
  }

  return texCoordSourceData;
}

//----------------------------------------------------------------------------
void vesKiwiDataConversionTools::SetVertexColors(
  vtkUnsignedCharArray* colors, vesSharedPtr<vesGeometryData> geometryData)
{
  if (geometryData) {
    vesSourceData::Ptr colorSourceData = ConvertColors(colors);
    if (colorSourceData) {
      geometryData->addSource(colorSourceData);
    }
  }
}

//----------------------------------------------------------------------------
void vesKiwiDataConversionTools::SetVertexColors(vtkDataArray* scalars,
  vtkScalarsToColors* scalarsToColors, vesSharedPtr<vesGeometryData> geometryData)
{
  if (geometryData) {
    vesSourceData::Ptr colorSourceData = ConvertScalarsToColors(scalars, scalarsToColors);
    if (colorSourceData) {
      geometryData->addSource(colorSourceData);
    }
  }
}

//----------------------------------------------------------------------------
void vesKiwiDataConversionTools::SetTextureCoordinates(vtkDataArray* tcoords,
  vesSharedPtr<vesGeometryData> geometryData)
{
  if (geometryData) {
    vesSourceDataT2f::Ptr tcoordSourceData = ConvertTCoords(tcoords);
    if (tcoordSourceData) {
      geometryData->addSource(tcoordSourceData);
    }
  }
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkUnsignedCharArray> vesKiwiDataConversionTools::MapScalars(
vtkDataArray* scalars, vtkScalarsToColors* scalarsToColors)
{
/*
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
*/

  scalarsToColors->SetVectorModeToMagnitude();

  vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::Take(
    scalarsToColors->MapScalars(scalars, VTK_COLOR_MODE_MAP_SCALARS, -1));

  return colors;
}

//----------------------------------------------------------------------------
vesImage::Ptr vesKiwiDataConversionTools::ConvertImage(vtkImageData* imageData)
{
  assert(imageData);
  vtkSmartPointer<vtkUnsignedCharArray> pixels = vtkUnsignedCharArray::SafeDownCast(imageData->GetPointData()->GetScalars());
  if (!pixels) {
    return vesImage::Ptr();
  }

  const int width = imageData->GetDimensions()[0];
  const int height = imageData->GetDimensions()[1];

  return vesKiwiDataConversionTools::ImageFromPixels(pixels, width, height);
}

//----------------------------------------------------------------------------
vesImage::Ptr vesKiwiDataConversionTools::ImageFromPixels(vtkUnsignedCharArray* pixels, int width, int height)
{
  assert(pixels);
  assert(pixels->GetNumberOfTuples() == width*height);

  vesImage::Ptr image (new vesImage());
  image->setWidth(width);
  image->setHeight(height);
  image->setPixelFormat(  pixels->GetNumberOfComponents() == 4 ? vesColorDataType::RGBA
                        : pixels->GetNumberOfComponents() == 3 ? vesColorDataType::RGB
                        : vesColorDataType::Luminance);
  image->setPixelDataType(vesColorDataType::UnsignedByte);
  image->setData(pixels->GetPointer(0), pixels->GetSize());
  return image;
}


//----------------------------------------------------------------------------
void vesKiwiDataConversionTools::SetTextureData(vtkUnsignedCharArray* pixels,
  vesSharedPtr<vesTexture> texture, int width, int height)
{
  assert(texture);
  vesImage::Ptr image = vesKiwiDataConversionTools::ImageFromPixels(pixels, width, height);
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
    vesStaticPtrCast< vesIndices<T> >
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
  const vtkIdType maximumNumberOfPoints = -1;

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

    for (int i = 0; i < verts->GetNumberOfCells(); ++i) {
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


  return output;
}

//-----------------------------------------------------------------------------
vesSharedPtr<vesGeometryData> vesKiwiDataConversionTools::ConvertPVWebData(vesSharedPtr<vesPVWebDataSet> dataset)
{
  vesSharedPtr<vesGeometryData> geometryData = vesSharedPtr<vesGeometryData>(new vesGeometryData);
  geometryData->setName("PolyData");

#ifdef VES_USE_CURL
  const int numberOfVerts = dataset->m_numberOfVerts;


  if (dataset->m_datasetType == 'M') {

    // verts and normals
    vesSourceDataP3N3f::Ptr sourceData(new vesSourceDataP3N3f());
    vesVertexDataP3N3f vertexData;
    for (int i = 0; i < numberOfVerts; ++i) {
      float* vertex = dataset->vertices() + i*3;
      float* normal = dataset->normals() + i*3;
      vertexData.m_position[0] = vertex[0];
      vertexData.m_position[1] = vertex[1];
      vertexData.m_position[2] = vertex[2];
      vertexData.m_normal[0] = normal[0];
      vertexData.m_normal[1] = normal[1];
      vertexData.m_normal[2] = normal[2];
      sourceData->pushBack(vertexData);
    }
    geometryData->addSource(sourceData);

    // triangles
    vesSharedPtr<vesIndices<unsigned int> > triangleIndices =
      vesSharedPtr<vesIndices<unsigned int> >(new vesIndices<unsigned int>());
    vesPrimitive::Ptr trianglesPrimitive = vesPrimitive::Ptr(new vesPrimitive());
    trianglesPrimitive->setIndexCount(3);
    trianglesPrimitive->setIndicesValueType(vesPrimitiveIndicesValueType::UnsignedInt);
    trianglesPrimitive->setPrimitiveType(vesPrimitiveRenderType::Triangles);
    trianglesPrimitive->setVesIndices(triangleIndices);
    geometryData->addPrimitive(trianglesPrimitive);

    for (int i = 0; i < dataset->m_numberOfIndices/3; ++i) {
      short* indices = dataset->indices() + i*3;
      triangleIndices->pushBackIndices(indices[0], indices[1], indices[2]);
    }

  }
  else if (dataset->m_datasetType == 'L') {

    // verts
    vesSourceDataP3f::Ptr vertexSourceData(new vesSourceDataP3f());
    vesVertexDataP3f vertexData;
    for (int i = 0; i < numberOfVerts; ++i) {
      float* vertex = dataset->vertices() + i*3;
      vertexData.m_position[0] = vertex[0];
      vertexData.m_position[1] = vertex[1];
      vertexData.m_position[2] = vertex[2];
      vertexSourceData->pushBack(vertexData);
    }
    geometryData->addSource(vertexSourceData);

    // lines
    vesSharedPtr<vesIndices<unsigned int> > lineIndices =
      vesSharedPtr<vesIndices<unsigned int> >(new vesIndices<unsigned int>());
    vesPrimitive::Ptr linesPrimitive = vesPrimitive::Ptr(new vesPrimitive());
    linesPrimitive->setIndexCount(2);
    linesPrimitive->setIndicesValueType(vesPrimitiveIndicesValueType::UnsignedInt);
    linesPrimitive->setPrimitiveType(vesPrimitiveRenderType::Lines);
    linesPrimitive->setVesIndices(lineIndices);
    geometryData->addPrimitive(linesPrimitive);

    for (int i = 0; i < dataset->m_numberOfIndices/2; ++i) {
      short* indices = dataset->indices() + i*2;
      lineIndices->pushBackIndices(indices[0], indices[1]);
    }
  }
  else if (dataset->m_datasetType == 'P') {

    // verts
    vesSourceDataP3f::Ptr vertexSourceData(new vesSourceDataP3f());
    vesVertexDataP3f vertexData;
    for (int i = 0; i < numberOfVerts; ++i) {
      float* vertex = dataset->vertices() + i*3;
      vertexData.m_position[0] = vertex[0];
      vertexData.m_position[1] = vertex[1];
      vertexData.m_position[2] = vertex[2];
      vertexSourceData->pushBack(vertexData);
    }
    geometryData->addSource(vertexSourceData);

    vesPrimitive::Ptr pointPrimitive (new vesPrimitive());
    pointPrimitive->setPrimitiveType(vesPrimitiveRenderType::Points);
    pointPrimitive->setIndexCount(1);
    geometryData->addPrimitive(pointPrimitive);
  }

  // colors
  float opacity = 1.0;
  if (dataset->m_transparency) {
    opacity = 0.4;
  }

  vesSourceDataC3f::Ptr colorSourceData(new vesSourceDataC3f());
  vesVertexDataC3f vertColor;
  for (size_t i = 0; i < numberOfVerts; ++i)
    {
    unsigned char* color = dataset->colors() + i*4;
    vertColor.m_color = vesVector3f(color[0]/255.0, color[1]/255.0, color[2]/255.0);
    colorSourceData->pushBack(vertColor);
    }

  geometryData->addSource(colorSourceData);
#endif // VES_USE_CURL
  return geometryData;
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> vesKiwiDataConversionTools::TriangulatePolyData(vtkPolyData* input, bool addNormals, bool duplicateVertices)
{
  vtkSmartPointer<vtkPolyData> polyData = input;

  // triangulate
  {
    vtkNew<vtkTriangleFilter> triangleFilter;
    triangleFilter->PassLinesOn();
    triangleFilter->PassVertsOn();
    triangleFilter->SetInputData(polyData);
    triangleFilter->Update();
    polyData = triangleFilter->GetOutput();
  }

  // add normals
  if (addNormals && !polyData->GetPointData()->GetNormals()) {
    const int normalsFeatureAngle = 25.0;
    vtkNew<vtkPolyDataNormals> normals;
    normals->SetInputData(polyData);
    normals->SplittingOn();
    normals->SetFeatureAngle(normalsFeatureAngle);
    normals->Update();
    polyData = normals->GetOutput();
  }

  // duplicate verts so that triangles do not share vertices
  if (duplicateVertices) {
    vtkNew<vtkShrinkPolyData> shrink;
    shrink->SetInputData(polyData);
    shrink->SetShrinkFactor(1.0);
    shrink->Update();
    polyData = shrink->GetOutput();
  }

  return polyData;
}

//-----------------------------------------------------------------------------
void vesKiwiDataConversionTools::RemoveSharedTriangleVertices(vesGeometryData::Ptr geometryData, const std::vector<vesSourceData::Ptr>& sourceData)
{
  if (!geometryData->triangles()) {
    return;
  }

  // get verts
  vesSourceDataP3N3f::Ptr verts = vesStaticPtrCast<vesSourceDataP3N3f>(geometryData->sourceData(vesVertexAttributeKeys::Position));
  size_t numberOfVerts = verts->arrayReference().size();


  // get triangles
  std::vector<unsigned int>& triangleIndices = *(vesStaticPtrCast<vesIndices<unsigned int> >(geometryData->triangles()->getVesIndices())->indices());
  size_t numberOfIndices = triangleIndices.size();


  // in this case, we'll assume there are no shared vertices
  if (numberOfVerts == numberOfIndices) {
    return;
  }

  //printf("number of verts: %ul\n", numberOfVerts);
  //printf("number of tris: %ul\n", numberOfIndices/3);
  //printf("number of tri indices: %ul\n", numberOfIndices);

  std::vector<bool> visited(numberOfVerts, false);
  std::vector<unsigned int> indicesToDuplicate;

  size_t counter = 0;
  // loop over triangles
  for (size_t i = 0; i < numberOfIndices; ++i) {

    unsigned int vertIndex = triangleIndices[i];

    if (visited[vertIndex]) {
      ++counter;
      triangleIndices[i] = numberOfVerts++;
      indicesToDuplicate.push_back(vertIndex);
    }
    else {
      visited[vertIndex] = true;
    }
  }

  // duplicate sources
  for (size_t i = 0; i < sourceData.size(); ++i) {
    sourceData[i]->duplicateElements(indicesToDuplicate);
  }

  for (unsigned int i = 0; i < geometryData->numberOfSources(); ++i) {
    vesSourceData::Ptr source = geometryData->source(i);
    if (std::find(sourceData.begin(), sourceData.end(), source) == sourceData.end()) {
      source->duplicateElements(indicesToDuplicate);
    }
  }

  //printf("duplicated verts: %lu\n", counter);
  //printf("now number of verts: %ul\n", verts->arrayReference().size());
}

//-----------------------------------------------------------------------------
void vesKiwiDataConversionTools::ComputeWireframeVertexArrays(vesGeometryData::Ptr geometryData)
{
  if (!geometryData->triangles()) {
    return;
  }

  // get verts
  vesSourceDataP3N3f::Ptr verts = vesStaticPtrCast<vesSourceDataP3N3f>(geometryData->sourceData(vesVertexAttributeKeys::Position));
  size_t numberOfVerts = verts->arrayReference().size();


  // get triangles
  std::vector<unsigned int>& triangleIndices = *(vesStaticPtrCast<vesIndices<unsigned int> >(geometryData->triangles()->getVesIndices())->indices());
  size_t numberOfTriangles = triangleIndices.size()/3;


  vesGenericSourceData3f::Ptr p1Source(new vesGenericSourceData3f(10));
  vesGenericSourceData3f::Ptr p2Source(new vesGenericSourceData3f(11));
  vesGenericSourceData1f::Ptr indsSource(new vesGenericSourceData1f(12));

  geometryData->addSource(p1Source);
  geometryData->addSource(p2Source);
  geometryData->addSource(indsSource);

  p1Source->arrayReference().resize(numberOfVerts);
  p2Source->arrayReference().resize(numberOfVerts);
  indsSource->arrayReference().resize(numberOfVerts);


  vesVertexDataP3f tempP1;
  vesVertexDataP3f tempP2;

  // loop over triangles
  for (size_t i = 0; i < numberOfTriangles; ++i) {

    unsigned int p0 = triangleIndices[i*3 + 0];
    unsigned int p1 = triangleIndices[i*3 + 1];
    unsigned int p2 = triangleIndices[i*3 + 2];


    // p0 --> p1, p2
    tempP1.m_position = verts->arrayReference()[p1].m_position;
    tempP2.m_position = verts->arrayReference()[p2].m_position;

    indsSource->arrayReference()[p0] = 0.0;
    p1Source->arrayReference()[p0] = tempP1.m_position;
    p2Source->arrayReference()[p0] = tempP2.m_position;

    // p1 --> p2, p0
    tempP1.m_position = verts->arrayReference()[p2].m_position;
    tempP2.m_position = verts->arrayReference()[p0].m_position;

    indsSource->arrayReference()[p1] = 1.0;
    p1Source->arrayReference()[p1] = tempP1.m_position;
    p2Source->arrayReference()[p1] = tempP2.m_position;

    // p2 --> p0, p1
    tempP1.m_position = verts->arrayReference()[p0].m_position;
    tempP2.m_position = verts->arrayReference()[p1].m_position;

    indsSource->arrayReference()[p2] = 2.0;
    p1Source->arrayReference()[p2] = tempP1.m_position;
    p2Source->arrayReference()[p2] = tempP2.m_position;
  }

}
