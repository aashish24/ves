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

#include "vesKiwiColorMapCollection.h"


#include <vesKiwiDataConversionTools.h>
#include <vtkDataArray.h>
#include <vtkScalarsToColors.h>
#include <string>
#include <map>

//----------------------------------------------------------------------------
class vesKiwiColorMapCollection::vesInternal
{
public:

  std::map<std::string, std::string> ArrayToColorMap;
  std::map<std::string, vtkSmartPointer<vtkScalarsToColors> > ColorMaps;
  vtkSmartPointer<vtkScalarsToColors> DefaultColorMap;
};

//----------------------------------------------------------------------------
vesKiwiColorMapCollection::vesKiwiColorMapCollection()
{
  this->Internal = new vesInternal;
}

//----------------------------------------------------------------------------
vesKiwiColorMapCollection::~vesKiwiColorMapCollection()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vesKiwiColorMapCollection::setColorMapForArray(const std::string& arrayName, const std::string& colorMapName)
{
  this->Internal->ArrayToColorMap[arrayName] = colorMapName;
}

//----------------------------------------------------------------------------
void vesKiwiColorMapCollection::setColorMap(const std::string& colorMapName, vtkSmartPointer<vtkScalarsToColors> colorMap)
{
  this->Internal->ColorMaps[colorMapName] = colorMap;
}

//----------------------------------------------------------------------------
void vesKiwiColorMapCollection::setDefaultColorMap(vtkSmartPointer<vtkScalarsToColors> colorMap)
{
  this->Internal->DefaultColorMap = colorMap;
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkScalarsToColors> vesKiwiColorMapCollection::colorMapForArray(vtkDataArray* scalars)
{
  if (!scalars) {
    return 0;
  }

  if (scalars->GetName()) {

    std::string colorMapName;
    std::string arrayName = scalars->GetName();

    std::map<std::string, std::string>::const_iterator itr = this->Internal->ArrayToColorMap.find(arrayName);
    if (itr != this->Internal->ArrayToColorMap.end()) {
      colorMapName = itr->second;
    }
    else {
      colorMapName = arrayName;
    }

    std::map<std::string, vtkSmartPointer<vtkScalarsToColors> >::const_iterator mapItr = this->Internal->ColorMaps.find(colorMapName);
    if (mapItr != this->Internal->ColorMaps.end()) {
      return mapItr->second;
    }

    vtkSmartPointer<vtkScalarsToColors> colorMap = vesKiwiDataConversionTools::GetPresetColorMap(colorMapName, scalars->GetRange());
    if (colorMap) {
      return colorMap;
    }
  }

  return this->Internal->DefaultColorMap;
}
