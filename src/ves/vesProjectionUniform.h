#ifndef VESPROJECTION_UNIFORM_H
#define VESPROJECTION_UNIFORM_H

// Base class
#include "vesUniform.h"

// VES includes
#include "vesRenderStage.h"

class vesProjectionUniform : public vesUniform
{
public:

  vesProjectionUniform(const std::string &name="projectionMatrix") :
    vesUniform(name, vesMatrix4x4f())
  {
  }


  virtual void update(const vesRenderState &renderState,
                      const vesShaderProgram &program)
  {
    this->set(*renderState.m_projectionMatrix);
  }
};

#endif // VESPROJECTION_UNIFORM_H
