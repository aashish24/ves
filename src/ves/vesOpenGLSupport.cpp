
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
