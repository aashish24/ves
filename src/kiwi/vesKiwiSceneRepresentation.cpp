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

#include "vesKiwiSceneRepresentation.h"

#include "vesRenderer.h"
#include "vesCamera.h"
#include "vesMapper.h"
#include "vesActor.h"
#include "vesGeometryData.h"
#include "vesSourceData.h"
#include "vesBackground.h"
#include "vesTexture.h"
#include "vesKiwiColorMapCollection.h"
#include "vesKiwiDataLoader.h"
#include "vesKiwiDataConversionTools.h"
#include "vesKiwiText2DRepresentation.h"
#include "vesKiwiPolyDataRepresentation.h"
#include "vesKiwiAnimationRepresentation.h"
#include "vesKiwiImageWidgetRepresentation.h"
#include "vesKiwiImagePlaneDataRepresentation.h"
#include "vesEigen.h"

#include "vesKiwiOptions.h"
#ifdef VES_USE_CURL
#  include "vesKiwiCurlDownloader.h"
#endif

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
#include <vtkImageData.h>
#include <vtkScalarsToColors.h>
#include <vtkLookupTable.h>
#include <vtkDiscretizableColorTransferFunction.h>

#include <vtksys/SystemTools.hxx>

#include <vector>
#include <cassert>

#include <fstream>
#include <string>

#include <cJSON.h>

typedef vesSharedPtr<cJSON> jsonSharedPtr;

namespace {

jsonSharedPtr makeShared(cJSON* json)
{
  return jsonSharedPtr(json, free);
}

void strPrint(const std::string& title, const std::string& str)
{
  printf("----%s----\n%s\n-----\n", title.c_str(), str.c_str());
}

std::string jsonToStr(cJSON* json)
{
  if (json) {
    char* cstr = cJSON_Print(json);
    std::string str = cstr;
    free(cstr);
    return str;
  }
  else {
    return "(null json object)";
  }
}

std::string jsonToStr(jsonSharedPtr json)
{
  return jsonToStr(json.get());
}

}


//----------------------------------------------------------------------------
class vesKiwiSceneRepresentation::vesInternal
{
public:

  std::vector<vesKiwiDataRepresentation::Ptr> AllReps;

  vesSharedPtr<vesShaderProgram> GeometryShader;
  vesSharedPtr<vesShaderProgram> GeometryWireframeShader;
  vesSharedPtr<vesShaderProgram> GeometrySurfaceWithEdgesShader;
  vesSharedPtr<vesShaderProgram> GeometryTextureShader;
  vesSharedPtr<vesShaderProgram> ImageTextureShader;
  vesSharedPtr<vesShaderProgram> ClipShader;

  vesKiwiDataLoader* DataLoader;

  std::string BaseDir;
  std::string ErrorTitle;
  std::string ErrorMessage;

  typedef std::map<std::string, vtkSmartPointer<vtkDiscretizableColorTransferFunction> > ColorMapsType;
  ColorMapsType ColorMaps;

  bool HasBackgroundSettings;
  vesVector3d BackgroundColor;
  vesVector3d BackgroundColor2;
  std::string BackgroundImage;

  bool HasCameraSettings;
  vesVector3d CameraPosition;
  vesVector3d CameraFocalPoint;
  vesVector3d CameraViewUp;

  vesInternal()
  {
    this->HasBackgroundSettings = false;
    this->HasCameraSettings = false;
    this->BackgroundColor = vesVector3d(0.2, 0.2, 0.2);
    this->BackgroundColor2 = vesVector3d(0, 0, 0);
  }

  ~vesInternal()
  {
  }

  void setError(const std::string& errorTitle, const std::string& errorMessage)
  {
    // preserve the existing error message if one already exists
    if (this->ErrorMessage.empty()) {
      this->ErrorTitle = errorTitle;
      this->ErrorMessage = errorMessage;
    }
  }

  std::string getStr(cJSON* json, const char* key)
  {
    cJSON* value = cJSON_GetObjectItem(json, key);
    if (value && value->type == cJSON_String) {
      return value->valuestring;
    }
    return std::string();
  }

