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

#include <vesKiwiBaseApp.h>
#include <vesCamera.h>
#include <vesRenderer.h>
#include <vesSetGet.h>
#include <vesUniform.h>
#include <vesVertexAttribute.h>
#include <vesShader.h>
#include <vesShaderProgram.h>
#include <vesModelViewUniform.h>
#include <vesNormalMatrixUniform.h>
#include <vesProjectionUniform.h>
#include <vesVertexAttributeKeys.h>

#include <cassert>
#include <cmath>
#include <vector>

//----------------------------------------------------------------------------
class vesKiwiBaseApp::vesInternal
{
public:

  vesInternal()
  {
  }

  ~vesInternal()
  {
  }

  vesSharedPtr<vesRenderer> Renderer;

  std::vector< vesSharedPtr<vesShaderProgram> > ShaderPrograms;
  std::vector< vesSharedPtr<vesShader> > Shaders;
  std::vector< vesSharedPtr<vesUniform> > Uniforms;
  std::vector< vesSharedPtr<vesVertexAttribute> > VertexAttributes;
};

//----------------------------------------------------------------------------
vesKiwiBaseApp::vesKiwiBaseApp()
{
  this->Internal = new vesInternal();
  this->Internal->Renderer = vesSharedPtr<vesRenderer>(new vesRenderer());
}

//----------------------------------------------------------------------------
vesKiwiBaseApp::~vesKiwiBaseApp()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
std::tr1::shared_ptr<vesCamera> vesKiwiBaseApp::camera() const
{
  assert(this->Internal->Renderer);
  return this->Internal->Renderer->camera();
}

