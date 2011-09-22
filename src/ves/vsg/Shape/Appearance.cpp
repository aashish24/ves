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

#include "vsg/Shape/Appearance.h"

#include "vsg/Shape/vsgAppearanceChildNode.h"

#include "vsg/vsgVisitor.h"
#include "Painter.h"

namespace vsg {
  Appearance::Appearance()
  {
  }


  Appearance::~Appearance()
  {
  }


  void Appearance::addAttribute(vsgAppearanceChildNode* attribute)
  {
    m_attributes.push_back(attribute);
  }


  vsgAppearanceChildNode* Appearance::attribute(unsigned int type)
  {
    return m_attributes[0];
  }


  bool Appearance::accept(vsgVisitor* vsgVisitor)
  {
    return vsgVisitor->visitAppearance(this);
  }


  void Appearance::render(Painter* render)
  {
    m_attributes[0]->render(render);
  }

  bool Appearance::read()
  {
  //  this->_shaders[0]->read();
    return true;
  }
}
