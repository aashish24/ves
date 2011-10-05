#ifndef VESVERTEXATTRIBUTE_H
#define VESVERTEXATTRIBUTE_H

// Base class
#include "vesMaterial.h"

// VES includes
#include "vesRenderStage.h"
#include "vesShaderProgram.h"
#include "vesTriangleData.h"

// C++ includes
#include <string>

#ifdef ANDROID
# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
#else
# include <OpenGLES/ES2/gl.h>
# include <OpenGLES/ES2/glext.h>
#endif

class vesVertexAttribute : public vesMaterialAttribute
{
public:

  vesVertexAttribute(const std::string &name)
  {
    this->m_name = name;
  }

  const std::string& name() const { return this->m_name; }

  virtual void update(const vesRenderState &renderState,
                      const vesShaderProgram &shaderProgram){;}

protected:
  std::string m_name;

};


class vesPositionVertexAttribute : public vesVertexAttribute
{
public:

  vesPositionVertexAttribute(const std::string &name="vertexPosition") :
    vesVertexAttribute(name)
  {
  }


  virtual void bindVertexData(const vesRenderState &renderState)
  {
    vesShaderProgram *program =
      static_cast<vesShaderProgram*>( renderState.m_material->attribute(
      vesMaterialAttribute::Shader) );

    glVertexAttribPointer(program->attributeLocation(this->m_name), 3, GL_FLOAT,
                          GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(program->attributeLocation(this->m_name));
  }


  virtual void unbindVertexData(const vesRenderState &renderState)
  {
      vesShaderProgram *program =
        static_cast<vesShaderProgram*>( renderState.m_material->attribute(
        vesMaterialAttribute::Shader) );

    glDisableVertexAttribArray(program->attributeLocation(this->m_name));
  }
};


class vesNormalVertexAttribute : public vesVertexAttribute
{
public:

  vesNormalVertexAttribute(const std::string &name="vertexNormal") :
    vesVertexAttribute(name)
  {
  }

  virtual void bindVertexData(const vesRenderState &renderState)
  {
    vesShaderProgram *program =
      static_cast<vesShaderProgram*>( renderState.m_material->attribute(
      vesMaterialAttribute::Shader) );

    glVertexAttribPointer(program->attributeLocation(this->m_name), 3,
                          GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(program->attributeLocation(this->m_name));
  }


  virtual void unbindVertexData(const vesRenderState &renderState)
  {
      vesShaderProgram *program =
        static_cast<vesShaderProgram*>( renderState.m_material->attribute(
        vesMaterialAttribute::Shader) );

    glDisableVertexAttribArray(program->attributeLocation(this->m_name));
  }
};

#endif // VESVERTEXATTRIBUTE_H
