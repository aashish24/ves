#ifndef VESINTEGERUNIFORM_H
#define VESINTEGERUNIFORM_H

#include "vesUniform.h"

// VES includes
#include "vesMapper.h"
#include "vesRenderState.h"
#include "vesTriangleData.h"

// C++ includes
#include <string>

class vesIntegerUniform : public vesUniform
{
public:

  vesIntegerUniform(const std::string &name="integerUniform") :
    vesUniform(name, 1)
  {
  }


  virtual void update(const vesRenderState &renderState,
                      const vesShaderProgram &program)
  {
    // \note: Do nothing.
  }
};

#endif // VESINTEGERUNIFORM_H
