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
#include <vesKiwiCameraInteractor.h>
#include <vesCamera.h>
#include <vesOpenGLSupport.h>
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
#include <vesKiwiCameraTransition.h>

#include "vtkTimerLog.h"

#include <cassert>
#include <cmath>
#include <vector>
#include <set>
#include <iostream>

//----------------------------------------------------------------------------
class vesKiwiBaseApp::vesInternal
{
public:

  vesInternal()
  {
    this->GLSupport = vesOpenGLSupport::Ptr(new vesOpenGLSupport());
    this->Renderer = vesRenderer::Ptr(new vesRenderer());
    this->CameraInteractor = vesKiwiCameraInteractor::Ptr(new vesKiwiCameraInteractor);
    this->CameraInteractor->setRenderer(this->Renderer);
  }

  ~vesInternal()
  {
  }

  vesOpenGLSupport::Ptr GLSupport;
  vesRenderer::Ptr Renderer;
  vesKiwiCameraInteractor::Ptr CameraInteractor;

  std::vector< vesSharedPtr<vesShaderProgram> > ShaderPrograms;
  std::vector< vesSharedPtr<vesShader> > Shaders;
  std::vector< vesSharedPtr<vesUniform> > Uniforms;
  std::vector< vesSharedPtr<vesVertexAttribute> > VertexAttributes;
  typedef std::set<vesKiwiTransition::Ptr> TransitionSet;
  TransitionSet Transitions;
};

//----------------------------------------------------------------------------
vesKiwiBaseApp::vesKiwiBaseApp()
{
  this->Internal = new vesInternal();
}

//----------------------------------------------------------------------------
vesKiwiBaseApp::~vesKiwiBaseApp()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::initGL()
{
  if (this->Internal->GLSupport->isInitialized()) {
    std::cerr << "error: initGL() has already been called" << std::endl;
    return;
  }
  this->Internal->GLSupport->initialize();
}

//----------------------------------------------------------------------------
vesOpenGLSupport::Ptr vesKiwiBaseApp::glSupport()
{
  if (!this->Internal->GLSupport->isInitialized()) {
    std::cerr << "error: glSupport() called before initGL()" << std::endl;
  }
  return this->Internal->GLSupport;
}

//----------------------------------------------------------------------------
vesSharedPtr<vesCamera> vesKiwiBaseApp::camera() const
{
  assert(this->Internal->Renderer);
  return this->Internal->Renderer->camera();
}

//----------------------------------------------------------------------------
vesSharedPtr<vesRenderer> vesKiwiBaseApp::renderer() const
{
  return this->Internal->Renderer;
}

//------------------------------------------------------------------------------
vesKiwiTransition::Ptr vesKiwiBaseApp::addTransition(vesKiwiTransition::Ptr transition)
{
  double curTime = vtkTimerLog::GetUniversalTime();
  if (transition->begin(curTime, this))
    {
    this->Internal->Transitions.insert(transition);
    }
  return transition;
}

/// Return the currently-running transitions.
/// You may not modify the set but you may call methods on the transitions.
const std::set<vesKiwiTransition::Ptr>& vesKiwiBaseApp::activeTransitions() const
{
  return this->Internal->Transitions;
}

//----------------------------------------------------------------------------
/**\brief Give each active transition the opportunity to prepare for a render.
 *
 * This method is called by render() after willRender() and before the
 * camera clipping range is reset.
 *
 * If a transition indicates it has completed since the previous frame,
 * this method will also dequeue it and queue any transitions chained
 * to start upon its completion.
 */
void vesKiwiBaseApp::updateTransitions()
{
  vesInternal::TransitionSet::iterator it;
  vesInternal::TransitionSet dead;
  double curTime = vtkTimerLog::GetUniversalTime();
  for (it = this->Internal->Transitions.begin(); it != this->Internal->Transitions.end(); ++it)
    {
    if (!(*it)->update(curTime, this))
      {
      dead.insert(*it);
      }
    }
  for (it = dead.begin(); it != dead.end(); ++it)
    {
    this->Internal->Transitions.erase(*it);
    }
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::render()
{
  this->willRender();
  this->updateTransitions();
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
void vesKiwiBaseApp::resetView(bool withTransition)
{
  this->resetView(vesVector3f(0.0, 0.0, -1.0), vesVector3f(0.0, 1.0, 0.0), withTransition);
}

//----------------------------------------------------------------------------
vesKiwiCameraInteractor::Ptr vesKiwiBaseApp::cameraInteractor() const
{
  return this->Internal->CameraInteractor;
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::resetView(const vesVector3f& viewDirection, const vesVector3f& viewUp, bool withTransition)
{
  // this is just confusing...
  // We want to set the direction to look from and view up
  // then we want to dolly the camera so that the surface takes up
  // a reasonable portion of the screen.
  // the ResetCamera function will overwrite view up
  // so we have to call things in this strange order.

  vesSharedPtr<vesRenderer> renderer = this->Internal->Renderer;
  vesCamera::Ptr camera = renderer->camera();
  vesKiwiCameraTransition::Ptr transition =
    vesKiwiCameraTransition::create(camera);
  transition
    ->takeInitialFrame()
    ->setDuration(1.0);

  //
  // set direction to look from
  camera->setViewPlaneNormal(-viewDirection);

  // dolly so that scene fits window
  renderer->resetCamera();

  // now set the up vector
  renderer->camera()->setViewUp(viewUp);
  renderer->camera()->orthogonalizeViewUp();

  if (withTransition)
    {
    transition->setFinalFrame(camera->position(), camera->focalPoint(), viewUp);
    this->addTransition(transition);
    }
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleTwoTouchPanGesture(double x0, double y0, double x1, double y1)
{
  this->Internal->CameraInteractor->pan(vesVector2d(x0, y0), vesVector2d(x1, y1));
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleSingleTouchPanGesture(double deltaX, double deltaY)
{
  this->Internal->CameraInteractor->rotate(vesVector2d(deltaX, deltaY));
}


//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleSingleTouchTap(int displayX, int displayY)
{
  vesNotUsed(displayX);
  vesNotUsed(displayY);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleDoubleTap(int displayX, int displayY)
{
  vesNotUsed(displayX);
  vesNotUsed(displayY);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleLongPress(int displayX, int displayY)
{
  vesNotUsed(displayX);
  vesNotUsed(displayY);
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
  this->Internal->CameraInteractor->dolly(scale);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::handleTwoTouchRotationGesture(double rotation)
{
  this->Internal->CameraInteractor->roll(rotation);
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
  this->Internal->Renderer->setBackgroundColor(r, g, b);
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
