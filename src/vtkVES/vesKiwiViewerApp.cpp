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

#include "vesKiwiViewerApp.h"
#include "vesKiwiDataLoader.h"
#include "vesKiwiDataRepresentation.h"
#include "vesKiwiPolyDataRepresentation.h"
#include "vesDataConversionTools.h"

#include "vesCamera.h"
#include "vesColorUniform.h"
#include "vesGMTL.h"
#include "vesModelViewUniform.h"
#include "vesNormalMatrixUniform.h"
#include "vesProjectionUniform.h"
#include "vesRenderer.h"
#include "vesActor.h"
#include "vesShader.h"
#include "vesShaderProgram.h"
#include "vesTexture.h"
#include "vesTriangleData.h"
#include "vesUniform.h"
#include "vesVertexAttribute.h"
#include "vesVertexAttributeKeys.h"


#include <vtkDataSet.h>

// todo- move these includes to image representation class
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkCutter.h>
#include <vtkPlane.h>
#include <vtkOutlineFilter.h>
#include <vtkQuad.h>
#include <vtkPoints.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkExtractVOI.h>
#include <vtkLookupTable.h>
#include <vtkUnsignedCharArray.h>
#include <vtkContourFilter.h>
#include <vtkLineSource.h>
#include <vtkCellLocator.h>
#include <vtkAppendPolyData.h>

// text tests
#include <vtkTextProperty.h>
#include <vtkFreeTypeStringToImage.h>
#include <vtkPNGWriter.h>

#include <cassert>
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>

// todo- move these functions to image representation class
namespace {

int GetImageFlatDimension(vtkImageData* image)
{
  int dimensions[3];
  image->GetDimensions(dimensions);
  for (int i = 0; i < 3; ++i) {
    if (dimensions[i] == 1) {
      return i;
    }
  }
  return -1;
}

vtkSmartPointer<vtkPolyData> GetPolyDataForImagePlane(vtkImageData* image)
{
  double bounds[6];
  image->GetBounds(bounds);

  vtkNew<vtkPoints> quadPoints;
  quadPoints->SetNumberOfPoints(4);

  const int flatDimension = GetImageFlatDimension(image);
  if (flatDimension == 2) {
    // XY plane
    quadPoints->SetPoint(0, bounds[0],bounds[2],bounds[4]);
    quadPoints->SetPoint(1, bounds[1],bounds[2],bounds[4]);
    quadPoints->SetPoint(2, bounds[1],bounds[3],bounds[4]);
    quadPoints->SetPoint(3, bounds[0],bounds[3],bounds[4]);
  }
  else if (flatDimension == 1) {
    // XZ plane
    quadPoints->SetPoint(0, bounds[0],bounds[2],bounds[4]);
    quadPoints->SetPoint(1, bounds[1],bounds[2],bounds[4]);
    quadPoints->SetPoint(2, bounds[1],bounds[2],bounds[5]);
    quadPoints->SetPoint(3, bounds[0],bounds[2],bounds[5]);
  }
  else {
    // YZ plane
    quadPoints->SetPoint(0, bounds[0],bounds[2],bounds[4]);
    quadPoints->SetPoint(1, bounds[0],bounds[3],bounds[4]);
    quadPoints->SetPoint(2, bounds[0],bounds[3],bounds[5]);
    quadPoints->SetPoint(3, bounds[0],bounds[2],bounds[5]);
  }

  vtkNew<vtkQuad> quad;
  quad->GetPointIds()->SetId(0, 0);
  quad->GetPointIds()->SetId(1, 1);
  quad->GetPointIds()->SetId(2, 2);
  quad->GetPointIds()->SetId(3, 3);

  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
  polyData->Allocate(1, 1);
  polyData->InsertNextCell(quad->GetCellType(), quad->GetPointIds());
  polyData->SetPoints(quadPoints.GetPointer());

  // add texture coordinates
  vtkNew<vtkFloatArray> tcoords;
  tcoords->SetName("tcoords");
  tcoords->SetNumberOfComponents(2);
  tcoords->SetNumberOfTuples(4);
  tcoords->SetTuple2(0, 0,0);
  tcoords->SetTuple2(1, 1,0);
  tcoords->SetTuple2(2, 1,1);
  tcoords->SetTuple2(3, 0,1);
  polyData->GetPointData()->SetScalars(tcoords.GetPointer());

  return polyData;
}

};

