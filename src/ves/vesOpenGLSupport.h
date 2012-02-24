#ifndef VESOPENGLSUPPORT_H
#define VESOPENGLSUPPORT_H

#include "vesSetGet.h"

// C/C++ includes
#include <set>
#include <string>

class vesOpenGLSupport
{
public:
  static vesOpenGLSupport* instance();

  void initialize();

  const std::string& glVendor() const
  {
    return m_vendor;
  }

  const std::string& glVersion(void) const
  {
    return m_version;
  }

  bool isSupported(const std::string& ext) const;


private:
  vesOpenGLSupport();
  ~vesOpenGLSupport();

  static vesOpenGLSupport* m_instance;

  bool m_initialized;

  std::string m_version;
  std::string m_vendor;

  std::set<std::string> m_extensionList;
};

#endif // VESOPENGLSUPPORT_H
