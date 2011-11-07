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

#ifndef VESMATERIAL_H
#define VESMATERIAL_H

#include "vesObject.h"

// VES includes
#include "vesSetGet.h"
#include "vesSharedPtr.h"

// Forward declarations
class vesRenderData;
class vesRenderState;
class vesShaderProgram;

class vesMaterialAttribute : public vesObject
{
public:
  enum AttributeType
  {
    Undefined = 0x0,
    Shader = 0x1,
    Texture = 0x2,
    Blend = 0x3,
    Depth = 0x4
  };


  enum AttributeBinding
  {
    BindAll = 0,
    BindMinimal = 1,
    BindVertexData = 2,
    BindRenderData = 3
  };


  vesMaterialAttribute() : vesObject(),
    m_enable(true),
    m_type(Undefined),
    m_binding(BindMinimal)
  {
  }


  virtual ~vesMaterialAttribute()
  {
  }

  inline void enable() { this->m_enable = true; }
  inline void disable() { this->m_enable = false; }
  inline bool isEnabled() const { return this->m_enable; }

  bool setType(AttributeType type) { this->m_type = type; return true; }
  AttributeType type() { return this->m_type; }
  AttributeType type() const { return this->m_type; }

  /// Define what sort of bind calls required by the attribute
  ///Should be set at the time of creation of the attribute.
  bool setBinding(AttributeBinding binding) { this->m_binding = binding; return true; }
  AttributeBinding binding() { return this->m_binding; }
  AttributeBinding binding() const { return this->m_binding; }

  //  \note: Fix (unused parameter warning)
  virtual void bind(const vesRenderState &renderState){}
  virtual void unbind(const vesRenderState &renderState){}
  virtual void setup(const vesRenderState &renderState){}

  virtual void bindVertexData(const vesRenderState &renderState, int key){}
  virtual void unbindVertexData(const vesRenderState &renderState, int key){}
  virtual void setupVertexData(const vesRenderState &renderState, int key){}

  virtual void bindRenderData(const vesRenderState &renderState,
                              const vesRenderData  &renderData){}

protected:
  bool m_enable;
  AttributeType m_type;
  AttributeBinding m_binding;
};


class vesMaterial
{
public:
  enum RenderHint
  {
    DefaultBin      = 0,
    OpaqueBin       = 1,
    TransparentBin  = 2
  };


  enum RenderBin
  {
    Default     = 0,
    Opaque      = 1,
    Transparent = 10,
    Overlay     = 20
  };

  vesMaterial();
  virtual ~vesMaterial();

  void setBinNumber(int number) { this->m_binNumber = number; }
  int binNumber() { return this->m_binNumber; }
  const int& binNumber() const { return this->m_binNumber; }

  bool addAttribute(vesSharedPtr<vesMaterialAttribute> attribute);
  vesSharedPtr<vesMaterialAttribute> attribute(
    vesMaterialAttribute::AttributeType type);
  const vesSharedPtr<vesMaterialAttribute> attribute(
    vesMaterialAttribute::AttributeType type) const;

  /*! Provide quick acces to shader program */
  bool setShaderProgram(vesSharedPtr<vesShaderProgram> shaderProgram);
  vesSharedPtr<vesShaderProgram> shaderProgram()
    { return this->m_shaderProgram; }
  const vesSharedPtr<vesShaderProgram> shaderProgram() const
    { return this->m_shaderProgram; }

  virtual void render(const vesRenderState &renderState);
  virtual void remove(const vesRenderState &renderState);

  virtual void bind(const vesRenderState &renderState);
  virtual void unbind(const vesRenderState &renderState);
  virtual void setup(const vesRenderState &renderState);

  virtual void bindVertexData(const vesRenderState &renderState, int key);
  virtual void unbindVertexData(const vesRenderState &renderState, int key);
  virtual void setupVertexData(const vesRenderState &renderState, int key);

  virtual void bindRenderData(const vesRenderState &renderState,
                              const vesRenderData  &renderData);

protected:

  int m_binNumber;

  vesSharedPtr<vesShaderProgram> m_shaderProgram;

  class vesInternal;
  vesInternal *m_internal;
};


#endif // VESMATERIAL_H
