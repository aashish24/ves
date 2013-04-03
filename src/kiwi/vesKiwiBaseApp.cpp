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
#include <vesActor.h>
#include <vesBackground.h>
#include <vesCamera.h>
#include <vesOpenGLSupport.h>
#include <vesRenderer.h>
#include <vesSetGet.h>
#include <vesStateAttributeBits.h>
#include <vesVertexAttribute.h>
#include <vesVertexAttributeKeys.h>
#include <vesViewport.h>

#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

//----------------------------------------------------------------------------
class vesKiwiBaseApp::vesInternal
{
public:

  vesInternal()
  {
    this->GLSupport = vesOpenGLSupport::Ptr(new vesOpenGLSupport());
    this->Renderer = vesRenderer::Ptr(new vesRenderer());
    this->Renderers.push_back(this->Renderer);
    this->CameraInteractor = vesKiwiCameraInteractor::Ptr(new vesKiwiCameraInteractor);
    this->CameraInteractor->setRenderer(this->Renderer);
  }

  ~vesInternal()
  {
  }

  vesOpenGLSupport::Ptr GLSupport;
  vesRenderer::Ptr Renderer;
  std::vector<vesRenderer::Ptr> Renderers;
  vesKiwiCameraInteractor::Ptr CameraInteractor;

  std::vector< vesSharedPtr<vesMaterial> > Materials;
  std::vector< vesSharedPtr<vesVertexAttribute> > VertexAttributes;
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

  for (size_t i = 0; i < this->Internal->Renderers.size(); ++i) {

    if (this->Internal->Renderer == this->Internal->Renderers[i]) {
      continue;
    }

    this->Internal->Renderers[i]->resetCameraClippingRange();
    this->Internal->Renderers[i]->render();
  }
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
  this->resetView(vesVector3f(0.0, 0.0, -1.0), vesVector3f(0.0, 1.0, 0.0));
}

