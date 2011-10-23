#ifndef VESDEPTH_H
#define VESDEPTH_H

#include "vesMaterial.h"

class vesDepth : public vesMaterialAttribute
{
public:
  vesDepth();
  virtual ~vesDepth();

  inline void setWriteMask(bool mask) { this->m_depthWriteMask = mask; }
  inline bool writeMask() const { return this->m_depthWriteMask; }

  virtual void bind (const vesRenderState &renderState);
  virtual void unbind (const vesRenderState &renderState);


protected:
  bool m_wasEnabled;
  bool m_previousDepthWriteMask;
  bool m_depthWriteMask;
};

#endif // VESDEPTH_H
