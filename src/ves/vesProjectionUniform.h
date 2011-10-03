#ifndef VESPROJECTIONUNIFORM_H
#define VESPROJECTIONUNIFORM_H

// Base class
#include "vesUniform.h"

// VES includes
#include "vesRenderStage.h"

class vesModelViewUniform : public vesUniform
{
public:

  vesModelViewUniform(const std::string &name="projectionMatrix") :
    vesUniform(name, vesMatrix4x4f)
  {
  }


  virtual void update(const vesRenderState &renderState, const vesShaderProgram &program)
  {
    this->set(program.location(this->m_name), renderState.m_projectionMatrix);
  }
};

#endif // VESPROJECTIONUNIFORM_H