//----------------------------------------------------------------------------
class vesKiwiViewerApp::vesInternal
{
public:

  vesInternal()
  {
    this->ShaderProgram = 0;
    this->TextureShader = 0;
    this->SelectedImageDimension = -1;
    this->ContourRep = 0;

    this->TextToImage = vtkSmartPointer<vtkFreeTypeStringToImage>::New();
    this->TextRepresentation = 0;
  }

  ~vesInternal()
  {

  }


  vesShaderProgram* ShaderProgram;
  vesShaderProgram* TextureShader;

  // todo- move these ivars to image representation class
  std::map<vesTexture*, vtkSmartPointer<vtkDataArray> > TextureStorage;
  vtkSmartPointer<vtkExtractVOI> SliceFilter;
  std::vector<vesKiwiPolyDataRepresentation*> SliceReps;
  vesKiwiPolyDataRepresentation* ContourRep;
  vtkSmartPointer<vtkCellLocator> Locator;
  vtkSmartPointer<vtkAppendPolyData> AppendFilter;
  int SelectedImageDimension;
  int CurrentSliceIndices[3];
  int ContourVis;
  double ImageScalarRange[2];

  std::vector<vesKiwiDataRepresentation*> DataRepresentations;

  vesKiwiDataLoader DataLoader;

  std::string VertexShaderSource;
  std::string FragmentShaderSource;

  std::vector<std::string> BuiltinDatasetNames;
  std::vector<std::string> BuiltinDatasetFilenames;

  std::string CurrentShadingModel;
  std::vector<std::string> BuiltinShadingModels;

  vtkSmartPointer<vtkFreeTypeStringToImage> TextToImage;
  vesKiwiPolyDataRepresentation* TextRepresentation;
};

//----------------------------------------------------------------------------
vesKiwiViewerApp::vesKiwiViewerApp()
{
  this->Internal = new vesInternal();
  this->setBackgroundColor(63/255.0, 96/255.0, 144/255.0);

  this->addBuiltinDataset("Sphere", "current.stl");
  this->addBuiltinDataset("Utah Teapot", "teapot.vtp");
  this->addBuiltinDataset("Stanford Bunny", "bunny.vtp");
  this->addBuiltinDataset("NLM Visible Woman Hand", "visible-woman-hand.vtp");
  this->addBuiltinDataset("NAMIC Knee Atlas", "AppendedKneeData.vtp");
  this->addBuiltinDataset("Mount St. Helens", "MountStHelen.vtp");
  this->addBuiltinDataset("Space Shuttle", "shuttle.vtp");
  this->addBuiltinDataset("Buckyball", "Buckyball.vtp");
  this->addBuiltinDataset("Motor", "motor.g");
  this->addBuiltinDataset("Caffeine", "caffeine.pdb");
  this->addBuiltinDataset("Head", "head.vti");
  this->addBuiltinDataset("KiwiViewer Logo", "kiwi.png");

  this->addBuiltinShadingModel("Gouraud");
  this->addBuiltinShadingModel("Blinn-Phong");
  this->addBuiltinShadingModel("Toon");
}

//----------------------------------------------------------------------------
vesKiwiViewerApp::~vesKiwiViewerApp()
{
  this->removeAllDataRepresentations();
  delete this->Internal;
}

