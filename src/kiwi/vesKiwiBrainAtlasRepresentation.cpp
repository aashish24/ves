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

#include "vesKiwiBrainAtlasRepresentation.h"

#include "vesRenderer.h"
#include "vesCamera.h"
#include "vesMapper.h"
#include "vesActor.h"
#include "vesKiwiDataLoader.h"
#include "vesKiwiText2DRepresentation.h"
#include "vesKiwiPolyDataRepresentation.h"

#include <vtkNew.h>
#include <vtkBoundingBox.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkArrowSource.h>
#include <vtkPolyData.h>
#include <vtkOutlineFilter.h>
#include <vtkPointData.h>
#include <vtkCellLocator.h>
#include <vtkAppendPolyData.h>
#include <vtkTransform.h>

#include <vtksys/SystemTools.hxx>

#include <vector>
#include <cassert>

//----------------------------------------------------------------------------
class vesKiwiBrainAtlasRepresentation::vesInternal
{
public:

  vesInternal()
  {
    this->TextVisible = false;
    this->ActiveModel = -1;
    this->SkinOpacity = 1.0;
  }

  ~vesInternal()
  {
  }

  std::vector<vesKiwiDataRepresentation::Ptr> AllReps;

  vesSharedPtr<vesShaderProgram> GeometryShader;
  vesSharedPtr<vesShaderProgram> TextureShader;
  vesSharedPtr<vesShaderProgram> ClipShader;

  vesKiwiPolyDataRepresentation::Ptr SkinRep;
  vesKiwiPolyDataRepresentation::Ptr SkullRep;
  vesKiwiText2DRepresentation::Ptr TextRep;

  bool TextVisible;
  int ActiveModel;
  double SkinOpacity;
  std::vector<vtkSmartPointer<vtkCellLocator> > Locators;
  std::vector<std::string> AnatomicalNames;
  std::vector<vesKiwiPolyDataRepresentation::Ptr> AnatomicalModels;
  std::vector<vesVector3f> Colors;
  std::vector<vesVector3f> AnnotationAnchors;
  std::vector<double> AnchorOffsets;
};

//----------------------------------------------------------------------------
vesKiwiBrainAtlasRepresentation::vesKiwiBrainAtlasRepresentation()
{
  this->Internal = new vesInternal();
}

//----------------------------------------------------------------------------
vesKiwiBrainAtlasRepresentation::~vesKiwiBrainAtlasRepresentation()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vesKiwiBrainAtlasRepresentation::initializeWithShader(vesSharedPtr<vesShaderProgram> geometryShader,
                                                           vesSharedPtr<vesShaderProgram> textureShader,
                                                           vesSharedPtr<vesShaderProgram> clipShader)
{

  this->Internal->TextRep = vesKiwiText2DRepresentation::Ptr(new vesKiwiText2DRepresentation);
  this->Internal->TextRep->initializeWithShader(textureShader);
  this->Internal->TextRep->setWorldAnchorPointEnabled(true);
  this->Internal->AllReps.push_back(this->Internal->TextRep);

  this->Internal->GeometryShader = geometryShader;
  this->Internal->TextureShader = textureShader;
  this->Internal->ClipShader = clipShader;
}

