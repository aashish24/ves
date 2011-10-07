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

#ifndef VESRENDERSTATE_H
#define VESRENDERSTATE_H

// VES includes
#include "vesMaterial.h"

// Forward declarations
class vesMapper;

/*! Data structure to hold objects and states related to rendering. */
class vesRenderState
{
public:
  vesRenderState()
  {
    this->m_material = 0x0;
    this->m_mapper   = 0x0;
    this->m_identity = new vesMatrix4x4f();

    this->m_modelViewMatrix   = this->m_identity;
    this->m_projectionMatrix  = this->m_identity;
  }


  ~vesRenderState()
  {
    delete this->m_identity; this->m_identity = 0x0;
  }


  void applyMaterial(vesMaterial *&material)
  {
    if (material != this->m_material) {
      this->m_material = material;

      this->m_material->render(*this);
    }
  }


  void invokeMaterial(vesMaterial *&material)
  {
    if (material == this->m_material) {
      this->m_material->remove(*this);
    }
  }


  void applyMapper(vesMapper *&mapper)
  {
    if (mapper != this->m_mapper) {
      this->m_mapper = mapper;
    }
  }


  void applyModelViewMatrix(vesMatrix4x4f *modelViewMatrix)
  {
    if (modelViewMatrix != m_modelViewMatrix && modelViewMatrix) {
      this->m_modelViewMatrix = modelViewMatrix;
    }
  }


  void applyProjectionMatrix(vesMatrix4x4f *projectionMatrix)
  {
    if (projectionMatrix != m_projectionMatrix && projectionMatrix) {
      this->m_projectionMatrix = projectionMatrix;
    }
  }

  vesMaterial   *m_material;
  vesMapper     *m_mapper;
  vesMatrix4x4f *m_identity;
  vesMatrix4x4f *m_projectionMatrix;
  vesMatrix4x4f *m_modelViewMatrix;
};

#endif // VESRENDERSTATE_H