//----------------------------------------------------------------------------
vesShaderProgram* vesKiwiViewerApp::shaderProgram() const
{
  return this->Internal->ShaderProgram;
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfBuiltinDatasets() const
{
  return static_cast<int>(this->Internal->BuiltinDatasetNames.size());
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::defaultBuiltinDatasetIndex() const
{
  return 1;
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::builtinDatasetName(int index)
{
  assert(index >= 0 && index < this->numberOfBuiltinDatasets());
  return this->Internal->BuiltinDatasetNames[index];
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::builtinDatasetFilename(int index)
{
  assert(index >= 0 && index < this->numberOfBuiltinDatasets());
  return this->Internal->BuiltinDatasetFilenames[index];
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::addBuiltinDataset(const std::string& name, const std::string& filename)
{
  this->Internal->BuiltinDatasetNames.push_back(name);
  this->Internal->BuiltinDatasetFilenames.push_back(filename);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::addBuiltinShadingModel(const std::string &name)
{
  this->Internal->BuiltinShadingModels.push_back(name);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::willRender()
{
  this->updateTextAnnotations();
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::scrollSliceModeActive() const
{
  return (this->Internal->SelectedImageDimension >= 0);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::scrollImageSlice(double deltaX, double deltaY)
{
  if (!this->Internal->SliceFilter) {
    return;
  }

  deltaY *= -1;

  vesRenderer* ren = this->renderer();
  vesCamera* camera = ren->camera();
  vesVector3f viewFocus = camera->GetFocalPoint();
  vesVector3f viewPoint = camera->GetPosition();
  vesVector3f viewFocusDisplay = ren->computeWorldToDisplay(viewFocus);
  float focalDepth = viewFocusDisplay[2];

  double x0 = viewFocusDisplay[0];
  double y0 = viewFocusDisplay[1];
  double x1 = x0 + deltaX;
  double y1 = y0 + deltaY;

  // map change into world coordinates
  vesVector3f point0 = ren->computeDisplayToWorld(vesVector3f(x0, y0, focalDepth));
  vesVector3f point1 = ren->computeDisplayToWorld(vesVector3f(x1, y1, focalDepth));
  vesVector3f motionVector = point1 - point0;



  int flatDimension = this->Internal->SelectedImageDimension;

  vesVector3f planeNormal(0, 0, 0);
  planeNormal[flatDimension] = 1.0;

  double vectorDot = gmtl::dot(motionVector, planeNormal);

  //printf("motion vector: %f %f %f\n", motionVector[0],motionVector[1],motionVector[2]);
  //printf("plane normal: %f %f %f\n", planeNormal[0],planeNormal[1],planeNormal[2]);
  //printf("dot: %f\n", vectorDot);

  double delta = vectorDot;
  if (fabs(delta) < 1e-6) {
    delta = deltaY;
  }

  int dimensions[3];
  vtkImageData::SafeDownCast(this->Internal->SliceFilter->GetInput())->GetDimensions(dimensions);

  int dimensionDelta = static_cast<int>(delta);
  if (dimensionDelta == 0) {
    dimensionDelta = delta > 0 ? 1 : -1;
  }

  // Get new VOI flat dimension
  int newDimension = this->Internal->CurrentSliceIndices[flatDimension] + dimensionDelta;
  if (newDimension < 0) {
    newDimension = 0;
  }
  else if (newDimension >= dimensions[flatDimension]) {
    newDimension = dimensions[flatDimension] - 1;
  }

  this->Internal->CurrentSliceIndices[flatDimension] = newDimension;

  if (flatDimension == 0) {
    this->Internal->SliceFilter->SetVOI(newDimension, newDimension, 0, dimensions[1], 0, dimensions[2]);
  }
  else if (flatDimension == 1) {
    this->Internal->SliceFilter->SetVOI(0, dimensions[0], newDimension, newDimension, 0, dimensions[2]);
  }
  else {
    this->Internal->SliceFilter->SetVOI(0, dimensions[0], 0, dimensions[1], newDimension, newDimension);
  }

  this->Internal->SliceFilter->Update();
  vtkImageData* sliceImage = this->Internal->SliceFilter->GetOutput();

  vtkSmartPointer<vtkPolyData> imagePlane = GetPolyDataForImagePlane(sliceImage);


  this->Internal->AppendFilter->GetInput(flatDimension)->DeepCopy(imagePlane);

  vesKiwiPolyDataRepresentation* rep = this->Internal->SliceReps[flatDimension];
  rep->setDataSet(imagePlane);
  this->Internal->TextureStorage.erase(rep->texture());
  this->setTextureFromImage(rep->texture(), sliceImage);
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleSingleTouchPanGesture(double deltaX, double deltaY)
{
  if (this->scrollSliceModeActive()) {
    this->scrollImageSlice(deltaX, deltaY);
    return;
  }

  this->Superclass::handleSingleTouchPanGesture(deltaX, deltaY);
}


//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleDoubleTap()
{
  if (this->Internal->ContourRep) {
    this->Internal->ContourVis = (this->Internal->ContourVis + 1) % 3;
    if (this->Internal->ContourVis == 0) {
      this->Internal->ContourRep->removeSelfFromRenderer(this->renderer());
    }
    else if (this->Internal->ContourVis == 1) {
      this->Internal->ContourRep->addSelfToRenderer(this->renderer());
      this->Internal->ContourRep->mapper()->setColor(0.8, 0.8, 0.8, 0.3);
    }
    else {
      this->Internal->ContourRep->mapper()->setColor(0.8, 0.8, 0.8, 1.0);
    }
  }
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleSingleTouchUp()
{
  this->Internal->SelectedImageDimension = -1;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::handleSingleTouchDown(int displayX, int displayY)
{
  if (!this->Internal->AppendFilter) {
    return;
  }

  // calculate the focal depth so we'll know how far to move
  vesRenderer* ren = this->renderer();

  // flip Y coordinate
  displayY = ren->height() - displayY;

  vesCamera* camera = ren->camera();
  vesVector3f cameraFocalPoint = camera->GetFocalPoint();
  vesVector3f cameraPosition = camera->GetPosition();
  vesVector3f displayFocus = ren->computeWorldToDisplay(cameraFocalPoint);
  float focalDepth = displayFocus[2];

  vesVector3f rayPoint0 = cameraPosition;
  vesVector3f rayPoint1 = ren->computeDisplayToWorld(vesVector3f(displayX, displayY, focalDepth));

  vesVector3f rayDirection = rayPoint1 - rayPoint0;

  gmtl::normalize(rayDirection);
  rayDirection *= 1000.0;
  rayPoint1 += rayDirection;

  // visualize the pick line
  /*
  vtkNew<vtkLineSource> line;
  line->SetPoint1(rayPoint0.getData());
  line->SetPoint2(rayPoint1.getData());
  line->Update();
  this->addPolyDataRepresentation(line->GetOutput());
  */

  vtkNew<vtkCellLocator> locator;
  this->Internal->AppendFilter->Update();
  locator->SetDataSet(this->Internal->AppendFilter->GetOutput());
  locator->BuildLocator();

  double p0[3] = {rayPoint0[0], rayPoint0[1], rayPoint0[2]};
  double p1[3] = {rayPoint1[0], rayPoint1[1], rayPoint1[2]};

  double pickPoint[3];
  double t;
  double paramCoords[3];
  vtkIdType cellId = -1;
  int subId;

  int result = locator->IntersectWithLine(p0, p1, 0.0, t, pickPoint, paramCoords, subId, cellId);
  if (result == 1) {
    this->Internal->SelectedImageDimension = cellId;
  }
  else {
    this->Internal->SelectedImageDimension = -1;
  }
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setVertexShaderSource(const std::string& source)
{
  this->Internal->VertexShaderSource = source;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setFragmentShaderSource(const std::string& source)
{
  this->Internal->FragmentShaderSource = source;
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::getNumberOfShadingModels() const
{
  return static_cast<int>(this->Internal->BuiltinShadingModels.size());
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::getCurrentShadingModel() const
{
  return this->Internal->CurrentShadingModel;
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::getShadingModel(int index) const
{
  std::string empty;
  if(index < 0 ||
     index > static_cast<int>(this->Internal->BuiltinShadingModels.size()))
  {
  return empty;
  }

  return this->Internal->BuiltinShadingModels[index];
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::setShadingModel(const std::string& name)
{
  bool success = true;
  return success;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initializeShaderUniforms()
{
  // Do nothing.
  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initializeShaderProgram()
{
  vesShaderProgram* shaderProgram = this->addShaderProgram(
    this->Internal->VertexShaderSource, this->Internal->FragmentShaderSource);

  this->addModelViewMatrixUniform(shaderProgram);
  this->addProjectionMatrixUniform(shaderProgram);
  this->addNormalMatrixUniform(shaderProgram);

  this->addVertexPositionAttribute(shaderProgram);
  this->addVertexNormalAttribute(shaderProgram);
  this->addVertexColorAttribute(shaderProgram);
  this->addVertexTextureCoordinateAttribute(shaderProgram);

  this->Internal->ShaderProgram = shaderProgram;
  return true;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::removeAllDataRepresentations()
{
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {

    vesKiwiDataRepresentation* rep = this->Internal->DataRepresentations[i];
    rep->removeSelfFromRenderer(this->renderer());

    if (static_cast<vesKiwiPolyDataRepresentation*>(rep)->texture()) {
      this->Internal->TextureStorage.erase(static_cast<vesKiwiPolyDataRepresentation*>(rep)->texture());
    }

    delete rep;
  }

  // todo- move to image representation class
  this->Internal->SliceReps.clear();
  this->Internal->SliceFilter = 0;
  this->Internal->AppendFilter = 0;
  this->Internal->ContourRep = 0;

  this->Internal->TextRepresentation = 0;

  this->Internal->DataRepresentations.clear();
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::addRepresentationsForDataSet(vtkDataSet* dataSet)
{

  if (vtkPolyData::SafeDownCast(dataSet)) {
    this->addPolyDataRepresentation(vtkPolyData::SafeDownCast(dataSet), this->Internal->ShaderProgram);
  }
  else if (vtkImageData::SafeDownCast(dataSet)) {

    // todo - move to image representation class
    vtkImageData* image = vtkImageData::SafeDownCast(dataSet);
    if (image->GetDataDimension() == 3) {

      double bounds[6];
      double boundsCenter[3];
      image->GetBounds(bounds);
      for (int i = 0; i < 3; ++i)
        {
        boundsCenter[i] = bounds[i*2] + (bounds[i*2+1]-bounds[i*2])/2.0;
        }

      this->Internal->AppendFilter = vtkSmartPointer<vtkAppendPolyData>::New();

      int dimensions[3];
      image->GetDimensions(dimensions);
      image->GetPointData()->GetScalars()->GetRange(this->Internal->ImageScalarRange);

      this->Internal->CurrentSliceIndices[0] = dimensions[0]/2;
      this->Internal->CurrentSliceIndices[1] = dimensions[1]/2;
      this->Internal->CurrentSliceIndices[2] = dimensions[2]/2;

      vtkNew<vtkExtractVOI> voi;
      this->Internal->SliceFilter = voi.GetPointer();
      voi->SetInput(image);

      voi->SetVOI(this->Internal->CurrentSliceIndices[0], this->Internal->CurrentSliceIndices[0], 0, dimensions[1], 0, dimensions[2]);
      voi->Update();
      this->addRepresentationsForDataSet(voi->GetOutput());

      voi->SetVOI(0, dimensions[0], this->Internal->CurrentSliceIndices[1], this->Internal->CurrentSliceIndices[1], 0, dimensions[2]);
      voi->Update();
      this->addRepresentationsForDataSet(voi->GetOutput());

      voi->SetVOI(0, dimensions[0], 0, dimensions[1], this->Internal->CurrentSliceIndices[2], this->Internal->CurrentSliceIndices[2]);
      voi->Update();
      this->addRepresentationsForDataSet(voi->GetOutput());


      vtkNew<vtkOutlineFilter> outline;
      outline->SetInput(image);
      outline->Update();
      this->addPolyDataRepresentation(outline->GetOutput(), this->Internal->ShaderProgram);

      if (image->GetNumberOfPoints() < 600000) {
        vtkNew<vtkContourFilter> contour;
        contour->SetInput(image);
        // contour value hardcoded for head image dataset
        contour->SetValue(0, 1400);
        contour->ComputeScalarsOff();
        contour->ComputeNormalsOff();
        contour->Update();
        vesKiwiPolyDataRepresentation* contourRep = this->addPolyDataRepresentation(contour->GetOutput(), this->Internal->ShaderProgram);
        contourRep->setColor(0.8, 0.8, 0.8, 0.4);
        this->Internal->ContourVis = 1;
        this->Internal->ContourRep = contourRep;
      }

    }
    else {

      // have a 2d image

      vtkSmartPointer<vtkPolyData> imagePlane = GetPolyDataForImagePlane(image);

      vesKiwiPolyDataRepresentation* rep = this->addPolyDataRepresentation(imagePlane, this->Internal->TextureShader);
      vesTexture* texture = this->newTextureFromImage(image);

      rep->setTexture(texture);

      // only do this when the append filter is present, which means this 2d image
      // is one slice from a 3d image
      if (this->Internal->AppendFilter) {
        this->Internal->AppendFilter->AddInput(imagePlane);
        this->Internal->SliceReps.push_back(rep);
      }

    }
  }
}

//----------------------------------------------------------------------------
vesKiwiPolyDataRepresentation* vesKiwiViewerApp::addPolyDataRepresentation(vtkPolyData* polyData, vesShaderProgram* program)
{
  vesKiwiPolyDataRepresentation* rep = new vesKiwiPolyDataRepresentation();
  rep->initializeWithShader(program);
  rep->setPolyData(polyData);
  rep->addSelfToRenderer(this->renderer());
  this->Internal->DataRepresentations.push_back(rep);
  return rep;
}

//----------------------------------------------------------------------------
vesKiwiPolyDataRepresentation* vesKiwiViewerApp::addTextRepresentation(const std::string& text)
{
  vtkNew<vtkTextProperty> textProperty;
  textProperty->SetFontFamilyToArial();
  textProperty->SetFontSize(32);
  textProperty->SetColor(1.0, 0.8, 0.0);

  vtkNew<vtkImageData> imageTexture;
  this->Internal->TextToImage->RenderString(textProperty.GetPointer(), text, imageTexture.GetPointer());

  // The above call to RenderString() sets the imageTexture origin so some nonzero value, not sure why
  imageTexture->SetOrigin(0, 0, 0);

  vtkSmartPointer<vtkPolyData> textQuad = GetPolyDataForImagePlane(imageTexture.GetPointer());

  vesKiwiPolyDataRepresentation* rep = this->addPolyDataRepresentation(textQuad.GetPointer(), this->Internal->TextureShader);
  vesTexture* texture = this->newTextureFromImage(imageTexture.GetPointer());
  rep->setTexture(texture);


  double bounds[6];
  imageTexture->GetBounds(bounds);
  //printf("bounds: %f %f %f %f %f %f\n", bounds[0],bounds[1],bounds[2],bounds[3],bounds[4],bounds[5]);

  double screenWidth = this->viewWidth();
  double screenHeight = this->viewHeight();
  double xmargin = 5;
  double ymargin = 5;

  double width = bounds[1] - bounds[0];
  double height = bounds[3] - bounds[2];
  double x = 0 + xmargin;
  double y = screenHeight - (height + ymargin);

  rep->actor()->setTranslation(vesVector3f(x, y, 0));
  rep->actor()->setIsOverlayActor(true);
  rep->actor()->material()->setBinNumber(20);
  return rep;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::initializeTextureShader()
{
  vesShaderProgram* shaderProgram = this->addShaderProgram(
    this->Internal->VertexShaderSource, this->Internal->FragmentShaderSource);
  this->addModelViewMatrixUniform(shaderProgram);
  this->addProjectionMatrixUniform(shaderProgram);
  this->addVertexPositionAttribute(shaderProgram);
  this->addVertexTextureCoordinateAttribute(shaderProgram);
  this->Internal->TextureShader = shaderProgram;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setBackgroundTexture(const std::string& filename)
{
  vtkSmartPointer<vtkImageData> image = vtkImageData::SafeDownCast(this->Internal->DataLoader.loadDataset(filename));
  vesTexture* backgroundTexture = this->newTextureFromImage(image);
  //this->renderer()->SetBackground(backgroundTexture);
}

//----------------------------------------------------------------------------
vesTexture* vesKiwiViewerApp::newTextureFromImage(vtkImageData* image)
{
  vesTexture* texture = new vesTexture();
  this->setTextureFromImage(texture, image);
  return texture;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setTextureFromImage(vesTexture* texture, vtkImageData* image)
{
  assert(image);
  assert(image->GetDataDimension() == 2);
  assert(image->GetPointData()->GetScalars());

  vtkSmartPointer<vtkUnsignedCharArray> pixels = vtkUnsignedCharArray::SafeDownCast(image->GetPointData()->GetScalars());

  if (!pixels) {
    vtkSmartPointer<vtkDataArray> scalars = image->GetPointData()->GetScalars();
    vtkSmartPointer<vtkLookupTable> lut = vesDataConversionTools::GetGrayscaleLookupTable(this->Internal->ImageScalarRange);
    pixels = vesDataConversionTools::MapScalars(scalars, lut);
  }

  int dimensions[3];
  image->GetDimensions(dimensions);
  const int flatDimension = GetImageFlatDimension(image);
 
  int width;
  int height;

  if (flatDimension == 2) {
    // XY plane
    width = image->GetDimensions()[0];
    height = image->GetDimensions()[1];
  }
  else if (flatDimension == 1) {
    // XZ plane
    width = image->GetDimensions()[0];
    height = image->GetDimensions()[2];
  }
  else {
    // YZ plane
    width = image->GetDimensions()[1];
    height = image->GetDimensions()[2];
  }

  vesDataConversionTools::SetTextureData(pixels, texture, width, height);
  this->Internal->TextureStorage[texture] = pixels;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::updateTextAnnotations()
{
  if (!this->Internal->TextRepresentation) {
    return;
  }

  vesVector3f worldPoint(34.3, 23.7, 0);
  vesVector3f displayPoint = this->renderer()->computeWorldToDisplay(worldPoint);
  displayPoint[2] = 0;
  this->Internal->TextRepresentation->setTranslation(displayPoint);
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::loadDataset(const std::string& filename)
{
  vtkSmartPointer<vtkDataSet> dataSet = this->Internal->DataLoader.loadDataset(filename);
  if (!dataSet) {
    return false;
  }

  this->removeAllDataRepresentations();
  this->addRepresentationsForDataSet(dataSet);

  if (filename.find("teapot.vtp") != std::string::npos) { 
    this->Internal->TextRepresentation = this->addTextRepresentation("this is a teapot");
  }

  return true;
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::loadDatasetErrorTitle() const
{
  return this->Internal->DataLoader.errorTitle();
}

//----------------------------------------------------------------------------
std::string vesKiwiViewerApp::loadDatasetErrorMessage() const
{
  return this->Internal->DataLoader.errorMessage();
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfModelFacets() const
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    count += this->Internal->DataRepresentations[i]->numberOfFacets();
  }
  return count;
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfModelVertices() const
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    count += this->Internal->DataRepresentations[i]->numberOfVertices();
  }
  return count;
}

//----------------------------------------------------------------------------
int vesKiwiViewerApp::numberOfModelLines() const
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->DataRepresentations.size(); ++i) {
    count += this->Internal->DataRepresentations[i]->numberOfLines();
  }
  return count;
}
