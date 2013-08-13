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
/// \class vesKiwiSceneRepresentation
/// \ingroup KiwiPlatform
#ifndef __vesKiwiSceneRepresentation_h
#define __vesKiwiSceneRepresentation_h

#include "vesKiwiWidgetRepresentation.h"

class vesKiwiDataLoader;
class vesShaderProgram;
class vtkPlane;

class vesKiwiSceneRepresentation
{
public:

  vesTypeMacro(vesKiwiSceneRepresentation);

  vesKiwiSceneRepresentation();
  ~vesKiwiSceneRepresentation();

  void setShaders(vesSharedPtr<vesShaderProgram> geometryShader,
                            vesSharedPtr<vesShaderProgram> geometryWireframeShader,
                            vesSharedPtr<vesShaderProgram> geometrySurfaceWithEdgesShader,
                            vesSharedPtr<vesShaderProgram> geometryTextureShader,
                            vesSharedPtr<vesShaderProgram> imageTextureShader,
                            vesSharedPtr<vesShaderProgram> clipShader);

  bool loadScene(const std::string& filename, vesKiwiDataLoader* dataLoader);

  const std::vector<vesSharedPtr<vesKiwiDataRepresentation> > dataRepresentations() const;

  bool hasBackgroundSettings() const;
  vesVector3f backgroundColor() const;
  vesVector3f backgroundColor2() const;
  std::string backgroundImage() const;

  bool hasCameraSettings() const;
  vesVector3f cameraPosition() const;
  vesVector3f cameraFocalPoint() const;
  vesVector3f cameraViewUp() const;

  void loadData(const std::string& filename);

  void setClipPlane(vtkPlane* plane);

  std::string errorTitle() const;
  std::string errorMessage() const;

protected:

  void resetErrorMessages();
  void setError(const std::string& errorTitle, const std::string& errorMessage);

private:

  vesKiwiSceneRepresentation(const vesKiwiSceneRepresentation&); // Not implemented
  void operator=(const vesKiwiSceneRepresentation&); // Not implemented

  class vesInternal;
  vesInternal* Internal;
};

#endif
