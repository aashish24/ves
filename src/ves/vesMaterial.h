#ifndef VESMATERIAL_H
#define VESMATERIAL_H

#include "vsg/Core/vsgNode.h"

// Forward declarations
class vesRenderState;

class vesMaterialAttribute : public vsgNode
{
public:
  enum AttributeType
  {
    Shader    = 0x1,
    Texture   = 0x2
  };

  virtual ~vesMaterialAttribute()
  {
  }

  AttributeType type()       { return this->m_type; }
  AttributeType type() const { return this->m_type; }

  //  \todo: Fix not used warning
  virtual void bind         (const vesRenderState &renderState){}
  virtual void unbind       (const vesRenderState &renderState){}
  virtual void setup        (const vesRenderState &renderState){}

  virtual void bindVertexData   (const vesRenderState &renderState){}
  virtual void unbindVertexData (const vesRenderState &renderState){}
  virtual void setupVertexData  (const vesRenderState &renderState){}

protected:

  AttributeType m_type;
};


class vesMaterial : public vsgNode
{
public:

           vesMaterial();
  virtual ~vesMaterial();

  void setBinNumber(int number) { this->m_binNumber = number; }
  int  binNumber() { return this->m_binNumber; }
  const int  binNumber() const { return this->m_binNumber; }

  bool addAttribute(vesMaterialAttribute *attribute);
  vesMaterialAttribute* attribute(vesMaterialAttribute::AttributeType type);

  virtual void render(const vesRenderState &renderState);
  virtual void remove(const vesRenderState &renderState);

  virtual void bind         (const vesRenderState &renderState);
  virtual void unbind       (const vesRenderState &renderState);
  virtual void setup        (const vesRenderState &renderState);

  virtual void bindVertexData   (const vesRenderState &renderState);
  virtual void unbindVertexData (const vesRenderState &renderState);
  virtual void setupVertexData  (const vesRenderState &renderState);


protected:

  int m_binNumber;

  class vesInternal;
  vesInternal *m_internal;
};


#endif // VESMATERIAL_H