//----------------------------------------------------------------------------
vesSharedPtr<vesRenderer> vesKiwiBaseApp::renderer() const
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
  vesSharedPtr<vesRenderer> renderer = this->Internal->Renderer;

  renderer->camera()->setViewPlaneNormal(vesVector3f(0.0, 0.0, 1.0));

  // dolly so that scene fits window
  renderer->resetCamera();

  // The current ResetCamera() method pulls the camera back further than
  // required.  ResetCamera should be fixed.  Until then, perform a dolly
  // with a scale factor of 1.5 (a magic number).
  renderer->camera()->dolly(1.5);

  // now set the view plane normal
  renderer->camera()->setViewUp(vesVector3f(0.0, 1.0, 0.0));
  renderer->camera()->orthogonalizeViewUp();
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleTwoTouchPanGesture(double x0, double y0, double x1, double y1)
{
  // calculate the focal depth so we'll know how far to move
  vesSharedPtr<vesRenderer> ren = this->Internal->Renderer;
  std::tr1::shared_ptr<vesCamera> camera = ren->camera();
  vesVector3f viewFocus = camera->focalPoint();
  vesVector3f viewPoint = camera->position();
  vesVector3f viewFocusDisplay = ren->computeWorldToDisplay(viewFocus);
  float focalDepth = viewFocusDisplay[2];

  // map change into world coordinates
  vesVector3f oldPickPoint = ren->computeDisplayToWorld(vesVector3f(x0, y0, focalDepth));
  vesVector3f newPickPoint = ren->computeDisplayToWorld(vesVector3f(x1, y1, focalDepth));
  vesVector3f motionVector = oldPickPoint - newPickPoint;

  vesVector3f newViewFocus = viewFocus + motionVector;
  vesVector3f newViewPoint = viewPoint + motionVector;
  camera->setFocalPoint(newViewFocus);
  camera->setPosition(newViewPoint);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleSingleTouchPanGesture(double deltaX, double deltaY)
{
  //
  // Rotate camera
  // Based on vtkInteractionStyleTrackballCamera::Rotate().
  //
  vesSharedPtr<vesRenderer> ren = this->Internal->Renderer;
  std::tr1::shared_ptr<vesCamera> camera = ren->camera();

  double delta_elevation = -20.0 / ren->height();
  double delta_azimuth   = -20.0 / ren->width();

  double motionFactor = 10.0;

  double rxf = deltaX * delta_azimuth * motionFactor;
  double ryf = deltaY * delta_elevation * motionFactor;

  camera->azimuth(rxf);
  camera->elevation(ryf);
  camera->orthogonalizeViewUp();
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
  vesNotUsed(displayX);
  vesNotUsed(displayY);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleTwoTouchPinchGesture(double scale)
{
  this->Internal->Renderer->camera()->dolly(scale);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleTwoTouchRotationGesture(double rotation)
{
  std::tr1::shared_ptr<vesCamera> camera = this->Internal->Renderer->camera();
  camera->roll(rotation * 180.0 / M_PI);
  camera->orthogonalizeViewUp();
}

//----------------------------------------------------------------------------
vesSharedPtr<vesShaderProgram> vesKiwiBaseApp::addShaderProgram(
  const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
  vesSharedPtr<vesShaderProgram> shaderProgram (new vesShaderProgram());
  vesSharedPtr<vesShader> vertexShader(new vesShader(vesShader::Vertex, vertexShaderSource));
  vesSharedPtr<vesShader> fragmentShader(new vesShader(vesShader::Fragment, fragmentShaderSource));

  shaderProgram->addShader(vertexShader);
  shaderProgram->addShader(fragmentShader);

  this->Internal->ShaderPrograms.push_back(shaderProgram);
  this->Internal->Shaders.push_back(vertexShader);
  this->Internal->Shaders.push_back(fragmentShader);
  return shaderProgram;
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::deleteShaderProgram(vesSharedPtr<vesShaderProgram> shaderProgram)
{
  if (!shaderProgram) {
    // Nothing to remove.
    return;
  }

  // Remove only one.
  std::vector< vesSharedPtr<vesShaderProgram> >::iterator itr =
    std::find(this->Internal->ShaderPrograms.begin(),
              this->Internal->ShaderPrograms.end(), shaderProgram);

  if (itr != this->Internal->ShaderPrograms.end()) {
    this->Internal->ShaderPrograms.erase(itr);
  }
}

//----------------------------------------------------------------------------
vesSharedPtr<vesUniform> vesKiwiBaseApp::addModelViewMatrixUniform(
  vesSharedPtr<vesShaderProgram> program, const std::string& name)
{
  this->Internal->Uniforms.push_back(
    name.empty() ? vesSharedPtr<vesUniform>(new vesModelViewUniform())
    : vesSharedPtr<vesUniform>(new vesModelViewUniform(name)));
  program->addUniform(this->Internal->Uniforms.back());

  return this->Internal->Uniforms.back();
}

//----------------------------------------------------------------------------
vesSharedPtr<vesUniform> vesKiwiBaseApp::addProjectionMatrixUniform(
  vesSharedPtr<vesShaderProgram> program, const std::string& name)
{
  this->Internal->Uniforms.push_back(
    name.empty() ? vesSharedPtr<vesUniform>(new vesProjectionUniform()) :
    vesSharedPtr<vesUniform>(new vesProjectionUniform(name)));

  program->addUniform(this->Internal->Uniforms.back());

  return this->Internal->Uniforms.back();
}

//----------------------------------------------------------------------------
vesSharedPtr<vesUniform> vesKiwiBaseApp::addNormalMatrixUniform(
  vesSharedPtr<vesShaderProgram> program, const std::string& name)
{
  this->Internal->Uniforms.push_back(
    name.empty() ? vesSharedPtr<vesUniform>(new vesNormalMatrixUniform())
    : vesSharedPtr<vesUniform>(new vesNormalMatrixUniform(name)));
  program->addUniform(this->Internal->Uniforms.back());

  return this->Internal->Uniforms.back();
}

//----------------------------------------------------------------------------
vesSharedPtr<vesVertexAttribute> vesKiwiBaseApp::addVertexPositionAttribute(
  vesSharedPtr<vesShaderProgram> program, const std::string& name)
{
  this->Internal->VertexAttributes.push_back(
    name.empty() ? vesSharedPtr<vesVertexAttribute>(new vesPositionVertexAttribute())
    : vesSharedPtr<vesVertexAttribute>(new vesPositionVertexAttribute(name)));
  program->addVertexAttribute(this->Internal->VertexAttributes.back(), vesVertexAttributeKeys::Position);

  return this->Internal->VertexAttributes.back();
}

//----------------------------------------------------------------------------
vesSharedPtr<vesVertexAttribute> vesKiwiBaseApp::addVertexNormalAttribute(
  vesSharedPtr<vesShaderProgram> program, const std::string& name)
{
  this->Internal->VertexAttributes.push_back(
    name.empty() ? vesSharedPtr<vesVertexAttribute>(new vesNormalVertexAttribute()):
    vesSharedPtr<vesVertexAttribute>(new vesNormalVertexAttribute(name)));
  program->addVertexAttribute(this->Internal->VertexAttributes.back(), vesVertexAttributeKeys::Normal);

  return this->Internal->VertexAttributes.back();
}

//----------------------------------------------------------------------------
vesSharedPtr<vesVertexAttribute> vesKiwiBaseApp::addVertexColorAttribute(
  vesSharedPtr<vesShaderProgram> program, const std::string& name)
{
  this->Internal->VertexAttributes.push_back(
    name.empty() ? vesSharedPtr<vesVertexAttribute>(new vesColorVertexAttribute())
    : vesSharedPtr<vesVertexAttribute>(new vesColorVertexAttribute(name)));
  program->addVertexAttribute(this->Internal->VertexAttributes.back(), vesVertexAttributeKeys::Color);

  return this->Internal->VertexAttributes.back();
}

//----------------------------------------------------------------------------
vesSharedPtr<vesVertexAttribute> vesKiwiBaseApp::addVertexTextureCoordinateAttribute(
  vesSharedPtr<vesShaderProgram> program, const std::string& name)
{
  this->Internal->VertexAttributes.push_back(
    name.empty() ? vesSharedPtr<vesVertexAttribute>(new vesTextureCoordinateVertexAttribute())
    : vesSharedPtr<vesVertexAttribute>(new vesTextureCoordinateVertexAttribute(name)));
  program->addVertexAttribute(this->Internal->VertexAttributes.back(), vesVertexAttributeKeys::TextureCoordinate);

  return this->Internal->VertexAttributes.back();
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

//----------------------------------------------------------------------------
vesVector3f vesKiwiBaseApp::cameraPosition() const
{
  return this->renderer()->camera()->position();
}

//----------------------------------------------------------------------------
vesVector3f vesKiwiBaseApp::cameraFocalPoint() const
{
  return this->renderer()->camera()->focalPoint();
}

//----------------------------------------------------------------------------
vesVector3f vesKiwiBaseApp::cameraViewUp() const
{
  return this->renderer()->camera()->viewUp();
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::setCameraPosition(const vesVector3f& position)
{
  this->renderer()->camera()->setPosition(position);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::setCameraFocalPoint(const vesVector3f& focalPoint)
{
  this->renderer()->camera()->setFocalPoint(focalPoint);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::setCameraViewUp(const vesVector3f& viewUp)
{
  this->renderer()->camera()->setViewUp(viewUp);
}
