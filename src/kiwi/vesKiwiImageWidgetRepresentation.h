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
/// \class vesKiwiImageWidgetRepresentation
/// \ingroup KiwiPlatform
#ifndef __vesKiwiImageWidgetRepresentation_h
#define __vesKiwiImageWidgetRepresentation_h

#include "vesKiwiWidgetRepresentation.h"

class vesShaderProgram;
class vesKiwiPolyDataRepresentation;
class vesKiwiImagePlaneDataRepresentation;

class vtkImageData;
class vtkPolyData;

class vesKiwiImageWidgetRepresentation : public vesKiwiWidgetRepresentation
{
public:

  typedef vesKiwiWidgetRepresentation Superclass;
  vesKiwiImageWidgetRepresentation();
  ~vesKiwiImageWidgetRepresentation();

  void setImageData(vtkImageData* imageData);
  void initializeWithShader(vesSharedPtr<vesShaderProgram> geometryShader,
                            vesSharedPtr<vesShaderProgram> textureShader);

  virtual void addSelfToRenderer(vesSharedPtr<vesRenderer> renderer);
  virtual void removeSelfFromRenderer(vesSharedPtr<vesRenderer> renderer);

  virtual int numberOfFacets();
  virtual int numberOfVertices();
  virtual int numberOfLines();

  bool handleSingleTouchDown(int displayX, int displayY);
  bool handleSingleTouchPanGesture(double deltaX, double deltaY);
  bool handleSingleTouchUp();
  bool handleDoubleTap(int displayX, int displayY);

  virtual void willRender(vesSharedPtr<vesRenderer> renderer);

  virtual bool interactionIsActive() const { return this->scrollSliceModeActive(); }

  bool scrollSliceModeActive() const;

  virtual void setShaderProgram(vesSharedPtr<vesShaderProgram> shaderProgram);
  virtual vesSharedPtr<vesShaderProgram> shaderProgram() const;

protected:

  void scrollImageSlice(double deltaX, double deltaY);

  // note- performs clamping on sliceIndex
  void setSliceIndex(int planeIndex, int sliceIndex);

  vesKiwiImagePlaneDataRepresentation* newSliceRepresentation(vtkImageData* sliceImage);
  vesKiwiPolyDataRepresentation* newGeometryRepresentation(vtkPolyData* polyData);

private:

  vesKiwiImageWidgetRepresentation(const vesKiwiImageWidgetRepresentation&); // Not implemented
  void operator=(const vesKiwiImageWidgetRepresentation&); // Not implemented

  class vesInternal;
  vesInternal* Internal;
};

#endif
