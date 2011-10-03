
#include "vesVisitor.h"

#include "vesGMTL.h"
#include "vesMapper.h"

// C++ includes
#include <deque>
#include <vector>

class vesVisitor::vesInternal
{
public:

  std::deque <const vesActor*>      m_actorStack;
  std::vector<const vesMatrix4x4f*> m_modelViewMatrixStack;
  std::vector<const vesMatrix4x4f*> m_projectionMatrixStack;
};


vesVisitor::vesVisitor(VisitorType type, TraversalMode mode) :
  m_visitorType   (type),
  m_traversalMode (mode)
{
  this->m_internal = new vesInternal();
}


vesVisitor::~vesVisitor()
{
}


void vesVisitor::pushActor(const vesActor& actor)
{
  this->m_internal->m_actorStack.push_back(&actor);
}


void vesVisitor::popActor()
{
  this->m_internal->m_actorStack.pop_back();
}


vesActor* vesVisitor::actor()
{
  return const_cast<vesActor*>(this->actor());
}


const vesActor* vesVisitor::actor() const
{
  return this->m_internal->m_actorStack.back();
}


void vesVisitor::pushModelViewMatrix(const vesMatrix4x4f &matrix)
{
  this->m_internal->m_modelViewMatrixStack.push_back(&matrix);
}


void vesVisitor::popModelViewMatrix()
{
  this->m_internal->m_modelViewMatrixStack.pop_back();
}


void vesVisitor::pushProjectionMatrix(const vesMatrix4x4f &matrix)
{
  this->m_internal->m_projectionMatrixStack.push_back(&matrix);
}


void vesVisitor::popProjectionMatrix()
{
  this->m_internal->m_projectionMatrixStack.pop_back();
}


void vesVisitor::visit(vesActor &actor)
{
  std::cout << "visiting actor " << std::endl;
  actor.accept(*this);
}


void vesVisitor::visit(vesMapper &mapper)
{
  mapper.accept(*this);
}


//void vesVisitor::visit(vesMaterial &material)
//{
//  material.accept(this);
//}


