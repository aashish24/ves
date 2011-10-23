
#include "vesDepth.h"

// VES includes.
#include "vesGL.h"

vesDepth::vesDepth()
{
  this->m_type = Depth;
  this->m_binding = BindMinimal;
}


vesDepth::~vesDepth()
{
}


void vesDepth::bind(const vesRenderState &renderState)
{
  // Save current state.
  this->m_wasEnabled = glIsEnabled(GL_DEPTH_TEST);

  // Save current depth mask for restoration later.
  glGet(GL_DEPTH_WRITEMASK, &this->m_previousDepthWriteMask);

  if (this->m_enable) {
    glEnable(GL_DEPTH_TEST);
    glDepthMask((GLboolean) this->m_depthWriteMask);
  }
  else {
    glDisable(GL_DEPTH_TEST);
  }
}


void vesDepth::unbind(const vesRenderState &renderState)
{
  if (this->m_wasEnabled) {
      glEnable(GL_DEPTH_TEST);
  }
  else {
    glDisable(GL_DEPTH_TEST);
  }

  // Restore previous depth mask.
  glDepthMask(this->m_previousDepthWriteMask);

  this->setDirtyStateOff();
}