//----------------------------------------------------------------------------
vesKiwiCameraInteractor::Ptr vesKiwiBaseApp::cameraInteractor() const
{
  return this->Internal->CameraInteractor;
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::setViewRect(int index, int x, int y, int width, int height)
{
  if (index >= static_cast<int>(this->Internal->Renderers.size())) {
    std::cerr << "error: Invalid index " << index << " for the viewport "
              << std::endl;
  }
  this->resizeView(width, height);
  this->resetView();
  this->Internal->Renderers[index]->camera()->viewport()->setViewport(
    x, y, width, height);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::addViewRect(int x, int y, int width, int height)
{
  vesRenderer::Ptr ren = vesRenderer::Ptr(new vesRenderer());
  ren->setBackgroundColor(1.0, 1.0, 0.0);
  ren->background()->setClearMask(vesStateAttributeBits::DepthBufferBit);
  ren->camera()->setClearMask(vesStateAttributeBits::DepthBufferBit);
  ren->resize(width, height, 1.0f);

  // Use main camera parameters as initial values for
  // new renderer camera parameters
  ren->camera()->setPosition(
        this->Internal->Renderer->camera()->position());
  ren->camera()->setFocalPoint(
        this->Internal->Renderer->camera()->focalPoint());
  ren->camera()->setViewUp(
        this->Internal->Renderer->camera()->viewUp());
  ren->camera()->setViewPlaneNormal(
        this->Internal->Renderer->camera()->viewPlaneNormal());
  ren->camera()->viewport()->setViewport(
    x, y, width, height);
  ren->background()->viewport()->setViewport(
    x, y, width, height);

  this->Internal->Renderers.push_back(ren);

  this->Internal->Renderer->background()->setClearMask(
    vesStateAttributeBits::ColorBufferBit |
    vesStateAttributeBits::DepthBufferBit);
  this->Internal->Renderer->camera()->setClearMask(
    vesStateAttributeBits::ColorBufferBit |
    vesStateAttributeBits::DepthBufferBit);
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::syncViewports()
{
  std::vector<vesActor::Ptr> actors = this->Internal->Renderer->sceneActors();

  for (size_t i = 0; i < this->Internal->Renderers.size(); ++i) {

    if (this->Internal->Renderer == this->Internal->Renderers[i]) {
      continue;
    }

    for (size_t j = 0; j < actors.size(); ++j) {
      vesActor::Ptr newActor = vesActor::Ptr(new vesActor());
      vesMapper::Ptr newMapper = vesMapper::Ptr(new vesMapper());
      newMapper->setGeometryData(actors[j]->mapper()->geometryData());
      newActor->setMaterial(actors[j]->material());
      newActor->setMapper(newMapper);

      this->Internal->Renderers[i]->addActor(newActor);
    }
  }
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::resetView(const vesVector3f& viewDirection, const vesVector3f& viewUp)
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

  renderer->camera()->setViewPlaneNormal(-viewDirection);

  // dolly so that scene fits window
  renderer->resetCamera();

  // The current ResetCamera() method pulls the camera back further than
  // required.  ResetCamera should be fixed.  Until then, perform a dolly
  // with a scale factor of 1.5 (a magic number).
  renderer->camera()->dolly(1.5);

  // now set the view plane normal
  renderer->camera()->setViewUp(viewUp);
  renderer->camera()->orthogonalizeViewUp();
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
vesSharedPtr<vesMaterial> vesKiwiBaseApp::addMaterial()
{
  vesSharedPtr<vesMaterial> material (new vesMaterial());
  this->Internal->Materials.push_back(material);
  return material;
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::deleteMaterial(vesSharedPtr<vesMaterial> material)
{
  if (!material) {
    // Nothing to remove.
    return;
  }

  // Remove only one.
  std::vector< vesSharedPtr<vesMaterial> >::iterator itr =
    std::find(this->Internal->Materials.begin(),
              this->Internal->Materials.end(), material);

  if (itr != this->Internal->Materials.end()) {
    this->Internal->Materials.erase(itr);
  }
}

//----------------------------------------------------------------------------
vesSharedPtr<vesVertexAttribute> vesKiwiBaseApp::addVertexPositionAttribute(
  vesSharedPtr<vesMaterial> material, const std::string& name)
{
  this->Internal->VertexAttributes.push_back(
    name.empty() ? vesSharedPtr<vesVertexAttribute>(new vesPositionVertexAttribute())
    : vesSharedPtr<vesVertexAttribute>(new vesPositionVertexAttribute(name)));
  material->addAttribute(this->Internal->VertexAttributes.back());

  return this->Internal->VertexAttributes.back();
}

//----------------------------------------------------------------------------
vesSharedPtr<vesVertexAttribute> vesKiwiBaseApp::addVertexNormalAttribute(
  vesSharedPtr<vesMaterial> material, const std::string& name)
{
  this->Internal->VertexAttributes.push_back(
    name.empty() ? vesSharedPtr<vesVertexAttribute>(new vesNormalVertexAttribute()):
    vesSharedPtr<vesVertexAttribute>(new vesNormalVertexAttribute(name)));
  material->addAttribute(this->Internal->VertexAttributes.back());

  return this->Internal->VertexAttributes.back();
}

//----------------------------------------------------------------------------
vesSharedPtr<vesVertexAttribute> vesKiwiBaseApp::addVertexColorAttribute(
  vesSharedPtr<vesMaterial> material, const std::string& name)
{
  this->Internal->VertexAttributes.push_back(
    name.empty() ? vesSharedPtr<vesVertexAttribute>(new vesColorVertexAttribute())
    : vesSharedPtr<vesVertexAttribute>(new vesColorVertexAttribute(name)));
  material->addAttribute(this->Internal->VertexAttributes.back());

  return this->Internal->VertexAttributes.back();
}

//----------------------------------------------------------------------------
vesSharedPtr<vesVertexAttribute> vesKiwiBaseApp::addVertexTextureCoordinateAttribute(
  vesSharedPtr<vesMaterial> material, const std::string& name)
{
  this->Internal->VertexAttributes.push_back(
    name.empty() ? vesSharedPtr<vesVertexAttribute>(new vesTextureCoordinateVertexAttribute())
    : vesSharedPtr<vesVertexAttribute>(new vesTextureCoordinateVertexAttribute(name)));
  material->addAttribute(this->Internal->VertexAttributes.back());

  return this->Internal->VertexAttributes.back();
}

//----------------------------------------------------------------------------
void vesKiwiBaseApp::setBackgroundColor(double r, double g, double b)
{
  this->Internal->Renderer->setBackgroundColor(r, g, b, 1.0);

  for (size_t i = 0; i < this->Internal->Renderers.size(); ++i) {

    if (this->Internal->Renderer == this->Internal->Renderers[i]) {
      continue;
    }
    this->Internal->Renderers[i]->setBackgroundColor(r, g, b, 1.0);
  }
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
