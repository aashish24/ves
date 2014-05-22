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

#include <android/log.h>

#define  LOG_TAG    "KiwiViewer"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#include "vesCullVisitor.h"

// VES includes
#include "vesActor.h"
#include "vesCamera.h"
#include "vesGroupNode.h"
#include "vesNode.h"
#include "vesRenderStage.h"
#include "vesTransformNode.h"

void vesCullVisitor::addGeometryAndStates(
  const vesSharedPtr<vesMapper> &mapper,
  const vesSharedPtr<vesMaterial> &material,
  const vesMatrix4x4f &modelViewMatrix,
  const vesMatrix4x4f &projectionMatrix,
  float depth)
{
  this->renderStage()->addRenderLeaf(
    vesRenderLeaf(depth, modelViewMatrix, projectionMatrix, material, mapper));

  LOGI("projectionMatrix %f", projectionMatrix(0, 0));
  LOGI("projectionMatrix %f", projectionMatrix(1, 0));
  LOGI("projectionMatrix %f", projectionMatrix(2, 0));
  LOGI("projectionMatrix %f", projectionMatrix(3, 0));
}


void vesCullVisitor::visit(vesNode &node)
{
  this->invokeCallbacksAndTraverse(node);
}


void vesCullVisitor::visit(vesGroupNode &groupNode)
{
  this->invokeCallbacksAndTraverse(groupNode);
}


void vesCullVisitor::visit(vesTransformNode &transformNode)
{
  vesMatrix4x4f matrix = this->modelViewMatrix();
  transformNode.computeLocalToWorldMatrix(matrix, *this);

  this->pushModelViewMatrix(matrix);

  this->invokeCallbacksAndTraverse(transformNode);

  this->popModelViewMatrix();
}


void vesCullVisitor::visit(vesActor &actor)
{
  vesMatrix4x4f matrix = this->modelViewMatrix();
  actor.computeLocalToWorldMatrix(matrix, *this);

  this->pushModelViewMatrix(matrix);

  if (actor.isVisible()) {
    if (actor.isOverlayNode()) {
    this->addGeometryAndStates(actor.mapper(), actor.material(),
      actor.modelViewMatrix(),  this->projection2DMatrix(), 1);
    }
    else {
      // \todo: We could do some optimization here.
      this->addGeometryAndStates(actor.mapper(), actor.material(),
        this->modelViewMatrix(), this->projectionMatrix(), 1);
    }
  }

  this->invokeCallbacksAndTraverse(actor);

  this->popModelViewMatrix();
}


void vesCullVisitor::visit(vesCamera &camera)
{
  vesMatrix4x4f matrix = this->modelViewMatrix();
  camera.computeLocalToWorldMatrix(matrix, *this);

  this->pushModelViewMatrix(matrix);

  if (camera.referenceFrame() == vesTransformNode::Relative) {
    this->pushProjectionMatrix(camera.projectionMatrix());
  } else {
    this->pushProjectionMatrix(camera.projectionMatrix());
  }

  // If camera is set as a NestedRender, treat camera as
  // a node that contains the subgraph in the current render stage.
  if (camera.renderOrder() == vesCamera::NestedRender) {
    this->invokeCallbacksAndTraverse(camera);
  } else {

    vesSharedPtr<vesRenderStage> previousRenderStage = this->renderStage();

    vesSharedPtr<vesRenderStage> renderStage =
      camera.getOrCreateRenderStage();
    renderStage->clearAll();
    renderStage->setViewport( camera.viewport()
      ? camera.viewport() : previousRenderStage->viewport() );
    renderStage->setClearColor(camera.clearColor());
    renderStage->setClearMask(camera.clearMask());
    renderStage->setClearDepth(camera.clearDepth());

    this->pushRenderStage(renderStage);

    this->invokeCallbacksAndTraverse(camera);

    this->popRenderStage();

    switch (camera.renderOrder()) {
    case vesCamera::PreRender:
      this->renderStage()->addPreRenderStage(renderStage, camera.renderOrderPriority());
      break;
    case vesCamera::PostRender:
      this->renderStage()->addPostRenderStage(renderStage, camera.renderOrderPriority());
      break;
    case vesCamera::NestedRender:
    default:
      break;
    };
  }

  this->popProjectionMatrix();
  this->popModelViewMatrix();
}
