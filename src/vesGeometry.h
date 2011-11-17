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

#ifndef VESGEOMETRY_H
#define VESGEOMETRY_H

#include "vesObject.h"

// VES includes
#include "vesGeometryData.h"

class vesGeometry : public vesObject
{
public:
  typedef std::vector< vesSharedPtr<vesGeometryData> > GeometryDataset;

  vesTypeMacro(vesGeometry);

  bool addGeometryData(vesSharedPtr<vesGeometryData> geometryData)
  {
    this->m_geometryDataset.push_back(geometryData);

    return true;
  }

  const GeometryDataset& geometryDataset() const
  {
    return this->m_geometryDataset;
  }

  GeometryDataset geometryDataset()
  {
    return this->m_geometryDataset;
  }

protected:
  GeometryDataset m_geometryDataset;
};

#endif // VESGEOMETRY_H
