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

#include "vesRenderStage.h"

void vesRenderStage::addPreRenderStage(vesSharedPtr<vesRenderStage> renderStage,
                                       int priority)
{
  if (renderStage) {
    RenderStageList::iterator itr;
    for (itr = m_preRenderList.begin(); itr != m_preRenderList.end(); ++itr) {
      if(priority < itr->first) {
          break;
      }
    }

    if(itr == m_preRenderList.end()) {
      m_preRenderList.push_back(RenderStageOrderPair(priority, renderStage));
    } else {
      m_preRenderList.insert(itr,RenderStageOrderPair(priority, renderStage));
    }
  }
}


void vesRenderStage::addPostRenderStage(vesSharedPtr<vesRenderStage> renderStage,
                                        int priority)
{
  if (renderStage) {
    RenderStageList::iterator itr;
    for(itr = this->m_postRenderList.begin(); itr != this->m_postRenderList.end(); ++itr) {
      if(priority < itr->first) {
          break;
      }
    }
    if(itr == this->m_postRenderList.end()) {
      this->m_postRenderList.push_back(RenderStageOrderPair(priority, renderStage));
    } else {
      this->m_postRenderList.insert(itr,RenderStageOrderPair(priority,renderStage));
    }
  }
}


void vesRenderStage::renderPreRenderStages(vesRenderState &renderState,
                                           vesRenderLeaf *previous)
{
  if (this->m_preRenderList.empty()) {
    return;
  }

  for (RenderStageList::iterator itr = this->m_preRenderList.begin();
    itr != this->m_preRenderList.end(); ++itr) {
      itr->second->render(renderState, previous);
  }
}


void vesRenderStage::renderPostRenderStages(vesRenderState &renderState, vesRenderLeaf *previous)
{
  if (m_postRenderList.empty()) {
    return;
  }

  for (RenderStageList::iterator itr = this->m_postRenderList.begin();
      itr != this->m_postRenderList.end(); ++itr) {
    itr->second->render(renderState, previous);
  }
}


void vesRenderStage::setClearMask(unsigned int mask)
{
  this->m_clearMask = mask;
}


unsigned int vesRenderStage::clearMask() const
{
  return this->m_clearMask;
}


void vesRenderStage::setClearColor(const vesVector4f &clearColor)
{
  this->m_clearColor = clearColor;
}


vesVector4f vesRenderStage::clearColor()
{
  return this->m_clearColor;
}


const vesVector4f& vesRenderStage::clearColor() const
{
  return this->m_clearColor;
}


void vesRenderStage::setClearDepth(double depth)
{
  this->m_clearDepth = depth;
}


double vesRenderStage::clearDepth() const
{
  return this->m_clearDepth;
}