  std::vector<std::string> getStrings(cJSON* json, const char* key)
  {
    std::vector<std::string> values;
    cJSON* jsonList = cJSON_GetObjectItem(json, key);
    int numberOfValues = jsonList ? cJSON_GetArraySize(jsonList) : 0;
    for (int i = 0; i < numberOfValues; ++i) {
      cJSON* value = cJSON_GetArrayItem(jsonList, i);
      if (value && value->type == cJSON_String) {
        values.push_back(value->valuestring);
      }
    }
    return values;
  }

  std::vector<double> getDoubles(cJSON* json, const char* key)
  {
    cJSON* jsonList = cJSON_GetObjectItem(json, key);

    int numberOfValues = jsonList ? cJSON_GetArraySize(jsonList) : 0;
    std::vector<double> values(numberOfValues);

    for (int i = 0; i < numberOfValues; ++i) {
      values[i] = cJSON_GetArrayItem(jsonList, i)->valuedouble;
    }

    return values;
  }


  // Usage: getVector<vesVector3d>(json, "color", color);
  template<typename T>
  bool getVector(cJSON* json, const char* key, T& vec)
  {
    std::vector<double> values = getDoubles(json, key);
    if (values.size() == vec.size()) {
      for (size_t i = 0; i < values.size(); ++i) {
        vec[i] = values[i];
      }
      return true;
    }
    return false;
  }

  void assignColorMaps(vesKiwiDataRepresentation::Ptr rep, cJSON* properties)
  {

    vesKiwiColorMapCollection::Ptr collection = rep->colorMapCollection();

    // set existing color maps

    ColorMapsType::const_iterator colorMapItr = this->ColorMaps.begin();
    for ( ; colorMapItr != this->ColorMaps.end(); ++colorMapItr) {
      std::string colorMapName = colorMapItr->first;
      vtkSmartPointer<vtkScalarsToColors> colorMap = colorMapItr->second;
      collection->setColorMap(colorMapName, colorMap);
    }


    // set object color map

    std::string colorBy = getStr(properties, "color_by");
    cJSON* colorMapJson = cJSON_GetObjectItem(properties, "color_map");
    if (colorMapJson && !colorBy.empty()) {

      vtkSmartPointer<vtkScalarsToColors> colorMap;
      if (colorMapJson->type == cJSON_Object) {
        collection->setColorMap(colorBy, this->createColorMap(colorMapJson));
      }
      else if (colorMapJson && colorMapJson->type == cJSON_String) {
        //printf("%s --> %s\n", colorBy.c_str(), colorMapJson->valuestring);
        collection->setColorMapForArray(colorBy, colorMapJson->valuestring);
      }
    }

  }

