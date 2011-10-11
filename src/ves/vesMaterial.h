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

// Forward declarations
class vesRenderData;
class vesRenderState;
class vesShaderProgram;

class vesMaterialAttribute
{
public:
  enum AttributeType
  {
    Undefined = 0x0,
    Shader    = 0x1,
    Texture   = 0x2
  };

  vesMaterialAttribute() :
    m_dirtyState(true),
    m_type      (Undefined)
  {
  }


  virtual ~vesMaterialAttribute()
  {
  }


  void        setDirty(const bool &value)
                                    { this->m_dirtyState = value; }
  void        setDirtyStateOn()     { this->setDirty(true); }
  void        setDirtyStateOff()    { this->setDirty(false); }
  bool        dirtyState()          { return this->m_dirtyState; }
  const bool& dirtyState() const    { return this->m_dirtyState; }


  AttributeType type()       { return this->m_type; }
  AttributeType type() const { return this->m_type; }

  //  \note: Fix (unused parameter warning)
  virtual void bind         (const vesRenderState &renderState){}
  virtual void unbind       (const vesRenderState &renderState){}
  virtual void setup        (const vesRenderState &renderState){}

  virtual void bindVertexData   (const vesRenderState &renderState, int key){}
  virtual void unbindVertexData (const vesRenderState &renderState, int key){}
  virtual void setupVertexData  (const vesRenderState &renderState, int key){}

  virtual void bindRenderData   (const vesRenderState &renderState,
                                 const vesRenderData  &renderData){}

protected:

  bool          m_dirtyState;

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

  void        setBinNumber(int number) { this->m_binNumber = number; }
  int         binNumber()              { return this->m_binNumber; }
  const int&  binNumber() const        { return this->m_binNumber; }

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

  virtual void bindRenderData   (const vesRenderState &renderState,
                                 const vesRenderData  &renderData);

protected:

  int m_binNumber;

  vesShaderProgram *m_shaderProgram;

  class vesInternal;
  vesInternal *m_internal;
};


#endif // VESMATERIAL_H
