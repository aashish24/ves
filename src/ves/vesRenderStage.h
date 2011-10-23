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

#ifndef VESRENDERSTAGE_H
#define VESRENDERSTAGE_H

// VES includes
#include "vesGMTL.h"
#include "vesMapper.h"
#include "vesMaterial.h"
#include "vesRenderLeaf.h"
#include "vesViewport.h"

// C++ includes
#include <list>
#include <map>
#include <vector>

class vesRenderStage
{
public:
  typedef std::vector< vesRenderLeaf> RenderLeaves;
  typedef std::map<int, RenderLeaves> BinRenderLeavesMap;

  // \todo: Use it later.
  enum SortMode
  {
    BackToFront = 0x0,
    FrontToBack,
    SortByState
  };

  vesRenderStage() :
    m_viewport(0x0)
  {
    // Do nothing as of now.
  }

 ~vesRenderStage()
  {
    this->m_binRenderLeavesMap.clear();
  }

  void addRenderLeaf(const vesRenderLeaf &renderLeaf)
  {
    this->m_binRenderLeavesMap[renderLeaf.m_bin].push_back(renderLeaf);
  }

  void setViewport(vesViewport *viewport) { this->m_viewport = viewport; }
  const vesViewport* viewport() const { return this->m_viewport; }
  vesViewport* viewport() { return this->m_viewport; }

  void sort(SortMode mode)
  {
    // \todo: Implement this.
  }

  void render(vesRenderState &renderState, vesRenderLeaf *previous)
  {
    this->renderPreRenderStages(renderState, previous);

    this->m_viewport->render(renderState);

    BinRenderLeavesMap::iterator itr = this->m_binRenderLeavesMap.begin();
    RenderLeaves::iterator rlsItr;

    for (; itr != this->m_binRenderLeavesMap.end(); ++itr) {
      for (rlsItr = itr->second.begin(); rlsItr != itr->second.end(); ++rlsItr) {
        (*rlsItr).render(renderState, previous);

        previous = &(*rlsItr);
      }

      // Make sure to restore the state.
      if (!itr->second.empty()) {
        (*(--rlsItr)).finalize(renderState);
      }
    }

    this->renderPostRenderStages(renderState, previous);
  }

  void clearAll()
  {
    this->m_binRenderLeavesMap.clear();
    this->m_preRenderList.clear();
    this->m_postRenderList.clear();
  }

  void addPreRenderStage(vesRenderStage *renderStage, int priority);
  void addPostRenderStage(vesRenderStage *renderStage, int priority);

  void renderPreRenderStages(vesRenderState &renderState, vesRenderLeaf *previous);
  void renderPostRenderStages(vesRenderState &renderState, vesRenderLeaf *previous);

private:
  vesViewport *m_viewport;

  typedef std::pair< int, vesRenderStage* > RenderStageOrderPair;
  typedef std::list< RenderStageOrderPair > RenderStageList;

  BinRenderLeavesMap  m_binRenderLeavesMap;

  RenderStageList m_preRenderList;
  RenderStageList m_postRenderList;

  // Not implemented.
  vesRenderStage(const vesRenderStage&);
  void operator=(const vesRenderStage&);
};

#endif // VESRENDERSTAGE_H