  vesKiwiPolyDataRepresentation::Ptr createPolyDataRepresentation(vtkPolyData* polyData, cJSON* properties)
  {
    vesKiwiPolyDataRepresentation::Ptr rep = vesKiwiPolyDataRepresentation::Ptr(new vesKiwiPolyDataRepresentation);

    this->assignColorMaps(rep, properties);

    rep->initializeWithShader(this->GeometryShader);
    rep->setWireframeShader(this->GeometryWireframeShader);
    rep->setSurfaceWithEdgesShader(this->GeometrySurfaceWithEdgesShader);
    rep->setPolyData(polyData);

    vesVector3d color(1.0 ,1.0 ,1.0);
    getVector<vesVector3d>(properties, "color", color);
    rep->setColor(color[0], color[1], color[2], 1.0);

    cJSON* opacityJson = cJSON_GetObjectItem(properties, "opacity");
    if (opacityJson && opacityJson->type == cJSON_Number) {
      rep->setOpacity(opacityJson->valuedouble);
    }

    cJSON* pointSizeJson = cJSON_GetObjectItem(properties, "point_size");
    if (pointSizeJson && pointSizeJson->type == cJSON_Number) {
      rep->setPointSize(pointSizeJson->valuedouble);
    }

    cJSON* lineWidthJson = cJSON_GetObjectItem(properties, "line_width");
    if (lineWidthJson && lineWidthJson->type == cJSON_Number) {
      rep->setLineWidth(lineWidthJson->valuedouble);
    }

    cJSON* drawOrderJson = cJSON_GetObjectItem(properties, "draw_order");
    if (drawOrderJson && drawOrderJson->type == cJSON_Number) {
      rep->setBinNumber(opacityJson->valueint);
    }

    std::string colorBy = getStr(properties, "color_by");
    std::string textureFile = getStr(properties, "texture");

    if (!textureFile.empty()) {

      if (vesKiwiDataConversionTools::FindTextureCoordinatesArray(polyData)) {

        textureFile = this->BaseDir + "/" + textureFile;
        vtkSmartPointer<vtkImageData> image = vtkImageData::SafeDownCast(this->DataLoader->loadDataset(textureFile));
        if (image) {

          vtkSmartPointer<vtkUnsignedCharArray> pixels = vtkUnsignedCharArray::SafeDownCast(image->GetPointData()->GetScalars());
          if (pixels) {
            int width = image->GetDimensions()[0];
            int height = image->GetDimensions()[1];

            vesTexture::Ptr texture = vesTexture::Ptr(new vesTexture());
            vesKiwiDataConversionTools::SetTextureData(pixels, texture, width, height);
            rep->setTexture(texture);
            rep->setTextureSurfaceShader(this->GeometryTextureShader);
            rep->colorByTexture();
          }
          else {
            this->setError("Error Loading Texture", "Could not convert image data to texture data.");
          }
        }
        else {
          this->setError("Error Loading Texture", "Could not open texture file.");
        }
      }
      else {
        this->setError("Error Loading Texture", "Could not locate texture coordinates array.");
      }
    }
    else if (!colorBy.empty()) {
      if (polyData->GetPointData()->GetArray(colorBy.c_str())) {
        rep->colorByScalars(colorBy);
      }
      else {
        this->setError("Missing Scalars Array", "Could not locate scalars array to color by.");
      }
    }
    else {
      rep->colorBySolidColor();
    }


    std::string geometryMode = getStr(properties, "geometry_mode");
    if (geometryMode == "wireframe") {
      rep->wireframeOn();
    }
    else if (geometryMode == "surface_with_edges") {
      rep->surfaceWithEdgesOn();
    }
    else if (geometryMode == "points") {
      rep->pointsOn();
    }

    return rep;
  }

  vesKiwiDataRepresentation::Ptr createImageDataRepresentation(vtkImageData* image, cJSON* properties)
  {
    if (image->GetDataDimension() == 3) {
      vesKiwiImageWidgetRepresentation::Ptr rep = vesKiwiImageWidgetRepresentation::Ptr(new vesKiwiImageWidgetRepresentation);
      this->assignColorMaps(rep, properties);
      rep->initializeWithShader(this->GeometryShader, this->ImageTextureShader);
      rep->setImageData(image);
      return rep;
    }
    else {
      vesKiwiImagePlaneDataRepresentation::Ptr rep = vesKiwiImagePlaneDataRepresentation::Ptr(new vesKiwiImagePlaneDataRepresentation);
      rep->initializeWithShader(this->ImageTextureShader);
      rep->setSurfaceShader(this->GeometryShader);
      rep->setTextureSurfaceShader(this->ImageTextureShader);
      rep->setWireframeShader(this->GeometryWireframeShader);
      rep->setSurfaceWithEdgesShader(this->GeometrySurfaceWithEdgesShader);

      rep->setImageData(image);
      return rep;
    }
  }


