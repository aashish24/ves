#ifndef VESCULLVISITOR_H
#define VESCULLVISITOR_H

// Base class
#include "vesVisitor.h"

// Forward declarations
class vesRenderStage;

class vesCullVisitor : public vesVisitor
{
public:
  vesCullVisitor(TraversalMode mode=TraverseAllChildren) :
    vesVisitor    (CullVisitor, mode),
    m_renderStage (0x0)
  {
  }


  ~vesCullVisitor()
  {
  }


  void setRenderStage(vesRenderStage *renderStage)
  {
    if (renderStage && this->m_renderStage != renderStage) {
      this->m_renderStage = renderStage;
    }
  }


  vesRenderStage* renderStage()
  {
    return this->m_renderStage;
  }


  const vesRenderStage* renderStage() const
  {
    return this->m_renderStage;
  }

  virtual void visit(vesActor  &actor);


protected:

  void addGeometryAndStates(vesMapper *mapper, vesMaterial *material,
                            const vesMatrix4x4f &modelViewMatrix,
                            const vesMatrix4x4f& projectionMatrix,
                            float depth);

  vesRenderStage *m_renderStage;
};

#endif // VESCULLVISITOR_H
