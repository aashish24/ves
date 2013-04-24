/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.
  Copyright 2012 Willow Garage, Inc.

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

#ifndef VESRENDERSTATE_H
#define VESRENDERSTATE_H

// VES includes
#include "vesGL.h"
#include "vesMaterial.h"
#include "vesMath.h"
#include "vesSetGet.h"

#include <map>

// Forward declarations
class vesMapper;

class vesGlobalRenderState
{
public:
  void enable(int cap)
  {
    glEnable(cap);
    this->m_attributesState[cap] = 1;
  }


  void disable(int cap)
  {
    glDisable(cap);
    this->m_attributesState[cap] = 0;
  }


  bool isEnabled(int cap)
  {
    std::map<int, int>::iterator itr = m_attributesState.find(cap);
    if (itr != m_attributesState.end()) {
      return static_cast<bool>(itr->second);
    }

    return false;
  }


  void get(int cap, bool& flag)
  {
    std::map<int, int>::iterator itr = m_attributesState.find(cap);
    if (itr != m_attributesState.end()) {
      flag = static_cast<bool>(itr->second);
    }

    flag = false;
  }


  bool set(int cap, bool flag)
  {
    this->m_attributesState[cap] = static_cast<int>(flag);
    return true;
  }

  std::map<int, int> m_attributesState;
};

/*! Data structure to hold objects and states related to rendering. */
class vesRenderState
{
public:
  vesTypeMacro(vesRenderState);

  vesRenderState()
  {
    this->m_identity = new vesMatrix4x4f;
    m_identity->setIdentity();

    this->m_modelViewMatrix   = this->m_identity;
    this->m_projectionMatrix  = this->m_identity;
  }


  ~vesRenderState()
  {
    delete this->m_identity; this->m_identity = 0x0;
  }


  void applyMaterial(const vesSharedPtr<vesMaterial> &material)
  {
    if (material && material != this->m_material) {
      this->m_material = material;
    }
  }


  void removeMaterial(const vesSharedPtr<vesMaterial> &material)
  {
    if (material && material == this->m_material) {
      this->m_material->remove(*this);
    }
  }


  void applyMapper(const vesSharedPtr<vesMapper> &mapper)
  {
    if (mapper && mapper != this->m_mapper) {
      this->m_mapper = mapper;
    }
  }


  void applyModelViewMatrix(vesMatrix4x4f *modelViewMatrix)
  {
    if (modelViewMatrix && modelViewMatrix != this->m_modelViewMatrix) {
      this->m_modelViewMatrix = modelViewMatrix;
    }
  }


  void applyProjectionMatrix(vesMatrix4x4f *projectionMatrix)
  {
    if (projectionMatrix  && projectionMatrix != m_projectionMatrix) {
      this->m_projectionMatrix = projectionMatrix;
    }
  }

  /// Return global render state
  /// @note Its Ok to modify the global state even if the render state
  /// is a const
  inline vesGlobalRenderState& getGlobalRenderState() const
  {
    return this->m_globalRenderState;
  }

  vesSharedPtr<vesMaterial> m_material;
  vesSharedPtr<vesMapper> m_mapper;

  vesMatrix4x4f *m_identity;
  vesMatrix4x4f *m_projectionMatrix;
  vesMatrix4x4f *m_modelViewMatrix;

  mutable vesGlobalRenderState m_globalRenderState;
};

#endif // VESRENDERSTATE_H