//----------------------------------------------------------------------------
void vesKiwiBrainAtlasRepresentation::loadData(const std::string& filename)
{
  this->Internal->Locators.clear();

  std::ifstream f;

  std::string modelInfoFile = filename;
  std::string dataDir = vtksys::SystemTools::GetFilenamePath(modelInfoFile);

  std::cout << "Reading model info file: " << modelInfoFile << std::endl;

  f.open(modelInfoFile.c_str());

  while (!f.eof()) {

    std::string anatomicalName;
    std::string modelFile;
    double color[3];

    f >> anatomicalName;
    if (!anatomicalName.size()) {
      break;
    }

    f >> color[0] >> color[1] >> color[2] >> modelFile;

    modelFile = dataDir + "/" + modelFile;

    vesKiwiDataLoader dataLoader;
    vtkSmartPointer<vtkPolyData> polyData = vtkPolyData::SafeDownCast(dataLoader.loadDataset(modelFile));
    if (!polyData) {
      std::cout << "Failed to read: " << modelFile << std::endl;
      continue;
    }

    vesSharedPtr<vesShaderProgram> shader = this->Internal->GeometryShader;

    double opacity = 1.0;
    int binNumber = 2;
    if (anatomicalName == "Skin" || anatomicalName == "skull_bone") {
      shader = this->Internal->ClipShader;
    }
    if (anatomicalName == "Skin" || anatomicalName == "skull_bone") {
      opacity = this->Internal->SkinOpacity;
      binNumber = 5;
    }

    vesKiwiPolyDataRepresentation::Ptr rep = vesKiwiPolyDataRepresentation::Ptr(new vesKiwiPolyDataRepresentation);
    rep->initializeWithShader(shader);
    rep->setPolyData(polyData);
    rep->setColor(color[0], color[1], color[2], opacity);
    rep->setBinNumber(binNumber);
    this->Internal->AllReps.push_back(rep);

    if (anatomicalName != "Skin" && anatomicalName != "skull_bone") {

      vtkSmartPointer<vtkCellLocator> locator = vtkSmartPointer<vtkCellLocator>::New();
      locator->SetDataSet(polyData);
      locator->BuildLocator();
      this->Internal->Locators.push_back(locator);
      this->Internal->AnatomicalNames.push_back(anatomicalName);
      this->Internal->AnatomicalModels.push_back(rep);
      this->Internal->Colors.push_back(vesVector3f(color[0], color[1], color[2]));


      double center[3];
      vesVector3f anchor;
      vtkBoundingBox bounds;
      bounds.SetBounds(polyData->GetBounds());
      bounds.GetCenter(center);
      anchor[0] = center[0];
      anchor[1] = center[1];
      anchor[2] = center[2];
      double modelRadius = bounds.GetDiagonalLength() / 2.0;
      this->Internal->AnnotationAnchors.push_back(anchor);
      this->Internal->AnchorOffsets.push_back(modelRadius);
    }
    if (anatomicalName == "Skin") {
      this->Internal->SkinRep = rep;
    }
    else if (anatomicalName == "skull_bone") {
      this->Internal->SkullRep = rep;
    }
  }
}

//----------------------------------------------------------------------------
namespace {

double PickDataSet(vtkCellLocator* locator, const vesVector3f& rayPoint0, const vesVector3f& rayPoint1)
{
  double p0[3] = {rayPoint0[0], rayPoint0[1], rayPoint0[2]};
  double p1[3] = {rayPoint1[0], rayPoint1[1], rayPoint1[2]};

  double pickPoint[3];
  double t;
  double paramCoords[3];
  vtkIdType cellId = -1;
  int subId;

  int result = locator->IntersectWithLine(p0, p1, 0.0, t, pickPoint, paramCoords, subId);//, cellId);
  if (result == 1) {
      return t;
  }

  return -1;
}

std::string GetHumanReadableName(std::string name)
{
  vtksys::SystemTools::ReplaceString(name, "_R", " (right)");
  vtksys::SystemTools::ReplaceString(name, "_L", " (left)");
  vtksys::SystemTools::ReplaceString(name, "_", " ");
  name = vtksys::SystemTools::CapitalizedWords(name);
  return name;
}

} //end namespace