  vesKiwiDataRepresentation::Ptr loadDataset(const std::string& filename, cJSON* objectJson)
  {
    //std::cout << "loading: " << filename << std::endl;

    vesKiwiDataRepresentation::Ptr rep;

    vtkSmartPointer<vtkDataSet> dataSet = this->DataLoader->loadDataset(filename);
    if (dataSet) {

      vtkPolyData* polyData = vtkPolyData::SafeDownCast(dataSet);
      vtkImageData* imageData = vtkImageData::SafeDownCast(dataSet);

      if (polyData) {
        rep = this->createPolyDataRepresentation(polyData, objectJson);
      }
      else if (vtkImageData::SafeDownCast(dataSet)) {
        rep = this->createImageDataRepresentation(imageData, objectJson);
      }
      else {
        this->setError("Unhandled data type", "Loaded unhandled data type from file: " + filename);
      }
    }
    else {
      this->setError(this->DataLoader->errorTitle(), this->DataLoader->errorMessage());
    }

    return rep;
  }

  bool loadObjectSeries(cJSON* objectJson)
  {
    std::vector<vesKiwiPolyDataRepresentation::Ptr> reps;

    std::vector<std::string> filenames = getStrings(objectJson, "filenames");

    for (size_t i = 0; i < filenames.size(); ++i) {

      std::string filename = this->BaseDir + "/" + filenames[i];
      vesKiwiPolyDataRepresentation::Ptr rep =
        vesDynamicPtrCast<vesKiwiPolyDataRepresentation>(this->loadDataset(filename, objectJson));

      if (!rep) {
        return false;
      }

      reps.push_back(rep);
    }

    if (!reps.size()) {
      return false;
    }

    vesKiwiAnimationRepresentation::Ptr series(new vesKiwiAnimationRepresentation);
    series->setRepresentations(reps);

    cJSON* framesPerSecondJson = cJSON_GetObjectItem(objectJson, "frames_per_second");
    if (framesPerSecondJson && framesPerSecondJson->type == cJSON_Number) {
      series->setFramesPerSecond(framesPerSecondJson->valueint);
    }


    this->AllReps.push_back(series);
    return true;
  }


  bool loadObject(cJSON* objectJson)
  {
    //strPrint("load object", jsonToStr(objectJson));

    if (!objectJson) {
      return false;
    }


    cJSON* filenamesJson = cJSON_GetObjectItem(objectJson, "filenames");
    if (filenamesJson) {
      return this->loadObjectSeries(objectJson);
    }

    std::string url = getStr(objectJson, "url");
    std::string filename = getStr(objectJson, "filename");

    if (filename.empty()) {
      this->setError("Missing Filename", "The object filename is missing.");
      return false;
    }

    filename = this->BaseDir + "/" + filename;

    if (!url.empty() && !vtksys::SystemTools::FileExists(filename.c_str(), true)) {
#ifdef VES_USE_CURL
      vesKiwiCurlDownloader downloader;
      if (!downloader.downloadUrlToFile(url, filename)) {
        this->setError(downloader.errorTitle(), downloader.errorMessage());
        return false;
      }
#else
      return false;
#endif // VES_USE_CURL
    }

    vesKiwiDataRepresentation::Ptr rep = this->loadDataset(filename, objectJson);
    if (!rep) {
      return false;
    }

    this->AllReps.push_back(rep);
    return true;
  }

  bool loadObjects(cJSON* objectsJson)
  {
    if (!objectsJson) {
      return false;
    }

    const int numberOfObjects = cJSON_GetArraySize(objectsJson);
    for (int i = 0; i < numberOfObjects; ++i) {
      cJSON* objectJson = cJSON_GetArrayItem(objectsJson, i);

      this->loadObject(objectJson);
    }

    return this->ErrorMessage.empty();
  }

