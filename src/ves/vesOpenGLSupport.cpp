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

#include "vesOpenGLSupport.h"

// VES includes
#include "vesGL.h"

// C++ includes
#include <cassert>
#include <sstream>

vesOpenGLSupport::vesOpenGLSupport() :
  m_initialized(false)
{
}


vesOpenGLSupport::~vesOpenGLSupport()
{
  this->m_extensionList.clear();
}


void vesOpenGLSupport::initialize()
{
  if(this->m_initialized) {
    return;
  }

  // Get version string
  const GLubyte* glVer = glGetString(GL_VERSION);
  assert(glVer && "Failed to retrive GL version");

  // \TODO Extract exact version number
  this->m_version = reinterpret_cast<const char*>(glVer);

  // Get vendor
  const GLubyte* glVen = glGetString(GL_VENDOR);
  this->m_vendor = reinterpret_cast<const char*>(glVen);

  // Set extension list
  std::stringstream ext;
  std::string str;

  const GLubyte* exts = glGetString(GL_EXTENSIONS);
  assert(exts && "Failed to query GL extensions");

  ext << exts;
  while (ext >> str) {

    this->m_extensionList.insert(str);
  }

  this->m_initialized = true;
}


bool vesOpenGLSupport::isSupported(const std::string &ext) const
{
  if (this->m_extensionList.find(ext) == this->m_extensionList.end()) {
    return false;
  }

  return true;
}


bool vesOpenGLSupport::isSupportedIndexUnsignedInt() const
{
  return this->isSupported("GL_OES_element_index_uint");
}