//----------------------------------------------------------------------------
bool vesKiwiBrainAtlasRepresentation::handleSingleTouchTap(int displayX, int displayY)
{
  vesSharedPtr<vesRenderer> ren = this->renderer();
  displayY = ren->height() - displayY;

  int cornerSize = 50;
  if (displayX < cornerSize && displayY < cornerSize) {

    if (!this->Internal->SkinRep) {
      return false;
    }

    vesVector4f skinColor = this->Internal->SkinRep->color();
    vesVector4f skullColor = this->Internal->SkullRep->color();
    if (this->Internal->SkinOpacity == 1.0) {
      this->Internal->SkinOpacity = 0.3;
      this->Internal->SkinRep->setColor(skinColor[0], skinColor[1], skinColor[2], this->Internal->SkinOpacity);
      this->Internal->SkullRep->setColor(skullColor[0], skullColor[1], skullColor[2], this->Internal->SkinOpacity);
    }
    else if (this->Internal->SkinOpacity == 0.3) {
      this->Internal->SkinOpacity = 0.0;
      this->Internal->SkinRep->removeSelfFromRenderer(this->renderer());
      this->Internal->SkullRep->removeSelfFromRenderer(this->renderer());
    }
    else if (this->Internal->SkinOpacity == 0.0) {
      this->Internal->SkinOpacity = 1.0;
      this->Internal->SkinRep->setColor(skinColor[0], skinColor[1], skinColor[2], this->Internal->SkinOpacity);
      this->Internal->SkullRep->setColor(skullColor[0], skullColor[1], skullColor[2], this->Internal->SkinOpacity);
      this->Internal->SkinRep->addSelfToRenderer(this->renderer());
      this->Internal->SkullRep->addSelfToRenderer(this->renderer());
    }
    return true;
  }
  else if (displayX < cornerSize && (ren->height() - displayY) < cornerSize) {
    return false;
  }

  vesVector3f rayPoint0 = ren->computeDisplayToWorld(vesVector3f(displayX, displayY, /*focalDepth=*/0.0));
  vesVector3f rayPoint1 = ren->computeDisplayToWorld(vesVector3f(displayX, displayY, /*focalDepth=*/1.0));

  int tappedModel = -1;
  double minDist = -1;

  for (size_t i = 0; i < this->Internal->Locators.size(); ++i) {
    if (this->Internal->ActiveModel >= 0 && i != this->Internal->ActiveModel) {
      continue;
    }

    double dist = PickDataSet(this->Internal->Locators[i], rayPoint0, rayPoint1);
    if (dist >= 0 && (minDist == -1 || dist < minDist)) {
      tappedModel = i;
      minDist = dist;
    }
  }

  if (this->Internal->ActiveModel == -1 && tappedModel == -1) {
    return false;
  }

  if (tappedModel == this->Internal->ActiveModel) {
    tappedModel = -1;
  }

  this->Internal->ActiveModel = tappedModel;

  if (tappedModel >= 0) {
    for (size_t i = 0; i < this->Internal->AnatomicalModels.size(); ++i) {
      this->Internal->AnatomicalModels[i]->removeSelfFromRenderer(this->renderer());
    }
    this->Internal->AnatomicalModels[tappedModel]->addSelfToRenderer(this->renderer());

    std::string anatomicalName = this->Internal->AnatomicalNames[tappedModel];

    anatomicalName = GetHumanReadableName(anatomicalName);

    this->Internal->TextRep->setText(anatomicalName);
    vesVector3f worldPoint = this->Internal->AnnotationAnchors[this->Internal->ActiveModel];
    double anchorOffset = this->Internal->AnchorOffsets[this->Internal->ActiveModel];

    this->Internal->TextRep->setWorldAnchorPoint(worldPoint);
    this->Internal->TextRep->setAnchorOffset(anchorOffset);
    this->Internal->TextRep->setWorldAnchorPointEnabled(true);
    this->Internal->TextRep->addSelfToRenderer(this->renderer());
    this->Internal->TextVisible = true;

  }
  else {
    for (size_t i = 0; i < this->Internal->AnatomicalModels.size(); ++i) {
      this->Internal->AnatomicalModels[i]->addSelfToRenderer(this->renderer());
    }
    this->Internal->TextRep->removeSelfFromRenderer(this->renderer());
    this->Internal->TextVisible = false;
  }

  return true;
}

//----------------------------------------------------------------------------
void vesKiwiBrainAtlasRepresentation::willRender(vesSharedPtr<vesRenderer> renderer)
{
  if (this->Internal->TextVisible) {
    this->Internal->TextRep->willRender(renderer);
  }
}

//----------------------------------------------------------------------------
void vesKiwiBrainAtlasRepresentation::addSelfToRenderer(vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::addSelfToRenderer(renderer);
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i) {
    if (this->Internal->AllReps[i] == this->Internal->TextRep && !this->Internal->TextVisible) {
      continue;
    }
    this->Internal->AllReps[i]->addSelfToRenderer(renderer);
  }
}

//----------------------------------------------------------------------------
void vesKiwiBrainAtlasRepresentation::removeSelfFromRenderer(vesSharedPtr<vesRenderer> renderer)
{
  this->Superclass::removeSelfFromRenderer(renderer);
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i) {
    this->Internal->AllReps[i]->removeSelfFromRenderer(renderer);
  }
}

//----------------------------------------------------------------------------
int vesKiwiBrainAtlasRepresentation::numberOfFacets()
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i)
    count += this->Internal->AllReps[i]->numberOfFacets();
  return count;
}

//----------------------------------------------------------------------------
int vesKiwiBrainAtlasRepresentation::numberOfVertices()
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i)
    count += this->Internal->AllReps[i]->numberOfVertices();
  return count;
}

//----------------------------------------------------------------------------
int vesKiwiBrainAtlasRepresentation::numberOfLines()
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->AllReps.size(); ++i)
    count += this->Internal->AllReps[i]->numberOfLines();
  return count;
}
