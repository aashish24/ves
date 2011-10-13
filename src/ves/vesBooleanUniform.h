#ifndef VESBOOLEANUNIFORM_H
#define VESBOOLEANUNIFORM_H

#include "vesUniform.h"

// VES includes
#include "vesMapper.h"
#include "vesRenderState.h"
#include "vesTriangleData.h"

// C++ includes
#include <string>

class vesBooleanUniform : public vesUniform
{
public:

  vesBooleanUniform(const std::string &name="booleanUniform") :
    vesUniform(name, false)
  {
  }


  virtual void update(const vesRenderState &renderState,
                      const vesShaderProgram &program)
  {
    // \note: Do nothing.
  }
};


class vesHasVertexColorsUniform : public vesBooleanUniform
{
public:

  vesHasVertexColorsUniform(const std::string &name="hasVertexColors") :
    vesBooleanUniform(name)
  {
  }


  virtual void update(const vesRenderState &renderState,
                      const vesShaderProgram &program)
  {
    if (!renderState.m_mapper->data()->GetVertexColors().empty()) {
      this->set(true);
    }
    else
    {
      this->set(false);
    }
  }

  virtual vesUniform* asUniform()
  {
    return (static_cast<vesUniform*>(this));
  }
};

#endif // VESBOOLEANUNIFORM_H
