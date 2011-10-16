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

#ifndef __vesKiwiPolyDataRepresentation_h
#define __vesKiwiPolyDataRepresentation_h

#include "vesKiwiDataRepresentation.h"

class vesTriangleData;
class vesActor;
class vesMapper;
class vesRenderer;
class vesShaderProgram;
class vesTexture;

class vtkPolyData;

class vesKiwiPolyDataRepresentation : public vesKiwiDataRepresentation
{
public:

  vesKiwiPolyDataRepresentation();
  ~vesKiwiPolyDataRepresentation();

  void initializeWithShader(vesShaderProgram* shaderProgram);

  void setPolyData(vtkPolyData* polyData);
  vesTriangleData* triangleData() const;

  virtual void addSelfToRenderer(vesRenderer* renderer);
  virtual void removeSelfFromRenderer(vesRenderer* renderer);

  void setColor(double r, double g, double b, double a);

  void setTexture(vesTexture* texture);
  vesTexture* texture() const;

  void setBinNumber(int binNumber);

  virtual void setTranslation(const vesVector3f& translation);

  virtual int numberOfFacets();
  virtual int numberOfVertices();
  virtual int numberOfLines();

  // todo:
  // move these back to protected.  This class should provide public api so that
  // the vesActor and vesMapper can remain implementation details that callers do
  // not have to access.
  vesMapper* mapper() const;
  vesActor* actor() const;

protected:


private:

  vesKiwiPolyDataRepresentation(const vesKiwiPolyDataRepresentation&); // Not implemented
  void operator=(const vesKiwiPolyDataRepresentation&); // Not implemented

  class vesInternal;
  vesInternal* Internal;
};


#endif
