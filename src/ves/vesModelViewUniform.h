#ifndef VESMODELVIEWUNIFORM_H
#define VESMODELVIEWUNIFORM_H

// Base class
#include "vesUniform.h"

// VES includes
#include "vesGMTL.h"
#include "vesRenderStage.h"

// C++ includes
#include <string>

class vesModelViewUniform : public vesUniform
{
public:

  vesModelViewUniform(const std::string &name="modelViewMatrix") :
    vesUniform(name, vesMatrix4x4f())
  {
  }


  virtual void update(const vesRenderState &renderState, const vesShaderProgram &program)
  {
    this->set(*renderState.m_modelViewMatrix);
  }
};


#endif // VESMODELVIEWUNIFORM_H
