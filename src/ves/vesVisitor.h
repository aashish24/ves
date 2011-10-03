#ifndef VESVISITOR_H
#define VESVISITOR_H

#include "vesActor.h"

// Forward declarations
class vesMapper;
class vesMaterial;

class vesVisitor
{
public:
  enum TraversalMode
  {
    TraverseNone            = 0x1,
    TraverseParents         = 0x2,
    TraverseAllChildren     = 0x4,
    TraverseActiveChildren  = 0x8
  };

  enum VisitorType
  {
    ActorVisitor  = 0x1,
    UpdateVisitor = 0x2,
    EventVisitor  = 0x4,
    CullVisitor   = 0x8
  };

           vesVisitor(VisitorType type, TraversalMode=TraverseNone);
  virtual ~vesVisitor();

  void            pushActor(const vesActor& actor);
  void            popActor();
  vesActor*       actor();
  const vesActor* actor() const;

  void  pushModelViewMatrix(const vesMatrix4x4f& matrix);
  void  popModelViewMatrix ();

  void  pushProjectionMatrix(const vesMatrix4x4f& matrix);
  void  popProjectionMatrix ();

  vesMatrix4x4f& modelViewMatrix();
  vesMatrix4x4f& projectionMatrix();

  inline void traverse(vesActor& actor)
  {
      if (m_traversalMode == TraverseParents) {
        actor.ascend(*this);
      }
      else if (m_traversalMode != TraverseNone) {
        actor.traverse(*this);
      }
  }

  virtual void visit(vesActor  &actor);
  virtual void visit(vesMapper &mapper);

#if 0
  virtual void visit(vesMaterial& material);
#endif

  VisitorType   type(){ return this->m_visitorType; }
  TraversalMode mode(){ return this->m_traversalMode; }

protected:

  TraversalMode m_traversalMode;
  VisitorType   m_visitorType;

  class vesInternal;
  vesInternal* m_internal;
};


#endif // VESVISITOR_H
