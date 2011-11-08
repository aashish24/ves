#ifndef VESCOLORUNIFORM_H
#define VESCOLORUNIFORM_H

#include "vesUniform.h"

// VES includes
#include "vesGMTL.h"
#include "vesRenderState.h"
#include "vesSetGet.h"

// C++ includes
#include <string>

class vesColorUniform : public vesUniform
{
public:
  vesColorUniform(const std::string &name="colorUniform") :
    vesUniform(name, vesVector4f())
  {
  }


  virtual void update(const vesRenderState   &renderState,
                      const vesShaderProgram &program)
  {
    // \note: Do nothing.
  }
};

#endif // VESCOLORUNIFORM_H
