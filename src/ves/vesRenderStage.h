#ifndef VESRENDERSTAGE_H
#define VESRENDERSTAGE_H

#include "vesGMTL.h"

#include "vesMapper.h"
#include "vesMaterial.h"

// C++ includes
#include <map>
#include <vector>

class vesRenderState
{
public:
  vesRenderState()
  {
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
  vesMatrix4x4f *m_projectionMatrix;
  vesMatrix4x4f *m_modelViewMatrix;
  vesMatrix4x4f *m_identity;
};


class vesRenderLeaf
{
public:
  vesRenderLeaf(int depth, const vesMatrix4x4f &modelViewMatrix,
                const vesMatrix4x4f &projectionMatrix,
                vesMaterial &material, vesMapper &mapper)
  {
    this->m_depth            = depth;
    this->m_bin              = material.binNumber();
    this->m_modelViewMatrix  = modelViewMatrix;
    this->m_projectionMatrix = projectionMatrix;
    this->m_material         = &material;
    this->m_mapper           = &mapper;
  }


  ~vesRenderLeaf()
  {
  }


  void render(vesRenderState &renderState, vesRenderLeaf *previous)
  {
    // \note: Ignore previous as of now (optimizaion).
    if (previous && this->m_material != previous->m_material) {
      renderState.invokeMaterial(previous->m_material);
    }

    renderState.applyProjectionMatrix (&this->m_projectionMatrix);
    renderState.applyModelViewMatrix  (&this->m_modelViewMatrix);
    renderState.applyMapper(this->m_mapper);
    renderState.applyMaterial(this->m_material);

    this->m_mapper->render(renderState);
  }


  int            m_depth;
  int            m_bin;

  vesMatrix4x4f m_projectionMatrix;
  vesMatrix4x4f m_modelViewMatrix;

  vesMaterial   *m_material;
  vesMapper     *m_mapper;
};


class vesRenderStage
{
public:
  typedef std::vector< vesRenderLeaf > RenderLeaves;
  typedef std::map<int, RenderLeaves > BinRenderLeavesMap;

  // \todo: Use it later.
  enum SortMode
  {
    BackToFront = 0x0,
    FrontToBack,
    SortByState
  };


  vesRenderStage()
  {
  }


 ~vesRenderStage()
  {
  }


  void addRenderLeaf(const vesRenderLeaf &renderLeaf)
  {
    this->m_binRenderLeavesMap[renderLeaf.m_bin].push_back(renderLeaf);
  }


  void sort(SortMode mode)
  {
    // \todo: Implement this.
  }


  void render(vesRenderState &renderState, vesRenderLeaf *previous)
  {
    BinRenderLeavesMap::iterator itr = this->m_binRenderLeavesMap.begin();
    RenderLeaves::iterator rlsItr;

    for (itr; itr != this->m_binRenderLeavesMap.end(); ++itr) {
      for (rlsItr = itr->second.begin(); rlsItr != itr->second.end(); ++rlsItr)
      {
        (*rlsItr).render(renderState, previous);

        previous = &(*rlsItr);
      }
    }
  }


  void clearAll()
  {
    this->m_binRenderLeavesMap.clear();
  }


private:

  BinRenderLeavesMap  m_binRenderLeavesMap;

  // Not implemented.
  vesRenderStage   (const vesRenderStage&);
  void operator=   (const vesRenderStage&);
};

#endif // VESRENDERSTAGE_H
