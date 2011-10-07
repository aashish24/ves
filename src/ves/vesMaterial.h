#ifndef VESMATERIAL_H
#define VESMATERIAL_H

// Forward declarations
class vesRenderState;
class vesShaderProgram;

class vesMaterialAttribute
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

  virtual void bindVertexData   (const vesRenderState &renderState, int key){}
  virtual void unbindVertexData (const vesRenderState &renderState, int key){}
  virtual void setupVertexData  (const vesRenderState &renderState, int key){}

protected:

  AttributeType m_type;
};


class vesMaterial
{
public:
  enum RenderHint
  {
    DefaultBin    = 0,
    OpaqueBin     = 1,
    TransparentBin =2
  };


  enum RenderBin
  {
    Default     = 0,
    Opaque      = 1,
    Transparent = 10
  };

           vesMaterial();
  virtual ~vesMaterial();

  void setBinNumber(int number) { this->m_binNumber = number; }
  int  binNumber() { return this->m_binNumber; }
  const int  binNumber() const { return this->m_binNumber; }

  bool addAttribute(vesMaterialAttribute *attribute);
  vesMaterialAttribute* attribute(vesMaterialAttribute::AttributeType type);

  /*! Provide quick acces to shader program */
  vesShaderProgram* shaderProgram()             { return this->m_shaderProgram; }
  const vesShaderProgram* shaderProgram() const { return this->m_shaderProgram; }

  virtual void render(const vesRenderState &renderState);
  virtual void remove(const vesRenderState &renderState);

  virtual void bind         (const vesRenderState &renderState);
  virtual void unbind       (const vesRenderState &renderState);
  virtual void setup        (const vesRenderState &renderState);

  virtual void bindVertexData   (const vesRenderState &renderState, int key);
  virtual void unbindVertexData (const vesRenderState &renderState, int key);
  virtual void setupVertexData  (const vesRenderState &renderState, int key);


protected:

  int m_binNumber;

  vesShaderProgram *m_shaderProgram;

  class vesInternal;
  vesInternal *m_internal;
};


#endif // VESMATERIAL_H
