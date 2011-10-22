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

#include "vesKiwiBaseApp.h"
#include "vesCamera.h"
#include "vesGMTL.h"
#include "vesRenderer.h"
#include "vesUniform.h"
#include "vesVertexAttribute.h"
#include "vesShader.h"
#include "vesShaderProgram.h"
#include "vesModelViewUniform.h"
#include "vesNormalMatrixUniform.h"
#include "vesProjectionUniform.h"
#include "vesVertexAttributeKeys.h"

#include <cassert>
#include <cmath>
#include <vector>

//----------------------------------------------------------------------------
class vesKiwiBaseApp::vesInternal
{
public:

  vesInternal()
  {
    this->Renderer = 0;
  }

  ~vesInternal()
  {
    #define _delete_items(ivar) for (size_t i = 0; i < ivar.size(); ++i) delete ivar[i];
    _delete_items(this->VertexAttributes);
    _delete_items(this->Uniforms);
    //_delete_items(this->Shaders);
    _delete_items(this->ShaderPrograms);

    delete this->Renderer;
  }

  vesRenderer* Renderer;

  std::vector<vesShaderProgram*> ShaderPrograms;
  std::vector<vesShader*> Shaders;
  std::vector<vesUniform*> Uniforms;
  std::vector<vesVertexAttribute*> VertexAttributes;
};

//----------------------------------------------------------------------------
vesKiwiBaseApp::vesKiwiBaseApp()
{
  this->Internal = new vesInternal();
  this->Internal->Renderer = new vesRenderer();
}

//----------------------------------------------------------------------------
vesKiwiBaseApp::~vesKiwiBaseApp()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
vesCamera* vesKiwiBaseApp::camera() const
{
  assert(this->Internal->Renderer);
  return this->Internal->Renderer->camera();
}

