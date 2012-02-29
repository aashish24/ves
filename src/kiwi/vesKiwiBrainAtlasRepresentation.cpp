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
#include "vesEigen.h"

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
    this->SkinRepIndex = -1;
    this->SkullRepIndex = -1;
    this->SkinOpacity = 1.0;
  }

  ~vesInternal()
  {
  }

  std::vector<vesKiwiDataRepresentation::Ptr> AllReps;

  vesSharedPtr<vesShaderProgram> GeometryShader;
  vesSharedPtr<vesShaderProgram> TextureShader;
  vesSharedPtr<vesShaderProgram> ClipShader;

  vesKiwiText2DRepresentation::Ptr TextRep;

  bool TextVisible;
  int ActiveModel;
  int SkinRepIndex;
  int SkullRepIndex;
  double SkinOpacity;
  vesKiwiPolyDataRepresentation::Ptr SkinRep;
  std::vector<bool> ModelStatus;
  std::vector<bool> ModelSceneStatus;
  std::vector<vtkSmartPointer<vtkCellLocator> > Locators;
  std::vector<std::string> AnatomicalNames;
  std::vector<vesKiwiPolyDataRepresentation::Ptr> AnatomicalModels;
  std::vector<vesVector3f> Colors;
  std::vector<vesVector3f> AnnotationAnchors;
  std::vector<double> AnchorOffsets;

  vtkSmartPointer<vtkPlane> Plane;
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

    vesKiwiPolyDataRepresentation::Ptr rep = vesKiwiPolyDataRepresentation::Ptr(new vesKiwiPolyDataRepresentation);
    rep->initializeWithShader(shader);
    rep->setPolyData(polyData);
    rep->setColor(color[0], color[1], color[2], opacity);
    rep->setBinNumber(binNumber);
    this->Internal->AllReps.push_back(rep);

    vtkSmartPointer<vtkCellLocator> locator = vtkSmartPointer<vtkCellLocator>::New();
    locator->SetDataSet(polyData);
    locator->BuildLocator();
    this->Internal->Locators.push_back(locator);
    this->Internal->AnatomicalNames.push_back(anatomicalName);
    this->Internal->AnatomicalModels.push_back(rep);
    this->Internal->ModelStatus.push_back(true);
    this->Internal->ModelSceneStatus.push_back(true);
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

    if (anatomicalName == "Skin") {
      this->Internal->SkinRepIndex = this->Internal->AnatomicalModels.size() - 1;
      this->Internal->SkinRep = rep;
      rep->setBinNumber(5);
    }
    else if (anatomicalName == "skull_bone") {
      this->Internal->SkullRepIndex = this->Internal->AnatomicalModels.size() - 1;
    }
  }
}

