#ifndef VESMODELVIEWUNIFORM_H
#define VESMODELVIEWUNIFORM_H

// Base class
#include "vesUniform.h"

// VES includes
#include "vesGMTL.h"
#include "vesRenderStage.h"

class vesModelViewUniform : public vesUniform
{
public:

  vesModelViewUniform(const std::string &name="modelViewMatrix") :
    vesUniform(name, vesMatrix4x4f)
  {
  }


  virtual void update(const vesRenderState &renderState, const vesShaderProgram &program)
  {
    this->set(program.location(this->m_name), renderState.m_modelViewMatrix);
  }
};


#endif // VESMODELVIEWUNIFORM_H