//----------------------------------------------------------------------------
vesRenderer* vesKiwiBaseApp::renderer() const
{
  return this->Internal->Renderer;
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::render()
{
  this->willRender();
  this->Internal->Renderer->resetCameraClippingRange();
  this->Internal->Renderer->render();
  this->didRender();
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::resizeView(int width, int height)
{
  this->Internal->Renderer->resize(width, height, 1.0f);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::resetView()
{
  // this is just confusing...
  // We want to set the direction to look from and view up
  // then we want to dolly the camera so that the surface takes up
  // a reasonable portion of the screen.
  // the ResetCamera function will overwrite view up
  // so we have to call things in this strange order.

  //
  // set direction to look from
  vesRenderer* renderer = this->Internal->Renderer;

  renderer->camera()->SetViewPlaneNormal(vesVector3f(0.0, 0.0, 1.0));

  // dolly so that scene fits window
  renderer->resetCamera();

  // The current ResetCamera() method pulls the camera back further than
  // required.  ResetCamera should be fixed.  Until then, perform a dolly
  // with a scale factor of 1.5 (a magic number).
  renderer->camera()->Dolly(1.5);

  // now set the view plane normal
  renderer->camera()->SetViewUp(vesVector3f(0.0, 1.0, 0.0));
  renderer->camera()->OrthogonalizeViewUp();
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleTwoTouchPanGesture(double x0, double y0, double x1, double y1)
{
  // calculate the focal depth so we'll know how far to move
  vesRenderer* ren = this->Internal->Renderer;
  vesCamera* camera = ren->camera();
  vesVector3f viewFocus = camera->GetFocalPoint();
  vesVector3f viewPoint = camera->GetPosition();
  vesVector3f viewFocusDisplay = ren->computeWorldToDisplay(viewFocus);
  float focalDepth = viewFocusDisplay[2];

  // map change into world coordinates
  vesVector3f oldPickPoint = ren->computeDisplayToWorld(vesVector3f(x0, y0, focalDepth));
  vesVector3f newPickPoint = ren->computeDisplayToWorld(vesVector3f(x1, y1, focalDepth));
  vesVector3f motionVector = oldPickPoint - newPickPoint;

  vesVector3f newViewFocus = viewFocus + motionVector;
  vesVector3f newViewPoint = viewPoint + motionVector;
  camera->SetFocalPoint(newViewFocus);
  camera->SetPosition(newViewPoint);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleSingleTouchPanGesture(double deltaX, double deltaY)
{
  //
  // Rotate camera
  // Based on vtkInteractionStyleTrackballCamera::Rotate().
  //
  vesRenderer* ren = this->Internal->Renderer;
  vesCamera *camera = ren->camera();

  double delta_elevation = -20.0 / ren->height();
  double delta_azimuth   = -20.0 / ren->width();

  double motionFactor = 10.0;

  double rxf = deltaX * delta_azimuth * motionFactor;
  double ryf = deltaY * delta_elevation * motionFactor;

  camera->Azimuth(rxf);
  camera->Elevation(ryf);
  camera->OrthogonalizeViewUp();
}


//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleDoubleTap()
{
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleSingleTouchUp()
{
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleSingleTouchDown(int displayX, int displayY)
{
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleTwoTouchPinchGesture(double scale)
{
  this->Internal->Renderer->camera()->Dolly(scale);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleTwoTouchRotationGesture(double rotation)
{
  vesCamera* camera = this->Internal->Renderer->camera();
  camera->Roll(rotation * 180.0 / M_PI);
  camera->OrthogonalizeViewUp();
}

//----------------------------------------------------------------------------
vesShaderProgram* vesKiwiBaseApp::addShaderProgram(const std::string& vertexShaderSource,
                                                   const std::string& fragmentShaderSource)
{
  vesShaderProgram* shaderProgram = new vesShaderProgram();
  vesShader* vertexShader = new vesShader(vesShader::Vertex, vertexShaderSource);
  vesShader* fragmentShader = new vesShader(vesShader::Fragment, fragmentShaderSource);

  shaderProgram->addShader(vertexShader);
  shaderProgram->addShader(fragmentShader);

  this->Internal->ShaderPrograms.push_back(shaderProgram);
  this->Internal->Shaders.push_back(vertexShader);
  this->Internal->Shaders.push_back(fragmentShader);
  return shaderProgram;
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::deleteShaderProgram(vesShaderProgram *shaderProgram)
{
  if (!shaderProgram) {
    // Nothing to remove.
    return;
  }

  // Remove only one.
  std::vector<vesShaderProgram*>::iterator itr =
    std::find(this->Internal->ShaderPrograms.begin(),
              this->Internal->ShaderPrograms.end(), shaderProgram);

  if (itr != this->Internal->ShaderPrograms.end()) {
    this->Internal->ShaderPrograms.erase(itr);
    delete shaderProgram;
  }
}

//----------------------------------------------------------------------------
vesUniform* vesKiwiBaseApp::addModelViewMatrixUniform(vesShaderProgram* program, const std::string& name)
{
  this->Internal->Uniforms.push_back(
    name.empty() ? new vesModelViewUniform() : new vesModelViewUniform(name));
  program->addUniform(this->Internal->Uniforms.back());
}

//----------------------------------------------------------------------------
vesUniform* vesKiwiBaseApp::addProjectionMatrixUniform(vesShaderProgram* program, const std::string& name)
{
  this->Internal->Uniforms.push_back(
    name.empty() ? new vesProjectionUniform() : new vesProjectionUniform(name));
  program->addUniform(this->Internal->Uniforms.back());
}

//----------------------------------------------------------------------------
vesUniform* vesKiwiBaseApp::addNormalMatrixUniform(vesShaderProgram* program, const std::string& name)
{
  this->Internal->Uniforms.push_back(
    name.empty() ? new vesNormalMatrixUniform() : new vesNormalMatrixUniform(name));
  program->addUniform(this->Internal->Uniforms.back());
}

//----------------------------------------------------------------------------
vesVertexAttribute* vesKiwiBaseApp::addVertexPositionAttribute(vesShaderProgram* program, const std::string& name)
{
  this->Internal->VertexAttributes.push_back(
    name.empty() ? new vesPositionVertexAttribute() : new vesPositionVertexAttribute(name));
  program->addVertexAttribute(this->Internal->VertexAttributes.back(), vesVertexAttributeKeys::Position);
}

//----------------------------------------------------------------------------
vesVertexAttribute* vesKiwiBaseApp::addVertexNormalAttribute(vesShaderProgram* program, const std::string& name)
{
  this->Internal->VertexAttributes.push_back(
    name.empty() ? new vesNormalVertexAttribute(): new vesNormalVertexAttribute(name));
  program->addVertexAttribute(this->Internal->VertexAttributes.back(), vesVertexAttributeKeys::Normal);
}

//----------------------------------------------------------------------------
vesVertexAttribute* vesKiwiBaseApp::addVertexColorAttribute(vesShaderProgram* program, const std::string& name)
{
  this->Internal->VertexAttributes.push_back(
    name.empty() ? new vesColorVertexAttribute() : new vesColorVertexAttribute(name));
  program->addVertexAttribute(this->Internal->VertexAttributes.back(), vesVertexAttributeKeys::Color);
}

//----------------------------------------------------------------------------
vesVertexAttribute* vesKiwiBaseApp::addVertexTextureCoordinateAttribute(vesShaderProgram* program, const std::string& name)
{
  this->Internal->VertexAttributes.push_back(
    name.empty() ? new vesTextureCoordinateVertexAttribute(): new vesTextureCoordinateVertexAttribute(name));
  program->addVertexAttribute(this->Internal->VertexAttributes.back(), vesVertexAttributeKeys::TextureCoordinate);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::storeUniformForDeletion(vesUniform* uniform)
{
  this->Internal->Uniforms.push_back(uniform);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::storeVertexAttributeForDeletion(vesVertexAttribute* attribute)
{
  this->Internal->VertexAttributes.push_back(attribute);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::setBackgroundColor(double r, double g, double b)
{
  this->Internal->Renderer->setBackgroundColor(r, g, b, 1.0);
}

//----------------------------------------------------------------------------
int vesKiwiBaseApp::viewWidth() const
{
  return this->Internal->Renderer->width();
}

//----------------------------------------------------------------------------
int vesKiwiBaseApp::viewHeight() const
{
  return this->Internal->Renderer->height();
}
