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

#include "vesKiwiImageWidgetRepresentation.h"

#include "vesRenderer.h"
#include "vesCamera.h"
#include "vesMapper.h"
#include "vesActor.h"
#include "vesKiwiColorMapCollection.h"
#include "vesKiwiDataConversionTools.h"
#include "vesKiwiImagePlaneDataRepresentation.h"
#include "vesKiwiPolyDataRepresentation.h"

#include <vtkNew.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkLookupTable.h>
#include <vtkOutlineFilter.h>
#include <vtkPointData.h>
#include <vtkExtractVOI.h>
#include <vtkCellLocator.h>
#include <vtkAppendPolyData.h>
#include <vtkTimerLog.h>
#include <vtkShortArray.h>
#include <vtkUnsignedShortArray.h>
#include <vtkCharArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkFloatArray.h>
#include <vtkImagePermute.h>


#include <vtkIntArray.h>
#include <vtkUnsignedIntArray.h>
#include <vtkDoubleArray.h>
#include <vtkLongArray.h>
#include <vtkUnsignedLongArray.h>
#include <vtkIdTypeArray.h>

#include <vector>
#include <map>
#include <cassert>

//----------------------------------------------------------------------------
class vesKiwiImageWidgetRepresentation::vesInternal
{
public:

  vesInternal()
  {
    this->SelectedImageDimension = -1;
    this->ImageScalarRange[0] = 0;
    this->ImageScalarRange[1] = 1;
    for (int i = 0; i < 3; ++i)
      this->CurrentSliceIndices[i] = 0;

    this->InteractionEnabled = true;
    this->WindowLevelInteractionEnabled = false;
    this->RefreshTextures = false;
  }

  int SelectedImageDimension;
  int CurrentSliceIndices[3];
  bool InteractionEnabled;
  bool WindowLevelInteractionEnabled;
  bool RefreshTextures;
  double ImageScalarRange[2];

  vesVector2f LastTouchPosition;
  vesVector3d GrabOffset;

  std::vector<vesKiwiDataRepresentation::Ptr> AllReps;
  std::vector<vesKiwiImagePlaneDataRepresentation::Ptr> SliceReps;

  std::map<int, int> TargetSliceIndex;

  vesKiwiPolyDataRepresentation::Ptr OutlineRep;

  vtkSmartPointer<vtkImageData> Image;
  vtkSmartPointer<vtkCellLocator> Locator;
  vtkSmartPointer<vtkAppendPolyData> AppendFilter;
  vtkSmartPointer<vtkLookupTable> LookupTable;

  vtkSmartPointer<vtkImageData> SliceImages[3];
};

//----------------------------------------------------------------------------
vesKiwiImageWidgetRepresentation::vesKiwiImageWidgetRepresentation()
{
  this->Internal = new vesInternal();

  this->OriginalWindow           = 1.0;
  this->OriginalLevel            = 0.5;
  this->CurrentWindow            = 1.0;
  this->CurrentLevel             = 0.5;
}

