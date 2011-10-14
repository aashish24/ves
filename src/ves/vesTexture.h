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

#ifndef __vesTexture_h
#define __vesTexture_h

#include "vesMaterial.h"

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"


class vesTexture : public vesMaterialAttribute
{
public:

  vesTexture();
  virtual ~vesTexture();

  virtual void bind         (const vesRenderState &renderState);
  virtual void unbind       (const vesRenderState &renderState);
  virtual void setup        (const vesRenderState &renderState);

  void setImageData(SFImage image);

  void setTextureUnit(unsigned int unit);
  unsigned int textureUnit()       { return this->m_textureUnit; }
  unsigned int textureUnit() const { return this->m_textureUnit; }

  void setWidth(int width);
  int  width()       { return this->m_width; }
  int  width() const { return this->m_width; }

  void setHeight(int height);
  int  height()       { return this->m_height; }
  int  height() const { return this->m_height; }

  void setDepth(int depth);
  int  depth()       { return this->m_depth; }
  int  depth() const { return this->m_depth; }

  unsigned int textureHandle()        { return this->m_textureHandle; }
  unsigned int textureHandle() const  { return this->m_textureHandle; }

protected:

  SFImage      m_image;

  int          m_width;
  int          m_height;
  int          m_depth;

  unsigned int m_textureHandle;
  unsigned int m_textureUnit;

private:

};
#endif // __vesTexture_h