  vtkSmartPointer<vtkDiscretizableColorTransferFunction> createColorMap(cJSON* mapJson)
  {
    if (!mapJson) {
      return 0;
    }

    std::string colorSpace = getStr(mapJson, "color_space");
    std::vector<double> rgbPoints = getDoubles(mapJson, "rgb_points");

    if (rgbPoints.empty() || rgbPoints.size() % 4 != 0) {
      return 0;
    }

    vtkSmartPointer<vtkDiscretizableColorTransferFunction> colorMap = vtkSmartPointer<vtkDiscretizableColorTransferFunction>::New();

    if (colorSpace == "RGB") {
      colorMap->SetColorSpaceToRGB();
    }
    else if (colorSpace == "HSV") {
      colorMap->SetColorSpaceToHSV();
      colorMap->HSVWrapOff();
    }
    else if (colorSpace == "WrappedHSV") {
      colorMap->SetColorSpaceToHSV();
      colorMap->HSVWrapOn();
    }
    else if (colorSpace == "Lab") {
      colorMap->SetColorSpaceToLab();
    }
    else if (colorSpace == "Diverging") {
      colorMap->SetColorSpaceToDiverging();
    }

    for (size_t i = 0; i < rgbPoints.size()/4; ++i) {
      colorMap->AddRGBPoint(rgbPoints[i*4 + 0],
                            rgbPoints[i*4 + 1],
                            rgbPoints[i*4 + 2],
                            rgbPoints[i*4 + 3]);
    }

    cJSON* numberOfValuesJson = cJSON_GetObjectItem(mapJson, "number_of_values");
    if (numberOfValuesJson) {
      colorMap->DiscretizeOn();
      colorMap->SetNumberOfValues(numberOfValuesJson->valueint);
    }

    return colorMap;
  }

  bool loadColorMaps(cJSON* mapsJson)
  {
    if (!mapsJson) {
      return false;
    }

    const int numberOfMaps = cJSON_GetArraySize(mapsJson);

    for (int i = 0; i < numberOfMaps; ++i) {
      cJSON* mapJson = cJSON_GetArrayItem(mapsJson, i);
      std::string colorMapName = mapJson->string;

      vtkSmartPointer<vtkDiscretizableColorTransferFunction> colorMap = this->createColorMap(mapJson);
      if (colorMap) {
        this->ColorMaps[colorMapName] = colorMap;
      }
    }

    return this->ErrorMessage.empty();
  }


};

//----------------------------------------------------------------------------
vesKiwiSceneRepresentation::vesKiwiSceneRepresentation()
{
  this->Internal = new vesInternal();
}

//----------------------------------------------------------------------------
vesKiwiSceneRepresentation::~vesKiwiSceneRepresentation()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vesKiwiSceneRepresentation::setShaders(vesSharedPtr<vesShaderProgram> geometryShader,
                                                      vesSharedPtr<vesShaderProgram> geometryWireframeShader,
                                                      vesSharedPtr<vesShaderProgram> geometrySurfaceWithEdgesShader,
                                                      vesSharedPtr<vesShaderProgram> geometryTextureShader,
                                                      vesSharedPtr<vesShaderProgram> imageTextureShader,
                                                      vesSharedPtr<vesShaderProgram> clipShader)
{
  this->Internal->GeometryShader = geometryShader;
  this->Internal->GeometryWireframeShader = geometryWireframeShader;
  this->Internal->GeometrySurfaceWithEdgesShader = geometrySurfaceWithEdgesShader;
  this->Internal->GeometryTextureShader = geometryTextureShader;
  this->Internal->ImageTextureShader = imageTextureShader;
  this->Internal->ClipShader = clipShader;
}