//----------------------------------------------------------------------------
vesKiwiImageWidgetRepresentation::~vesKiwiImageWidgetRepresentation()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::willRender(vesSharedPtr<vesRenderer> renderer)
{
  vesNotUsed(renderer);

  if (this->Internal->TargetSliceIndex.size()) {

    std::map<int, int>::const_iterator itr;
    for (itr = this->Internal->TargetSliceIndex.begin(); itr != this->Internal->TargetSliceIndex.end(); ++itr) {
      this->setSliceIndex(itr->first, itr->second);
    }

    this->Internal->TargetSliceIndex.clear();
  }

  if (this->Internal->RefreshTextures) {
    for (int i = 0; i < 3; ++i) {
      vtkImageData* imageData = this->Internal->SliceReps[i]->imageData();
      if (imageData && this->planeVisibility(i)) {
        this->Internal->SliceReps[i]->setImageData(imageData);
      }
    }
    this->Internal->RefreshTextures = false;
  }
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::refreshTextures()
{
  this->Internal->RefreshTextures = true;
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::setInteractionIsEnabled(bool enabled)
{
  this->Internal->InteractionEnabled = enabled;
}

//----------------------------------------------------------------------------
bool vesKiwiImageWidgetRepresentation::interactionIsEnabled() const
{
  return this->Internal->InteractionEnabled;
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::setWindowLevelInteractionEnabled(bool enabled)
{
  this->Internal->WindowLevelInteractionEnabled = enabled;
}

//----------------------------------------------------------------------------
bool vesKiwiImageWidgetRepresentation::windowLevelInteractionEnabled() const
{
  return this->Internal->WindowLevelInteractionEnabled;
}

//----------------------------------------------------------------------------
vtkImageData* vesKiwiImageWidgetRepresentation::imageData() const
{
  return this->Internal->Image;
}

//----------------------------------------------------------------------------
double* vesKiwiImageWidgetRepresentation::imageBounds()
{
  return this->imageData()->GetBounds();
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::setScrollSlice(int planeIndex)
{
  this->Internal->SelectedImageDimension = planeIndex;
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::setImageData(vtkImageData* image)
{
  this->Internal->Image = image;
  image->GetPointData()->GetScalars()->GetRange(this->Internal->ImageScalarRange);

  this->Internal->LookupTable = vtkSmartPointer<vtkLookupTable>::New();
  this->Internal->LookupTable->SetNumberOfColors( 256);
  this->Internal->LookupTable->SetHueRange( 0, 0);
  this->Internal->LookupTable->SetSaturationRange( 0, 0);
  this->Internal->LookupTable->SetValueRange( 0 ,1);
  this->Internal->LookupTable->SetAlphaRange( 1, 1);

  double range[2];
  image->GetScalarRange(range);

  this->Internal->LookupTable->SetTableRange(range[0],range[1]);
  this->Internal->LookupTable->Build();

  this->OriginalWindow = range[1] - range[0];
  this->OriginalLevel = 0.5*(range[0] + range[1]);

  if( fabs( this->OriginalWindow ) < 0.001 )
    {
    this->OriginalWindow = 0.001 * ( this->OriginalWindow < 0.0 ? -1 : 1 );
    }
  if( fabs( this->OriginalLevel ) < 0.001 )
    {
    this->OriginalLevel = 0.001 * ( this->OriginalLevel < 0.0 ? -1 : 1 );
    }

  this->setWindowLevel(this->OriginalWindow,this->OriginalLevel);
  this->Internal->RefreshTextures = false;

  // slice images will get color map information from
  // this instance's color map collection
  for (int i = 0; i < 3; ++i) {
    this->Internal->SliceReps[i]->setColorMapCollection(this->colorMapCollection());
  }

  // note, if there is a color mapping for the image scalars in the
  // color map collection then it will override this window/level lookup table
  this->colorMapCollection()->setDefaultColorMap(this->Internal->LookupTable);



  int dimensions[3];
  image->GetDimensions(dimensions);
  this->Internal->CurrentSliceIndices[0] = dimensions[0]/2;
  this->Internal->CurrentSliceIndices[1] = dimensions[1]/2;
  this->Internal->CurrentSliceIndices[2] = dimensions[2]/2;

  for (int i = 0; i < 3; ++i)
    this->setSliceIndex(i, this->Internal->CurrentSliceIndices[i]);

  vtkNew<vtkOutlineFilter> outline;
  outline->SetInputData(image);
  outline->Update();
  this->Internal->OutlineRep->setPolyData(outline->GetOutput());
  this->Internal->OutlineRep->setColor(0.5, 0.5, 0.5, 0.5);
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::invertTable()
{

  int index = this->Internal->LookupTable->GetNumberOfTableValues();
  unsigned char swap[4];
  size_t num = 4*sizeof(unsigned char);
  vtkUnsignedCharArray* table = this->Internal->LookupTable->GetTable();
  for ( int count = 0; count < --index; count++ )
    {
    unsigned char *rgba1 = table->GetPointer(4*count);
    unsigned char *rgba2 = table->GetPointer(4*index);
    memcpy( swap,  rgba1, num );
    memcpy( rgba1, rgba2, num );
    memcpy( rgba2, swap,  num );
    }

  // force the lookuptable to update its InsertTime to avoid
  // rebuilding the array
  this->Internal->LookupTable->SetTableValue(0, this->Internal->LookupTable->GetTableValue(0));
}

//----------------------------------------------------------------------------
double vesKiwiImageWidgetRepresentation::window() const
{
  return this->CurrentWindow;
}

//----------------------------------------------------------------------------
double vesKiwiImageWidgetRepresentation::level() const
{
  return this->CurrentLevel;
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::resetWindowLevel()
{
  this->setWindowLevel(this->OriginalWindow, this->OriginalLevel);
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::setWindowLevel(double window, double level)
{
  if (this->CurrentWindow == window && this->CurrentLevel == level)
    {
    return;
    }

  // if the new window is negative and the old window was positive invert table
  if (   ( window < 0 && this->CurrentWindow > 0 )
      || ( window > 0 && this->CurrentWindow < 0 ))
    {
    this->invertTable();
    }

  this->CurrentWindow = window;
  this->CurrentLevel = level;


  double rmin = this->CurrentLevel - 0.5*fabs( this->CurrentWindow );
  double rmax = rmin + fabs( this->CurrentWindow );
  this->Internal->LookupTable->SetTableRange( rmin, rmax );

  this->Internal->RefreshTextures = true;
}


//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::initializeWithShader(
  vesSharedPtr<vesShaderProgram> geometryShader,
  vesSharedPtr<vesShaderProgram> textureShader)
{

  this->Internal->AppendFilter = vtkSmartPointer<vtkAppendPolyData>::New();


  this->Internal->OutlineRep = vesKiwiPolyDataRepresentation::Ptr(new vesKiwiPolyDataRepresentation);
  this->Internal->OutlineRep->initializeWithShader(geometryShader);
  this->Internal->OutlineRep->setBinNumber(2);
  this->Internal->AllReps.push_back(this->Internal->OutlineRep);

  for (int i = 0; i < 3; ++i) {
    vesKiwiImagePlaneDataRepresentation::Ptr rep(new vesKiwiImagePlaneDataRepresentation);
    rep->setTextureSurfaceShader(textureShader);
    rep->initializeWithShader(textureShader);
    rep->setBinNumber(1);
    this->Internal->SliceReps.push_back(rep);
    this->Internal->AllReps.push_back(rep);
    this->Internal->AppendFilter->AddInputData(vtkSmartPointer<vtkPolyData>::New());
  }

}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::setOutlineVisible(bool visible)
{
  if (this->Internal->OutlineRep) {
    this->Internal->OutlineRep->actor()->setVisible(visible);
  }
}

//----------------------------------------------------------------------------
bool vesKiwiImageWidgetRepresentation::scrollSliceModeActive() const
{
  return (this->Internal->SelectedImageDimension >= 0);
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::setPlaneVisibility(int planeIndex, bool visible)
{
  assert(planeIndex >= 0 && planeIndex <= 2);

  this->Internal->SliceReps[planeIndex]->actor()->setVisible(visible);
}

//----------------------------------------------------------------------------
bool vesKiwiImageWidgetRepresentation::planeVisibility(int planeIndex) const
{
  assert(planeIndex >= 0 && planeIndex <= 2);
  return this->Internal->SliceReps[planeIndex]->actor()->isVisible();
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::scrollImageSlice(double deltaX, double deltaY)
{
  deltaY *= -1;

  vesSharedPtr<vesRenderer> ren = this->renderer();
  vesVector3f viewFocus = ren->camera()->focalPoint();
  vesVector3f viewFocusDisplay = ren->computeWorldToDisplay(viewFocus);
  float focalDepth = viewFocusDisplay[2];

  this->Internal->LastTouchPosition += vesVector2f(deltaX, deltaY);
  vesVector3f worldTouchPosition = ren->computeDisplayToWorld(vesVector3f(this->Internal->LastTouchPosition[0], this->Internal->LastTouchPosition[1], focalDepth));
  worldTouchPosition -= this->Internal->GrabOffset.cast<float>();

  int flatDimension = this->Internal->SelectedImageDimension;
  vesVector3f planeNormal(0, 0, 0);
  planeNormal[flatDimension] = 1.0;

  int extent[6];
  vesVector3d spacing;
  vesVector3d origin;
  this->imageData()->GetOrigin(origin.data());
  this->imageData()->GetSpacing(spacing.data());
  this->imageData()->GetExtent(extent);

  double distanceAlongInteractionAxis = worldTouchPosition.dot(planeNormal);
  double distanceFromOriginAlongAxis = distanceAlongInteractionAxis - origin[flatDimension];
  int slicesFromOrigin = distanceFromOriginAlongAxis / spacing[flatDimension] - extent[flatDimension*2]*spacing[flatDimension];

  this->scheduleSetSliceIndex(flatDimension, slicesFromOrigin);
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::scheduleSetSliceIndex(int planeIndex, int sliceIndex)
{
  int dimensions[3];
  this->imageData()->GetDimensions(dimensions);

  if (sliceIndex < 0) {
    sliceIndex = 0;
  }
  else if (sliceIndex >= dimensions[planeIndex]) {
    sliceIndex = dimensions[planeIndex] - 1;
  }

  this->Internal->TargetSliceIndex[planeIndex] = sliceIndex;
  this->Internal->CurrentSliceIndices[planeIndex] = sliceIndex;
}


//vtkIdType GetLinearIndex(const int i, const int j, const int k, const int N1, const int N2 )
//{
//  return( (k*N2+j)*N1+i );
//}

//----------------------------------------------------------------------------
int vesKiwiImageWidgetRepresentation::numberOfSlices(int planeIndex) const
{
  assert(planeIndex >= 0 && planeIndex <= 2);
  return this->imageData()->GetDimensions()[planeIndex];
}

//----------------------------------------------------------------------------
int vesKiwiImageWidgetRepresentation::sliceIndex(int planeIndex) const
{
  assert(planeIndex >= 0 && planeIndex <= 2);
  return this->Internal->CurrentSliceIndices[planeIndex];
}


template <typename VTKARRAYTYPE, typename PRIMITIVETYPE>
void extractSliceExecute(vtkImageData *inImage, vtkImageData *sliceImage, int planeIndex, int sliceIndex)
{
  PRIMITIVETYPE* pixels = VTKARRAYTYPE::SafeDownCast(inImage->GetPointData()->GetScalars())->GetPointer(0);
  PRIMITIVETYPE* data = VTKARRAYTYPE::SafeDownCast(sliceImage->GetPointData()->GetScalars())->GetPointer(0);

  int extent[6];
  int dimensions[3];
  inImage->GetExtent(extent);
  inImage->GetDimensions(dimensions);


  if (planeIndex == 0) {

    // x axis, yz plane
    const vtkIdType x = sliceIndex;
    for (vtkIdType y = 0; y < dimensions[1]; ++y) {
      for (vtkIdType z = 0; z < dimensions[2]; ++z) {
        data[z*dimensions[1] + y] = pixels[(z * (dimensions[0]*dimensions[1])) + (y*dimensions[0]) + x];
      }
    }

    extent[0] = extent[1] = extent[0] + sliceIndex;
  }
  else if (planeIndex == 1) {

    // y axis, xz plane
    const size_t y = sliceIndex;
    for (size_t x= 0; x < dimensions[0]; ++x) {
      for (size_t z = 0; z < dimensions[2]; ++z) {
        data[z*dimensions[0] + x] = pixels[(z * (dimensions[0]*dimensions[1])) + (y*dimensions[0]) + x];
      }
    }

    extent[2] = extent[3] = extent[2] + sliceIndex;
  }
  else {

    // z axis, xy plane
    const size_t z = sliceIndex;
    for (size_t x = 0; x < dimensions[0]; ++x) {
      for (size_t y = 0; y < dimensions[1]; ++y) {
        data[y*dimensions[0] + x] = pixels[(z * (dimensions[0]*dimensions[1])) + (y*dimensions[0]) + x];
      }
    }

    extent[4] = extent[5] = extent[4] + sliceIndex;
  }

  sliceImage->SetExtent(extent);
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::setSliceIndex(int planeIndex, int sliceIndex)
{
  int dimensions[3];
  this->imageData()->GetDimensions(dimensions);

  if (sliceIndex < 0) {
    sliceIndex = 0;
  }
  else if (sliceIndex >= dimensions[planeIndex]) {
    sliceIndex = dimensions[planeIndex] - 1;
  }

  // allocate images if needed
  if (planeIndex == 0) {
    if (!this->Internal->SliceImages[0]) {
      this->Internal->SliceImages[0] = vtkSmartPointer<vtkImageData>::New();
      this->Internal->SliceImages[0]->SetOrigin(this->imageData()->GetOrigin());
      this->Internal->SliceImages[0]->SetSpacing(this->imageData()->GetSpacing());
      this->Internal->SliceImages[0]->SetDimensions(1, dimensions[1], dimensions[2]);
      this->Internal->SliceImages[0]->AllocateScalars(this->imageData()->GetScalarType(), 1);
      this->Internal->SliceImages[0]->GetPointData()->GetScalars()->SetName(this->imageData()->GetPointData()->GetScalars()->GetName());
    }
  }
  else if (planeIndex == 1) {
    if (!this->Internal->SliceImages[1]) {
      this->Internal->SliceImages[1] = vtkSmartPointer<vtkImageData>::New();
      this->Internal->SliceImages[1]->SetOrigin(this->imageData()->GetOrigin());
      this->Internal->SliceImages[1]->SetSpacing(this->imageData()->GetSpacing());
      this->Internal->SliceImages[1]->SetDimensions(dimensions[0], 1, dimensions[2]);
      this->Internal->SliceImages[1]->AllocateScalars(this->imageData()->GetScalarType(), 1);
      this->Internal->SliceImages[1]->GetPointData()->GetScalars()->SetName(this->imageData()->GetPointData()->GetScalars()->GetName());
    }
  }
  else {
    if (!this->Internal->SliceImages[2]) {
      this->Internal->SliceImages[2] = vtkSmartPointer<vtkImageData>::New();
      this->Internal->SliceImages[2]->SetOrigin(this->imageData()->GetOrigin());
      this->Internal->SliceImages[2]->SetSpacing(this->imageData()->GetSpacing());
      this->Internal->SliceImages[2]->SetDimensions(dimensions[0], dimensions[1], 1);
      this->Internal->SliceImages[2]->AllocateScalars(this->imageData()->GetScalarType(), 1);
      this->Internal->SliceImages[2]->GetPointData()->GetScalars()->SetName(this->imageData()->GetPointData()->GetScalars()->GetName());
    }
  }

  vtkImageData* sliceImage = this->Internal->SliceImages[planeIndex];

  #define mycall(vtktypename, vtkarraytype, primitivetype)   \
    case vtktypename: extractSliceExecute<vtkarraytype, primitivetype>(this->imageData(), sliceImage, planeIndex, sliceIndex); break;

  switch (this->imageData()->GetScalarType())
    {
      mycall(VTK_CHAR, vtkCharArray, char);
      mycall(VTK_UNSIGNED_CHAR, vtkUnsignedCharArray, unsigned char);
      mycall(VTK_SHORT, vtkShortArray, short);
      mycall(VTK_UNSIGNED_SHORT,vtkUnsignedShortArray, unsigned short);
      mycall(VTK_INT, vtkIntArray, int);
      mycall(VTK_UNSIGNED_INT,vtkUnsignedIntArray, unsigned int);
      mycall(VTK_LONG, vtkLongArray, long);
      mycall(VTK_UNSIGNED_LONG,vtkUnsignedLongArray, unsigned long);
      mycall(VTK_FLOAT, vtkFloatArray, float);
      mycall(VTK_DOUBLE, vtkDoubleArray, double);
      mycall(VTK_ID_TYPE, vtkIdTypeArray, vtkIdType);
    default:
      vtkGenericWarningMacro("Execute: Unknown input ScalarType");
      return;
    }


  vesKiwiImagePlaneDataRepresentation::Ptr rep = this->Internal->SliceReps[planeIndex];
  rep->setImageData(sliceImage);

  this->Internal->AppendFilter->GetInput(planeIndex)->DeepCopy(rep->imagePlanePolyData());
  this->Internal->CurrentSliceIndices[planeIndex] = sliceIndex;
}

//----------------------------------------------------------------------------
bool vesKiwiImageWidgetRepresentation::handleSingleTouchPanGesture(double deltaX, double deltaY)
{
  if (!this->interactionIsActive()) {
    return false;
  }

  if (this->Internal->WindowLevelInteractionEnabled) {
    this->setWindowLevel(this->CurrentWindow + deltaX, this->CurrentLevel+deltaY);
  }
  else {
    this->scrollImageSlice(deltaX, deltaY);
  }
  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiImageWidgetRepresentation::handleSingleTouchDown(int displayX, int displayY)
{
  if (!this->Internal->InteractionEnabled) {
    return false;
  }

  // calculate the focal depth so we'll know how far to move
  vesSharedPtr<vesRenderer> ren = this->renderer();

  // flip Y coordinate
  displayY = ren->height() - displayY;
  this->Internal->LastTouchPosition = vesVector2f(displayX, displayY);

  vesSharedPtr<vesCamera> camera = ren->camera();
  vesVector3f cameraFocalPoint = camera->focalPoint();
  vesVector3f cameraPosition = camera->position();
  vesVector3f displayFocus = ren->computeWorldToDisplay(cameraFocalPoint);
  float focalDepth = displayFocus[2];

  vesVector3f rayPoint0 = cameraPosition;
  vesVector3f rayPoint1 = ren->computeDisplayToWorld(vesVector3f(displayX, displayY, focalDepth));
  vesVector3f touchWorldPosition = rayPoint1;

  vesVector3f rayDirection = rayPoint1 - rayPoint0;
  rayDirection.normalize();
  rayPoint1 += rayDirection*1e6;


  vtkNew<vtkAppendPolyData> appendFilter;
  std::vector<int> cellIdToPlaneId;

  for (int i = 0; i < 3; ++i) {
    if (this->planeVisibility(i)) {
      appendFilter->AddInputData(this->Internal->SliceReps[i]->imagePlanePolyData());
      cellIdToPlaneId.push_back(i);
    }
  }

  appendFilter->Update();

  vtkNew<vtkCellLocator> locator;
  locator->SetDataSet(appendFilter->GetOutput());
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
    this->Internal->SelectedImageDimension = cellIdToPlaneId[cellId];
    this->interactionOn();

    int currentDimension = this->Internal->SelectedImageDimension;
    int currentSliceIndex = this->Internal->CurrentSliceIndices[this->Internal->SelectedImageDimension];
    int extent[6];
    vesVector3d spacing;
    vesVector3d origin;
    this->imageData()->GetOrigin(origin.data());
    this->imageData()->GetSpacing(spacing.data());
    this->imageData()->GetExtent(extent);
    double sliceDistanceAlongAxis = spacing[currentDimension] * (currentSliceIndex + extent[currentDimension*2]);
    origin[currentDimension] += sliceDistanceAlongAxis;
    this->Internal->GrabOffset = touchWorldPosition.cast<double>() - origin;

  }
  else {
    this->Internal->SelectedImageDimension = -1;
    this->interactionOff();
  }

  return this->interactionIsActive();
}

//----------------------------------------------------------------------------
bool vesKiwiImageWidgetRepresentation::handleDoubleTap(int displayX, int displayY)
{
  vesNotUsed(displayX);
  vesNotUsed(displayY);

  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiImageWidgetRepresentation::handleSingleTouchUp()
{
  if (!this->interactionIsActive()) {
    return false;
  }

  this->Internal->SelectedImageDimension = -1;
  this->interactionOff();
  return true;
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::addSelfToRenderer(
  vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::addSelfToRenderer(renderer);
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i) {
    this->Internal->AllReps[i]->addSelfToRenderer(renderer);
  }
}

//----------------------------------------------------------------------------
void vesKiwiImageWidgetRepresentation::removeSelfFromRenderer(
  vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::removeSelfFromRenderer(renderer);
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i) {
    this->Internal->AllReps[i]->removeSelfFromRenderer(renderer);
  }
}
