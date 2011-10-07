/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

#ifndef VESSHADER_PROGRAM_H
#define VESSHADER_PROGRAM_H

// VES includes
#include "vesGMTL.h"
#include "vesMaterial.h"

// C++ includes
#include <string>

// Forward declarations
class vesShader;
class vesRenderState;
class vesUniform;
class vesVertexAttribute;

class vesShaderProgram : public vesMaterialAttribute
{
public:
            vesShaderProgram();
   virtual ~vesShaderProgram();

  /*!
   * Add shader to a program. For now lifetime of the shader is controlled
   * by the program.
   */
  bool addShader(vesShader *shader);

  bool addUniform(vesUniform *uniform);
  bool addVertexAttribute(vesVertexAttribute *attribute, int key);

  bool addBindAttributeLocation(const std::string &name, int location);

  int  uniformLocation  (const std::string &name) const;
  int  attributeLocation(const std::string &name) const;

  vesUniform* uniform     (const std::string &name);
  bool        uniformExist(const std::string &name);

  virtual void updateUniforms();

  bool link();

  bool validate();
  void use();

  void cleanUp();
  void deleteProgram();
  void deleteVertexAndFragment();

  unsigned int        programHandle();
  const unsigned int& programHandle() const;

  virtual void bind         (const vesRenderState &renderState);
  virtual void unbind       (const vesRenderState &renderState);
  virtual void setup        (const vesRenderState &renderState);

  virtual void bindVertexData   (const vesRenderState &renderState, int key);
  virtual void unbindVertexData (const vesRenderState &renderState, int key);
  virtual void setupVertexData  (const vesRenderState &renderState, int key);


protected:

  int  queryUniformLocation  (const std::string &value);
  int  queryAttributeLocation(const std::string &value);

  void bindAttributes();
  void bindUniforms();


private:

  class vesInternal;
  vesInternal *m_internal;

  vesShaderProgram(const vesShaderProgram&);
  void operator=  (const vesShaderProgram&);
};


#endif
