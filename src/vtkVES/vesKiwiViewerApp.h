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

#ifndef __vesKiwiViewerApp_h
#define __vesKiwiViewerApp_h


#include <string>

class vesCamera;
class vesRenderer;
class vesShaderProgram;
class vesKiwiDataRepresentation;

class vtkDataSet;

class vesKiwiViewerApp
{
public:

  vesKiwiViewerApp();
  ~vesKiwiViewerApp();

  int numberOfBuiltinDatasets() const;
  int defaultBuiltinDatasetIndex() const;
  std::string builtinDatasetName(int index);
  std::string builtinDatasetFilename(int index);

  bool loadDataset(const std::string& filename);
  std::string loadDatasetErrorTitle() const;
  std::string loadDatasetErrorMessage() const;

  void setVertexShaderSource(const std::string& source);
  void setFragmentShaderSource(const std::string& source);

  int  getNumberOfShadingModels() const;
  std::string getCurrentShadingModel() const;
  std::string getShadingModel(int index) const;
  bool setShadingModel(const std::string& name);

  bool initializeShaderProgram();
  bool initializeRendering();

  void render();
  void resetView();
  void resizeView(int width, int height);

  void handleTwoTouchPanGesture(double x0, double y0, double x1, double y1);
  void handleSingleTouchPanGesture(double deltaX, double deltaY);
  void handleTwoTouchPinchGesture(double scale);
  void handleTwoTouchRotationGesture(double rotation);

  int numberOfModelFacets() const;
  int numberOfModelVertices() const;
  int numberOfModelLines() const;

  int viewWidth() const;
  int viewHeight() const;

protected:

  // These accessors are protected so that apps cannot use the APIs of
  // these objects.  Instead, this class should provide public methods to
  // wrap the APIs.  The goal is to allow the ves APIs to be refactored
  // without breaking the Android and iOS KiwiViewer apps.
  vesCamera* camera() const;
  vesRenderer* renderer() const;
  vesShaderProgram* shaderProgram() const;

  void addBuiltinDataset(const std::string& name, const std::string& filename);
  void addBuiltinShadingModel(const std::string& name);

  void removeAllDataRepresentations();
  vesKiwiDataRepresentation* addNewDataRepresentation(vtkDataSet* dataSet);

private:

  vesKiwiViewerApp(const vesKiwiViewerApp&); // Not implemented
  void operator=(const vesKiwiViewerApp&); // Not implemented

  class vesInternal;
  vesInternal* Internal;
};


#endif