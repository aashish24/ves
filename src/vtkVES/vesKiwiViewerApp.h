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

class vesActor;
class vesMapper;
class vesRenderer;
class vesCamera;

class vesKiwiViewerApp
{
public:


  vesKiwiViewerApp();
  ~vesKiwiViewerApp();


  int numberOfBuiltinDatasets() const;
  int defaultBuiltinDatasetIndex() const;
  std::string builtinDatasetName(int index);
  std::string builtinDatasetFilename(int index);

  void setVertexShaderSource(const std::string& source);
  void setFragmentShaderSource(const std::string& source);
  bool initializeShaderProgram();
  bool initializeRendering();

  void render();
  void resetView();
  void resizeView(int width, int height);

  vesActor* actor() const;
  vesMapper* mapper() const;
  vesCamera* camera() const;
  vesRenderer* renderer() const;

protected:

  void addBuiltinDataset(const std::string& name, const std::string& filename);

private:

  vesKiwiViewerApp(const vesKiwiViewerApp&); // Not implemented
  void operator=(const vesKiwiViewerApp&); // Not implemented

  class vesInternal;
  vesInternal* Internal;
};


#endif
