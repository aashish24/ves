#ifndef VESENGINEUNIFORM_H
#define VESENGINEUNIFORM_H

#include "vesBooleanUniform.h"
#include "vesIntegerUniform.h"
#include "vesRenderData.h"

class vesEngineUniform
{
public:
  vesEngineUniform() :
    m_uniform(0x0)
  {
  }


  virtual ~vesEngineUniform()
  {
    delete this->m_uniform;
  }


  virtual void bindRenderData(const vesRenderState &renderState,
                              const vesRenderData  &renderData){}


  vesUniform* uniform(){ return this->m_uniform; }


protected:

  vesUniform *m_uniform;
};


class vesHasVertexColors : public vesEngineUniform
{
public:
  vesHasVertexColors() : vesEngineUniform()
  {
    this->m_uniform = new vesHasVertexColorsUniform();
  }
};


class vesPrimitiveType : public vesEngineUniform
{
public:

  vesPrimitiveType() : vesEngineUniform()
  {
    this->m_uniform = new vesIntegerUniform("primitiveType");
  }


  virtual void bindRenderData(const vesRenderState &renderState,
                              const vesRenderData  &renderData)
  {
    this->m_uniform->set   (renderData.m_pritimiveType);
    this->m_uniform->callGL(renderState.m_material->shaderProgram()->
                            uniformLocation(this->m_uniform->name()));
  }

};

#endif // VESENGINEUNIFORM_H