//----------------------------------------------------------------------------
namespace {

double PickDataSet(vtkCellLocator* locator, vesVector3d& rayPoint0, vesVector3d& rayPoint1)
{
  double pickPoint[3];
  double t;
  double paramCoords[3];
  int subId;

  int result = locator->IntersectWithLine(rayPoint0.data(), rayPoint1.data(), 0.0, t, pickPoint, paramCoords, subId);
  if (result == 1) {
      return t*vesVector3d(rayPoint1 - rayPoint0).norm();
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
bool vesKiwiBrainAtlasRepresentation::handleLongPress(int displayX, int displayY)
{
  vesNotUsed(displayX);
  vesNotUsed(displayY);

  this->deselectModel();

  // show all models
  for (size_t i = 0; i < this->Internal->AnatomicalModels.size(); ++i) {
    if (!this->Internal->ModelStatus[i]) {
      this->Internal->AnatomicalModels[i]->addSelfToRenderer(this->renderer());
      this->Internal->ModelStatus[i] = true;
      this->Internal->ModelSceneStatus[i] = true;
    }
  }

  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiBrainAtlasRepresentation::handleDoubleTap(int displayX, int displayY)
{
  if (this->Internal->ActiveModel == -1) {
    return this->selectModel(displayX, displayY);
  }
  else {
    this->deselectModel();
    return true;
  }
}

//----------------------------------------------------------------------------
void vesKiwiBrainAtlasRepresentation::setClipPlane(vtkPlane* plane)
{
  this->Internal->Plane = plane;
}

//----------------------------------------------------------------------------
int vesKiwiBrainAtlasRepresentation::findTappedModel(int displayX, int displayY)
{

  vesSharedPtr<vesRenderer> ren = this->renderer();
  displayY = ren->height() - displayY;

  vesVector3d rayPoint0 = ren->computeDisplayToWorld(vesVector3f(displayX, displayY, /*focalDepth=*/0.0)).cast<double>();
  vesVector3d rayPoint1 = ren->computeDisplayToWorld(vesVector3f(displayX, displayY, /*focalDepth=*/1.0)).cast<double>();

  int tappedModel = -1;
  double minDist = -1;

  double extraDist = 0;
  for (size_t i = 0; i < this->Internal->Locators.size(); ++i) {

    if (!this->Internal->ModelStatus[i])
      continue;

    vesVector3d pickRayPoint0 = rayPoint0;
    extraDist = 0.0;

    if (static_cast<int>(i) == this->Internal->SkullRepIndex
        || static_cast<int>(i) == this->Internal->SkinRepIndex) {

      // set rayPoint0 to intersection of ray with plane so that the ray
      // only intersects with the non-clipped portion of these models
      if (this->Internal->Plane && this->Internal->Plane->EvaluateFunction (rayPoint0.data()) > 0) {
        double t;
        vesVector3d planeIntersection;
        this->Internal->Plane->IntersectWithLine(rayPoint0.data(), rayPoint1.data(), t, planeIntersection.data());
        pickRayPoint0 = planeIntersection;
        vesVector3d rayToPlane = pickRayPoint0 - rayPoint0;
        extraDist = rayToPlane.norm();
      }
    }

    double dist = PickDataSet(this->Internal->Locators[i], pickRayPoint0, rayPoint1);

    if (dist >= 0)
      dist += extraDist;

    if (dist >= 0 && (minDist == -1 || dist < minDist)) {
      tappedModel = i;
      minDist = dist;
    }
  }

  return tappedModel;
}

//----------------------------------------------------------------------------
bool vesKiwiBrainAtlasRepresentation::hideModel(int displayX, int displayY)
{
  int modelIndex = this->findTappedModel(displayX, displayY);

  if (modelIndex >= 0) {
    this->Internal->AnatomicalModels[modelIndex]->removeSelfFromRenderer(this->renderer());
    this->Internal->ModelStatus[modelIndex] = false;
    this->Internal->ModelSceneStatus[modelIndex] = false;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
void vesKiwiBrainAtlasRepresentation::showTextLabel(int modelIndex) {

  std::string anatomicalName = this->Internal->AnatomicalNames[modelIndex];

  anatomicalName = GetHumanReadableName(anatomicalName);

  this->Internal->TextRep->setText(anatomicalName);
  this->Internal->TextRep->addSelfToRenderer(this->renderer());

  bool useWorldAnchor = true;

  if (!useWorldAnchor) {
    this->Internal->TextRep->actor()->setIsOverlayNode(true);
    this->Internal->TextRep->setBinNumber(21);
    vesVector2f displayPosition;
    displayPosition[0] = (this->renderer()->width()/2.0) - (this->Internal->TextRep->textWidth()/2.0);
    displayPosition[1] = 150;
    this->Internal->TextRep->setDisplayPosition(displayPosition);
    this->Internal->TextRep->setWorldAnchorPointEnabled(false);
  }
  else {

    vesVector3f worldPoint = this->Internal->AnnotationAnchors[modelIndex];
    double anchorOffset = this->Internal->AnchorOffsets[modelIndex];

    this->Internal->TextRep->setWorldAnchorPoint(worldPoint);
    this->Internal->TextRep->setAnchorOffset(anchorOffset);
    this->Internal->TextRep->setWorldAnchorPointEnabled(true);
  }


  this->Internal->TextVisible = true;
}

//----------------------------------------------------------------------------
void vesKiwiBrainAtlasRepresentation::hideTextLabel() {
  if (this->Internal->TextVisible) {
    this->Internal->TextRep->removeSelfFromRenderer(this->renderer());
    this->Internal->TextVisible = false;
  }
}

//----------------------------------------------------------------------------
void vesKiwiBrainAtlasRepresentation::deselectModel()
{
  this->Internal->ActiveModel = -1;

  // show the models
  for (size_t i = 0; i < this->Internal->AnatomicalModels.size(); ++i) {
    if (this->Internal->ModelSceneStatus[i]) {
      this->Internal->AnatomicalModels[i]->addSelfToRenderer(this->renderer());
      this->Internal->ModelStatus[i] = true;
    }
  }

  // remove the text label
  this->hideTextLabel();
}

//----------------------------------------------------------------------------
bool vesKiwiBrainAtlasRepresentation::selectModel(int displayX, int displayY)
{

  int tappedModel = this->findTappedModel(displayX, displayY);

  if (this->Internal->ActiveModel == -1 && tappedModel == -1) {
    return false;
  }

  if (tappedModel == this->Internal->ActiveModel) {
    tappedModel = -1;
  }

  this->Internal->ActiveModel = tappedModel;

  if (tappedModel >= 0) {

    // hide all models
    for (size_t i = 0; i < this->Internal->AnatomicalModels.size(); ++i) {
      this->Internal->AnatomicalModels[i]->removeSelfFromRenderer(this->renderer());
      this->Internal->ModelStatus[i] = false;
    }

    // show the tapped model
    this->Internal->AnatomicalModels[tappedModel]->addSelfToRenderer(this->renderer());
    this->Internal->ModelStatus[tappedModel] = true;

    // show the text label
    this->showTextLabel(tappedModel);

  }
  else {
    this->deselectModel();
  }

  return true;
}


//----------------------------------------------------------------------------
bool vesKiwiBrainAtlasRepresentation::toggleSkinOpacity(int displayX, int displayY)
{
  if (!this->Internal->ModelStatus[this->Internal->SkinRepIndex]) {
    return false;
  }

  vesSharedPtr<vesRenderer> ren = this->renderer();
  displayY = ren->height() - displayY;

  int cornerSize = 150;
  if (displayX < cornerSize && displayY < cornerSize) {
    vesVector4f skinColor = this->Internal->SkinRep->color();
    if (this->Internal->SkinOpacity == 1.0) {
      this->Internal->SkinOpacity = 0.3;
      this->Internal->SkinRep->setColor(skinColor[0], skinColor[1], skinColor[2], this->Internal->SkinOpacity);
    }
    else if (this->Internal->SkinOpacity == 0.3) {
      this->Internal->SkinOpacity = 1.0;
      this->Internal->SkinRep->setColor(skinColor[0], skinColor[1], skinColor[2], this->Internal->SkinOpacity);
    }
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
bool vesKiwiBrainAtlasRepresentation::handleSingleTouchTap(int displayX, int displayY)
{
  if (this->Internal->ActiveModel == -1) {
    return this->hideModel(displayX, displayY);
  }
  else {
    this->deselectModel();
    return false;
  }
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
  for (size_t i = 0; i < this->Internal->AnatomicalModels.size(); ++i)
    if (this->Internal->ModelStatus[i])
      count += this->Internal->AnatomicalModels[i]->numberOfFacets();
  return count;
}

//----------------------------------------------------------------------------
int vesKiwiBrainAtlasRepresentation::numberOfVertices()
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->AnatomicalModels.size(); ++i)
    if (this->Internal->ModelStatus[i])
      count += this->Internal->AnatomicalModels[i]->numberOfVertices();
  return count;
}

//----------------------------------------------------------------------------
int vesKiwiBrainAtlasRepresentation::numberOfLines()
{
  int count = 0;
  for (size_t i = 0; i < this->Internal->AnatomicalModels.size(); ++i)
    if (this->Internal->ModelStatus[i])
      count += this->Internal->AnatomicalModels[i]->numberOfLines();
  return count;
}
