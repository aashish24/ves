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

#include "vesKiwiPolyDataRepresentation.h"
#include "vesKiwiColorMapCollection.h"
#include "vesKiwiDataConversionTools.h"
#include "vesActor.h"
#include "vesBlend.h"
#include "vesDepth.h"
#include "vesGeometryData.h"
#include "vesMapper.h"
#include "vesMaterial.h"
#include "vesRenderer.h"
#include "vesShaderProgram.h"
#include "vesTexture.h"

#include "vesGL.h"

#include "vesVertexAttribute.h"
#include "vesSourceData.h"
#include "vesPrimitive.h"
#include "vesUniform.h"

#include <vtkCellData.h>
#include <vtkCellDataToPointData.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkNew.h>
#include <vtkShrinkFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkLookupTable.h>
#include <vtkDiscretizableColorTransferFunction.h>

#include <cassert>

//----------------------------------------------------------------------------
class vesKiwiPolyDataRepresentation::vesInternal
{
public:

  vesInternal()
  {
    this->GeometryMode = SURFACE_MODE;
  }

  ~vesInternal()
  {
  }

  int GeometryMode;

  vesSharedPtr<vesActor>     Actor;
  vesSharedPtr<vesMapper>    Mapper;
  vesSharedPtr<vesMaterial>  Material;
  vesSharedPtr<vesTexture>   Texture;
  vesSharedPtr<vesBlend>     Blend;
  vesSharedPtr<vesDepth>     Depth;

  vesShaderProgram::Ptr WireframeShader;
  vesShaderProgram::Ptr SurfaceWithEdgesShader;
  vesShaderProgram::Ptr SurfaceShader;
  vesShaderProgram::Ptr TextureSurfaceShader;

  vesPrimitive::Ptr Triangles;
  vesPrimitive::Ptr Lines;
  vesPrimitive::Ptr Points;

  vesSourceData::Ptr Colors;
  vesSourceData::Ptr ScalarColors;
  vesSourceDataT2f::Ptr TCoords;

  std::vector<std::string> ScalarArrayNames;
  std::vector<vesSourceData::Ptr> ScalarArrays;

  vesSourceData::Ptr WireframeSources[3];
};

//----------------------------------------------------------------------------
vesKiwiPolyDataRepresentation::vesKiwiPolyDataRepresentation()
{
  this->Internal = new vesInternal();
}

//----------------------------------------------------------------------------
vesKiwiPolyDataRepresentation::~vesKiwiPolyDataRepresentation()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setPolyData(vtkPolyData* input,
                                                bool convertCellToPointData)
{
  assert(input);
  assert(this->Internal->Mapper);

  vesGeometryData::Ptr geometryData;
  vtkSmartPointer<vtkPolyData> polyData;

  // Convert cell data to point data first
  if (convertCellToPointData && input->GetCellData()->GetNumberOfArrays()) {
    vtkNew<vtkShrinkFilter> shrinkFilter;
    shrinkFilter->SetShrinkFactor(1.0);
    shrinkFilter->SetInputData(input);

    vtkNew<vtkDataSetSurfaceFilter> surfaceFilter;
    surfaceFilter->SetInputConnection(shrinkFilter->GetOutputPort());

    vtkNew<vtkCellDataToPointData> cellToPointData;
    cellToPointData->SetInputConnection(surfaceFilter->GetOutputPort());
    cellToPointData->Update();
    polyData = vtkPolyData::SafeDownCast(cellToPointData->GetOutput());
    }
  else {
    polyData = input;
    }

  assert(polyData);

  if (!polyData->GetNumberOfStrips() && !polyData->GetNumberOfPolys() && !polyData->GetNumberOfLines()) {
    geometryData = vesKiwiDataConversionTools::ConvertPoints(polyData);
  }
  else {
    bool addNormals = true;
    bool duplicateVerts = false;
    polyData = vesKiwiDataConversionTools::TriangulatePolyData(polyData, addNormals, duplicateVerts);
    geometryData =  vesKiwiDataConversionTools::Convert(polyData);
  }

  this->Internal->Mapper->setGeometryData(geometryData);
  this->convertVertexArrays(polyData);
  this->colorByDefault();
}

