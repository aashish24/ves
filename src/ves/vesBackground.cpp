
#include "vesBackground.h"

// VES includes.
#include "vesActor.h"
#include "vesDepth.h"
#include "vesMapper.h"
#include "vesMaterial.h"
#include "vesViewport.h"

class vesBackground::vesInternal
{
public:
  vesInternal()
  {
    this->m_backgroundActor = new vesActor();
    this->m_backgroundMapper = new vesMapper();
    this->m_backgroundMaterial = new vesMaterial();
    this->m_depth = new vesDepth();
  }

  ~vesInternal()
  {
  }

  void createBackground();

  vesActor *m_backgroundActor;
  vesMapper *m_backgroundMapper;
  vesMaterial *m_backgroundMaterial;
  vesDepth *m_depth;
};


void vesBackground::vesInternal::createBackground()
{
  this->m_backgroundActor->setMapper(this->m_backgroundMapper);
  this->m_backgroundActor->setMaterial(this->m_backgroundMaterial);

  this->m_backgroundMaterial->addAttribute(this->m_depth);
}


vesBackground::vesBackground() : vesCamera()
{
  this->setReferenceFrame(Absolute);
  this->setRenderOrder(PreRender);

  this->createBackground();
}


vesBackground::~vesBackground()
{
}


vesMatrix4x4f vesBackground::modelViewMatrix()
{
  return vesMatrix4x4f();
}


vesMatrix4x4f vesBackground::projectionMatrix()
{
  vesMatrix4x4f projection2DMatrix = vesOrtho(this->m_viewport->x(),
    this->m_viewport->width(), this->m_viewport->y(), this->m_viewport->height(), -1, 1);
  return projection2DMatrix;
}


void vesBackground::createBackground()
{
  this->m_internal->createBackground();
}
