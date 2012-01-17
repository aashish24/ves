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
/// \class vesKiwiPolyDataRepresentation
/// \ingroup KiwiPlatform
#ifndef __vesKiwiPolyDataRepresentation_h
#define __vesKiwiPolyDataRepresentation_h

#include "vesKiwiDataRepresentation.h"

// VES includes
#include <vesSharedPtr.h>

class vesGeometryData;
class vesActor;
class vesMapper;
class vesRenderer;
class vesShaderProgram;
class vesTexture;

class vtkPolyData;
class vtkScalarsToColors;
class vtkDataArray;

class vesKiwiPolyDataRepresentation : public vesKiwiDataRepresentation
{
public:

  vesTypeMacro(vesKiwiPolyDataRepresentation);

  vesKiwiPolyDataRepresentation();
  ~vesKiwiPolyDataRepresentation();

  void initializeWithShader(vesSharedPtr<vesShaderProgram> shaderProgram);

  void setPolyData(vtkPolyData* polyData, vtkScalarsToColors* scalarsToColors=NULL);

  void addTextureCoordinates(vtkDataArray* textureCoordinates);

  vesSharedPtr<vesGeometryData> geometryData() const;

  virtual void addSelfToRenderer(vesSharedPtr<vesRenderer> renderer);
  virtual void removeSelfFromRenderer(vesSharedPtr<vesRenderer>);

  void setColor(double r, double g, double b, double a);
  vesVector4f color();

  void setTexture(vesSharedPtr<vesTexture> texture);
  vesSharedPtr<vesTexture> texture() const;

  void setBinNumber(int binNumber);

  virtual void setTranslation(const vesVector3f& translation);

  virtual void setShaderProgram(vesSharedPtr<vesShaderProgram> shaderProgram);
  virtual vesSharedPtr<vesShaderProgram> shaderProgram() const;

  virtual int numberOfFacets();
  virtual int numberOfVertices();
  virtual int numberOfLines();

  // todo:
  // move these back to protected.  This class should provide public api so that
  // the vesActor and vesMapper can remain implementation details that callers do
  // not have to access.
  vesSharedPtr<vesMapper> mapper() const;
  vesSharedPtr<vesActor> actor() const;

protected:


private:

  vesKiwiPolyDataRepresentation(const vesKiwiPolyDataRepresentation&); // Not implemented
  void operator=(const vesKiwiPolyDataRepresentation&); // Not implemented

  class vesInternal;
  vesInternal* Internal;
};


#endif
