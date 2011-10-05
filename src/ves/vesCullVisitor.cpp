
#include "vesCullVisitor.h"

// VES includes
#include "vesActor.h"
#include "vesRenderStage.h"


void vesCullVisitor::addGeometryAndStates(vesMapper *mapper, vesMaterial *material,
                                          const vesMatrix4x4f &modelViewMatrix,
                                          const vesMatrix4x4f &projectionMatrix,
                                          float depth)
{
  this->m_renderStage->addRenderLeaf(
    vesRenderLeaf(depth, modelViewMatrix, projectionMatrix, *material, *mapper));
}


void vesCullVisitor::visit(vesActor &actor)
{
  this->pushModelViewMatrix(actor.modelViewMatrix());

  // \todo: We could do some optimization here.
  this->addGeometryAndStates(actor.mapper(), actor.material(),
                             this->modelViewMatrix(), this->projectionMatrix(),
                             1);

  this->vesVisitor::visit(actor);
}