//----------------------------------------------------------------------------
vesSharedPtr<vesGeometryData> vesKiwiPolyDataRepresentation::geometryData() const
{
  if (this->Internal->Mapper) {
    return this->Internal->Mapper->geometryData();
  }
  return vesSharedPtr<vesGeometryData>();
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::surfaceOn()
{

  // restore primitives
  if (this->Internal->Triangles) {
    this->geometryData()->addPrimitive(this->Internal->Triangles);
  }
  if (this->Internal->Lines) {
    this->geometryData()->addPrimitive(this->Internal->Lines);
  }
  if (this->Internal->Points) {
    this->geometryData()->removePrimitive(this->Internal->Points);
  }

  this->Internal->Triangles.reset();
  this->Internal->Lines.reset();


  // disable wireframe mode
  for (int i = 0; i < 3; ++i) {
    this->geometryData()->removeSource(this->Internal->WireframeSources[i]);
  }

  // reset VBO
  vesGeometryData::Ptr geometryData = this->geometryData();
  this->mapper()->setGeometryData(vesGeometryData::Ptr(new vesGeometryData));
  this->mapper()->setGeometryData(geometryData);

  this->setShaderProgram(this->Internal->SurfaceShader);

  this->Internal->GeometryMode = SURFACE_MODE;
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::surfaceWithEdgesOn()
{
  this->wireframeOn();
  this->setShaderProgram(this->Internal->SurfaceWithEdgesShader);
  this->Internal->GeometryMode = SURFACE_WITH_EDGES_MODE;
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::wireframeOn()
{

  // restore primitives
  if (this->Internal->Triangles) {
    this->geometryData()->addPrimitive(this->Internal->Triangles);
  }
  if (this->Internal->Lines) {
    this->geometryData()->addPrimitive(this->Internal->Lines);
  }
  if (this->Internal->Points) {
    this->geometryData()->removePrimitive(this->Internal->Points);
  }

  this->Internal->Triangles.reset();
  this->Internal->Lines.reset();

  // enable wireframe mode
  if (!this->Internal->WireframeSources[0]) {

    std::vector<vesSourceData::Ptr> sourceData = this->Internal->ScalarArrays;
    if (this->Internal->Colors) {
      sourceData.push_back(this->Internal->Colors);
    }
    if (this->Internal->TCoords) {
      sourceData.push_back(this->Internal->TCoords);
    }

    vesKiwiDataConversionTools::RemoveSharedTriangleVertices(this->geometryData(), sourceData);
    vesKiwiDataConversionTools::ComputeWireframeVertexArrays(this->geometryData());
    this->Internal->WireframeSources[0] = this->geometryData()->sourceData(10);
    this->Internal->WireframeSources[1] = this->geometryData()->sourceData(11);
    this->Internal->WireframeSources[2] = this->geometryData()->sourceData(12);
  }
  else {
    for (int i = 0; i < 3; ++i) {
      this->geometryData()->addSource(this->Internal->WireframeSources[i]);
    }
  }

  // reset VBO
  vesGeometryData::Ptr geometryData = this->geometryData();
  this->mapper()->setGeometryData(vesGeometryData::Ptr(new vesGeometryData));
  this->mapper()->setGeometryData(geometryData);


  this->setShaderProgram(this->Internal->WireframeShader);

  this->Internal->GeometryMode = WIREFRAME_MODE;
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::pointsOn()
{
  this->setShaderProgram(this->Internal->SurfaceShader);

  for (int i = 0; i < 3; ++i) {
    this->geometryData()->removeSource(this->Internal->WireframeSources[i]);
  }

  if (this->geometryData()->triangles()) {
    this->Internal->Triangles = this->geometryData()->triangles();
  }

  if (this->geometryData()->lines()) {
    this->Internal->Lines = this->geometryData()->lines();
  }

  this->geometryData()->removePrimitive(this->Internal->Triangles);
  this->geometryData()->removePrimitive(this->Internal->Lines);
  this->geometryData()->removePrimitive(this->Internal->Points);

  // disable wireframe mode
  for (int i = 0; i < 3; ++i) {
    this->geometryData()->removeSource(this->Internal->WireframeSources[i]);
  }

  // reset VBO
  vesGeometryData::Ptr geometryData = this->geometryData();
  this->mapper()->setGeometryData(vesGeometryData::Ptr(new vesGeometryData));
  this->mapper()->setGeometryData(geometryData);

  vesPrimitive::Ptr pointPrimitive (new vesPrimitive());
  pointPrimitive->setPrimitiveType(vesPrimitiveRenderType::Points);
  pointPrimitive->setIndexCount(1);
  this->geometryData()->addPrimitive(pointPrimitive);
  this->Internal->Points = pointPrimitive;

  this->Internal->GeometryMode = POINTS_MODE;
}

//----------------------------------------------------------------------------
int vesKiwiPolyDataRepresentation::geometryMode() const
{
  return this->Internal->GeometryMode;
}

//----------------------------------------------------------------------------
const std::string& vesKiwiPolyDataRepresentation::name() const
{
  return this->geometryData()->name();
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setName(const std::string& name)
{
  this->geometryData()->setName(name);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::addTextureCoordinates(vtkDataArray* textureCoordinates)
{
  assert(this->Internal->Mapper);
  assert(this->Internal->Mapper->geometryData());

  vesGeometryData::Ptr geometryData = this->Internal->Mapper->geometryData();
  vesKiwiDataConversionTools::SetTextureCoordinates(textureCoordinates, geometryData);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setTranslation(const vesVector3f& translation)
{
  assert(this->Internal->Actor);
  this->Internal->Actor->setTranslation(translation);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setShaderProgram(
  vesSharedPtr<vesShaderProgram> shaderProgram)
{
  if (!shaderProgram) {
    return;
  }

  this->Internal->Actor->material()->setShaderProgram(shaderProgram);
}

//----------------------------------------------------------------------------
vesSharedPtr<vesShaderProgram> vesKiwiPolyDataRepresentation::shaderProgram() const
{
  return this->Internal->Actor->material()->shaderProgram();
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setSurfaceShader(vesSharedPtr<vesShaderProgram> shader)
{
  this->Internal->SurfaceShader = shader;
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setWireframeShader(vesSharedPtr<vesShaderProgram> shader)
{
  this->Internal->WireframeShader = shader;
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setSurfaceWithEdgesShader(vesSharedPtr<vesShaderProgram> shader)
{
  this->Internal->SurfaceWithEdgesShader = shader;
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setTextureSurfaceShader(vesSharedPtr<vesShaderProgram> shader)
{
  this->Internal->TextureSurfaceShader = shader;
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::initializeWithShader(
  vesSharedPtr<vesShaderProgram> shaderProgram)
{
  assert(shaderProgram);
  assert(!this->Internal->Mapper && !this->Internal->Actor);

  this->Internal->SurfaceShader = shaderProgram;
  this->Internal->Mapper = vesSharedPtr<vesMapper>(new vesMapper());

  this->Internal->Actor = vesSharedPtr<vesActor>(new vesActor());
  this->Internal->Actor->setMapper(this->Internal->Mapper);

  this->Internal->Material = vesSharedPtr<vesMaterial>(new vesMaterial());
  this->Internal->Actor->setMaterial(this->Internal->Material);

  this->Internal->Blend = vesSharedPtr<vesBlend>(new vesBlend());
  this->Internal->Depth = vesSharedPtr<vesDepth>(new vesDepth());

  this->Internal->Actor->material()->addAttribute(shaderProgram);
  this->Internal->Actor->material()->addAttribute(this->Internal->Blend);
  this->Internal->Actor->material()->addAttribute(this->Internal->Depth);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setBinNumber(int binNumber)
{
  assert(this->Internal->Actor);
  this->Internal->Actor->material()->setBinNumber(binNumber);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setTexture(vesSharedPtr<vesTexture> texture)
{
  assert(this->Internal->Actor);
  this->Internal->Texture = texture;
  this->Internal->Actor->material()->addAttribute(texture);
}

//----------------------------------------------------------------------------
vesSharedPtr<vesTexture> vesKiwiPolyDataRepresentation::texture() const
{
  return this->Internal->Texture;
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setColor(double r, double g, double b, double a)
{
  assert(this->Internal->Actor && this->Internal->Actor->mapper());
  this->Internal->Actor->mapper()->setColor(r, g, b, a);
}

//----------------------------------------------------------------------------
const vesVector4f& vesKiwiPolyDataRepresentation::color() const
{
  return this->Internal->Actor->mapper()->colorVector();
}

//----------------------------------------------------------------------------
vesVector4f& vesKiwiPolyDataRepresentation::color()
{
  return this->Internal->Actor->mapper()->colorVector();
}

//----------------------------------------------------------------------------
const double& vesKiwiPolyDataRepresentation::opacity() const
{
  return this->Internal->Actor->mapper()->color()[3];
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setOpacity(const double& opacity)
{
  float* color = this->Internal->Actor->mapper()->color();
  this->Internal->Actor->mapper()->setColor(color[0], color[1], color[2], opacity);

  if (opacity != 1.0) {
    this->setBinNumber(2);
  }
  else {
    this->setBinNumber(1);
  }
}

//----------------------------------------------------------------------------
int vesKiwiPolyDataRepresentation::pointSize() const
{
  return this->Internal->Actor->mapper()->pointSize();
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setPointSize(int size)
{
  this->Internal->Actor->mapper()->setPointSize(size);
}

//----------------------------------------------------------------------------
int vesKiwiPolyDataRepresentation::lineWidth() const
{
  return this->Internal->Actor->mapper()->lineWidth();
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setLineWidth(int width)
{
  this->Internal->Actor->mapper()->setLineWidth(width);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::addSelfToRenderer(
  vesSharedPtr<vesRenderer> renderer)
{
  assert(renderer);
  renderer->addActor(this->Internal->Actor);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::removeSelfFromRenderer(
  vesSharedPtr<vesRenderer> renderer)
{
  assert(renderer);
  renderer->removeActor(this->Internal->Actor);
}

//----------------------------------------------------------------------------
vesSharedPtr<vesActor> vesKiwiPolyDataRepresentation::actor() const
{
  return this->Internal->Actor;
}

//----------------------------------------------------------------------------
vesSharedPtr<vesMapper> vesKiwiPolyDataRepresentation::mapper() const
{
  return this->Internal->Mapper;
}

//----------------------------------------------------------------------------
std::vector<std::string> vesKiwiPolyDataRepresentation::colorModes()
{
  std::vector<std::string> names;

  names.push_back("Solid Color");

  if (this->Internal->Colors) {
    names.push_back("Vertex RGB");
  }

  if (this->Internal->TCoords && this->Internal->Texture) {
    names.push_back("Texture");
  }

  names.insert(names.end(), this->Internal->ScalarArrayNames.begin(), this->Internal->ScalarArrayNames.end());
  return names;
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::setColorMode(const std::string& colorMode)
{
  if (colorMode == "Solid Color") {
    this->colorBySolidColor();
  }
  if (colorMode == "Vertex RGB") {
    this->colorByRGBArray();
  }
  else if (colorMode == "Texture") {
    this->colorByTexture();
  }
  else {
    this->colorByScalars(colorMode);
  }
}

//----------------------------------------------------------------------------
std::string vesKiwiPolyDataRepresentation::colorMode() const
{
  vesSourceData::Ptr colors = this->geometryData()->sourceData(vesVertexAttributeKeys::Color);
  vesSourceData::Ptr tcoords = this->geometryData()->sourceData(vesVertexAttributeKeys::TextureCoordinate);

  if (!colors && !tcoords) {
    return "Solid Color";
  }
  else if (tcoords && tcoords == this->Internal->TCoords) {
    return "Texture";
  }
  else if (colors && colors == this->Internal->Colors) {
    return "Vertex RGB";
  }
  else if (colors) {

    std::vector<vesSourceData::Ptr>::const_iterator itr = std::find(this->Internal->ScalarArrays.begin(),
                                                           this->Internal->ScalarArrays.end(), colors);
    if (itr != this->Internal->ScalarArrays.end()) {
      return this->Internal->ScalarArrayNames[itr - this->Internal->ScalarArrays.begin()];
    }
  }

  return "";
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::colorByDefault()
{
  this->colorBySolidColor();

  if (this->Internal->Colors) {
    this->colorByRGBArray();
  }
  else if (this->Internal->TCoords) {
    this->colorByTexture();
  }
  else if (this->Internal->ScalarColors) {
    this->colorByScalars();
  }
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::colorByTexture()
{
  this->colorBySolidColor();
  if (this->Internal->TCoords && this->Internal->TextureSurfaceShader) {
    this->geometryData()->addSource(this->Internal->TCoords);
    this->setShaderProgram(this->Internal->TextureSurfaceShader);
  }
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::colorByRGBArray()
{
  this->colorBySolidColor();
  if (this->Internal->Colors) {
    this->geometryData()->addSource(this->Internal->Colors);
  }
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::colorBySolidColor()
{
  this->geometryData()->removeSource(this->geometryData()->sourceData(vesVertexAttributeKeys::Color));
  this->geometryData()->removeSource(this->geometryData()->sourceData(vesVertexAttributeKeys::TextureCoordinate));


  // set correct shader
  if (this->Internal->GeometryMode == SURFACE_MODE
      || this->Internal->GeometryMode == POINTS_MODE) {
    this->setShaderProgram(this->Internal->SurfaceShader);
  }
  else if (this->Internal->GeometryMode == WIREFRAME_MODE) {
    this->setShaderProgram(this->Internal->WireframeShader);
  }
  else if (this->Internal->GeometryMode == SURFACE_WITH_EDGES_MODE) {
    this->setShaderProgram(this->Internal->SurfaceWithEdgesShader);
  }


  // reset VBO
  vesGeometryData::Ptr geometryData = this->geometryData();
  this->mapper()->setGeometryData(vesGeometryData::Ptr(new vesGeometryData));
  this->mapper()->setGeometryData(geometryData);
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::colorByScalars()
{
  this->colorBySolidColor();
  if (this->Internal->ScalarColors) {
    this->geometryData()->addSource(this->Internal->ScalarColors);
  }
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::colorByScalars(const std::string& arrayName)
{
  this->colorBySolidColor();

  std::vector<std::string>::const_iterator itr = std::find(this->Internal->ScalarArrayNames.begin(),
                                                           this->Internal->ScalarArrayNames.end(), arrayName);

  if (itr != this->Internal->ScalarArrayNames.end()) {
    vesSourceData::Ptr scalarColors = this->Internal->ScalarArrays[itr - this->Internal->ScalarArrayNames.begin()];
    this->geometryData()->addSource(scalarColors);
  }
}

/*
//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::colorByScalars(vtkDataArray* scalars, vtkScalarsToColors* scalarsToColors)
{

  std::string arrayName = scalars->GetName();
  this->Internal->ScalarColors = vesKiwiDataConversionTools::ConvertScalarsToColors(scalars, scalarsToColors);

  if (scalars->GetName()) {
    std::string arrayName = scalars->GetName();
    std::vector<std::string>::const_iterator itr = std::find(this->Internal->ScalarArrayNames.begin(),
                                                             this->Internal->ScalarArrayNames.end(), arrayName);

    if (itr != this->Internal->ScalarArrayNames.end()) {
      this->Internal->ScalarArrays[itr - this->Internal->ScalarArrayNames.begin()] = this->Internal->ScalarColors;
    }
  }
  this->colorByScalars();
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::colorByTexture(vtkDataArray* tcoords)
{
  this->Internal->TCoords = vesKiwiDataConversionTools::ConvertTCoords(tcoords);
  this->colorByTexture();
}
*/

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::assignColorsInternal()
{
  vesSourceData::Ptr colors = this->geometryData()->sourceData(vesVertexAttributeKeys::Color);
  if (colors) {
    this->Internal->Colors = colors;
  }
}

//----------------------------------------------------------------------------
void vesKiwiPolyDataRepresentation::convertVertexArrays(vtkPolyData* dataSet)
{
  this->Internal->Colors.reset();
  this->Internal->ScalarColors.reset();
  this->Internal->TCoords.reset();
  this->Internal->ScalarArrayNames.clear();
  this->Internal->ScalarArrays.clear();


  vtkUnsignedCharArray* colors = vesKiwiDataConversionTools::FindRGBColorsArray(dataSet);
  if (colors) {
    this->Internal->Colors = vesKiwiDataConversionTools::ConvertColors(colors);
  }

  std::vector<vtkDataArray*> scalarArrays = vesKiwiDataConversionTools::FindScalarArrays(dataSet);
  for (size_t i = 0; i < scalarArrays.size(); ++i) {

    vtkDataArray* scalars = scalarArrays[i];

    vtkSmartPointer<vtkScalarsToColors> colorMap = this->colorMapCollection()->colorMapForArray(scalars);
    if (!colorMap) {
      colorMap = vesKiwiDataConversionTools::GetBlueToRedLookupTable(scalars->GetRange());
    }

    vesSourceData::Ptr arrayData = vesKiwiDataConversionTools::ConvertScalarsToColors(scalars, colorMap);

    if (arrayData) {
      std::string arrayName = scalars->GetName() ? scalars->GetName() : "scalars";
      this->Internal->ScalarArrayNames.push_back(arrayName);
      this->Internal->ScalarArrays.push_back(arrayData);
    }
  }
  if (this->Internal->ScalarArrays.size()) {
    this->Internal->ScalarColors = this->Internal->ScalarArrays.front();
  }

  vtkDataArray* tcoords = vesKiwiDataConversionTools::FindTextureCoordinatesArray(dataSet);
  if (tcoords) {
    this->Internal->TCoords = vesKiwiDataConversionTools::ConvertTCoords(tcoords);
  }
}