//----------------------------------------------------------------------------
bool vesKiwiSceneRepresentation::loadScene(const std::string& filename, vesKiwiDataLoader* dataLoader)
{
  this->Internal->BaseDir = vtksys::SystemTools::GetFilenamePath(filename);
  this->Internal->DataLoader = dataLoader;

  // read contents of file
  std::ifstream ifs(filename.c_str());
  std::string jsonContent((std::istreambuf_iterator<char>(ifs)),
                          (std::istreambuf_iterator<char>()));


  jsonSharedPtr sceneJson = makeShared(cJSON_Parse(jsonContent.c_str()));

  if (!sceneJson) {
    this->setError("Parse Error", "Error parsing JSON file contents.");

    //
    //std::string errorStr = std::string(cJSON_GetErrorPtr()-5, 10);
    //printf("parse error: %s\n", errorStr.c_str());

    return false;
  }

  //strPrint("json data", jsonToStr(sceneJson));

  // load color maps
  cJSON* colorMapsJson = cJSON_GetObjectItem(sceneJson.get(), "color_maps");
  this->Internal->loadColorMaps(colorMapsJson);

  // load objects
  cJSON* objectsJson = cJSON_GetObjectItem(sceneJson.get(), "objects");
  this->Internal->loadObjects(objectsJson);


  // parse background settings
  this->Internal->HasBackgroundSettings = false;
  if (this->Internal->getVector<vesVector3d>(sceneJson.get(), "background_color", this->Internal->BackgroundColor)) {
    this->Internal->HasBackgroundSettings = true;
    if (!this->Internal->getVector<vesVector3d>(sceneJson.get(), "background_color2", this->Internal->BackgroundColor2)) {
      this->Internal->BackgroundColor2 = this->Internal->BackgroundColor;
    }
  }

  this->Internal->BackgroundImage = this->Internal->getStr(sceneJson.get(), "background_image");
  if (!this->Internal->BackgroundImage.empty()) {
    this->Internal->HasBackgroundSettings = true;
  }


  // parse camera settings
  this->Internal->HasCameraSettings = false;
  cJSON* cameraJson = cJSON_GetObjectItem(sceneJson.get(), "camera");
  if (cameraJson) {
    this->Internal->HasCameraSettings =
      (    this->Internal->getVector<vesVector3d>(cameraJson, "position", this->Internal->CameraPosition)
        && this->Internal->getVector<vesVector3d>(cameraJson, "focal_point", this->Internal->CameraFocalPoint)
        && this->Internal->getVector<vesVector3d>(cameraJson, "view_up", this->Internal->CameraViewUp));
  }



  return this->Internal->ErrorMessage.empty();
}

//----------------------------------------------------------------------------
const std::vector<vesKiwiDataRepresentation::Ptr> vesKiwiSceneRepresentation::dataRepresentations() const
{
  return this->Internal->AllReps;
}

//-----------------------------------------------------------------------------
std::string vesKiwiSceneRepresentation::errorTitle() const
{
  return this->Internal->ErrorTitle;
}

//-----------------------------------------------------------------------------
std::string vesKiwiSceneRepresentation::errorMessage() const
{
  return this->Internal->ErrorMessage;
}

//-----------------------------------------------------------------------------
void vesKiwiSceneRepresentation::resetErrorMessages()
{
  this->Internal->ErrorTitle = std::string();
  this->Internal->ErrorMessage = std::string();
}

//-----------------------------------------------------------------------------
void vesKiwiSceneRepresentation::setError(const std::string& errorTitle, const std::string& errorMessage)
{
  this->Internal->setError(errorTitle, errorMessage);
}

//-----------------------------------------------------------------------------
vesVector3f vesKiwiSceneRepresentation::backgroundColor() const
{
  return this->Internal->BackgroundColor.cast<float>();
}

//-----------------------------------------------------------------------------
vesVector3f vesKiwiSceneRepresentation::backgroundColor2() const
{
  return this->Internal->BackgroundColor2.cast<float>();
}

//-----------------------------------------------------------------------------
std::string vesKiwiSceneRepresentation::backgroundImage() const
{
  if (!this->Internal->BackgroundImage.empty()) {
    return this->Internal->BaseDir + "/" + this->Internal->BackgroundImage;
  }
  return std::string();
}

//-----------------------------------------------------------------------------
bool vesKiwiSceneRepresentation::hasCameraSettings() const
{
  return this->Internal->HasCameraSettings;
}

//-----------------------------------------------------------------------------
bool vesKiwiSceneRepresentation::hasBackgroundSettings() const
{
  return this->Internal->HasBackgroundSettings;
}

//-----------------------------------------------------------------------------
vesVector3f vesKiwiSceneRepresentation::cameraPosition() const
{
  return this->Internal->CameraPosition.cast<float>();
}

//-----------------------------------------------------------------------------
vesVector3f vesKiwiSceneRepresentation::cameraFocalPoint() const
{
  return this->Internal->CameraFocalPoint.cast<float>();
}

//-----------------------------------------------------------------------------
vesVector3f vesKiwiSceneRepresentation::cameraViewUp() const
{
  return this->Internal->CameraViewUp.cast<float>();
}
