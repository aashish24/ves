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

#ifndef __vesKiwiBaseApp_h
#define __vesKiwiBaseApp_h

#include <vesGMTL.h>
#include <vesSharedPtr.h>

// C/C++
#include <string>
#include<tr1/memory>

class vesCamera;
class vesRenderer;
class vesShader;
class vesShaderProgram;
class vesUniform;
class vesVertexAttribute;

class vesKiwiBaseApp
{
public:

  vesKiwiBaseApp();
  virtual ~vesKiwiBaseApp();

  void render();

  virtual void resetView();
  virtual void resizeView(int width, int height);

  virtual void handleTwoTouchPanGesture(double x0, double y0, double x1, double y1);
  virtual void handleSingleTouchPanGesture(double deltaX, double deltaY);
  virtual void handleTwoTouchPinchGesture(double scale);
  virtual void handleTwoTouchRotationGesture(double rotation);
  virtual void handleSingleTouchDown(int displayX, int displayY);
  virtual void handleSingleTouchUp();
  virtual void handleDoubleTap();

  void setBackgroundColor(double r, double g, double b);

  int viewWidth() const;
  int viewHeight() const;

  vesVector3f cameraPosition() const;
  vesVector3f cameraFocalPoint() const;
  vesVector3f cameraViewUp() const;

  void setCameraPosition(const vesVector3f& position);
  void setCameraFocalPoint(const vesVector3f& focalPoint);
  void setCameraViewUp(const vesVector3f& viewUp);

protected:

  // Subclasses may override these methods to perform actions before and after
  // rendering.
  virtual void willRender() {}
  virtual void didRender() {}

  vesSharedPtr<vesShaderProgram> addShaderProgram(
    const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
  void deleteShaderProgram(vesSharedPtr<vesShaderProgram> shaderProgram);


  vesSharedPtr<vesUniform> addModelViewMatrixUniform(
    vesSharedPtr<vesShaderProgram> program, const std::string& name=std::string());
  vesSharedPtr<vesUniform> addProjectionMatrixUniform(
    vesSharedPtr<vesShaderProgram> program, const std::string& name=std::string());
  vesSharedPtr<vesUniform> addNormalMatrixUniform(
    vesSharedPtr<vesShaderProgram> program, const std::string& name=std::string());

  vesSharedPtr<vesVertexAttribute> addVertexPositionAttribute(
    vesSharedPtr<vesShaderProgram> program, const std::string& name=std::string());
  vesSharedPtr<vesVertexAttribute> addVertexNormalAttribute(
    vesSharedPtr<vesShaderProgram> program, const std::string& name=std::string());
  vesSharedPtr<vesVertexAttribute> addVertexColorAttribute(
    vesSharedPtr<vesShaderProgram> program, const std::string& name=std::string());
  vesSharedPtr<vesVertexAttribute> addVertexTextureCoordinateAttribute(
    vesSharedPtr<vesShaderProgram> program, const std::string& name=std::string());

  // These accessors are protected so that apps cannot use the APIs of
  // these objects.  Instead, this class should provide public methods to
  // wrap the APIs.  The goal is to allow the ves APIs to be refactored
  // without breaking the Android and iOS KiwiViewer apps.
  vesSharedPtr<vesCamera> camera() const;
  vesSharedPtr<vesRenderer> renderer() const;

private:

  vesKiwiBaseApp(const vesKiwiBaseApp&); // Not implemented
  void operator=(const vesKiwiBaseApp&); // Not implemented

  class vesInternal;
  vesInternal* Internal;
};


#endif
