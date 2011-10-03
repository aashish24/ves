#ifndef VESNORMALMATRIXUNIFORM_H
#define VESNORMALMATRIXUNIFORM_H

// Base class
#include "vesUniform.h"

// VES includes
#include "vesGMTL.h"

class vesModelViewUniform : public vesUniform
{
public:

  vesModelViewUniform(const std::string &name="normalMatrix") :
    vesUniform(name, vesMatrix3x3f)
  {
  }


  virtual void update(const vesRenderState &renderState, const vesShaderProgram &program)
  {
    vesMatrix3x3f normalMatrix =
      makeNormalMatrix3x3f(makeTransposeMatrix4x4(makeInverseMatrix4x4 (mv)));

    this->set(program.location(this->m_name), normalMatrix);
  }
};



#endif // VESNORMALMATRIXUNIFORM_H
