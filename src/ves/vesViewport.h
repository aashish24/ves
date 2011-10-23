#ifndef vesViewport_H
#define vesViewport_H

#include "vesObject.h"

// VES includes.
class vesRenderState;

class vesViewport : vesObject
{
public:
  vesViewport();
  vesViewport(int x, int y, int width, int height);

  inline void setViewport(int x, int y, int width, int height)
  {
    this->m_x = x;
    this->m_y = y;
    this->m_width = width;
    this->m_height = height;
  }

  double aspect() const;
  double inverseAspect() const;

  inline int x() const { return this->m_x; }
  inline int y() const { return this->m_y; }
  inline int width() const { return this->m_width; }
  inline int height() const { return this->m_height; }

  virtual void render(vesRenderState &renderState);

protected:
  virtual ~vesViewport();

  int m_x;
  int m_y;
  int m_width;
  int m_height;

private:
  /// Not implemented.
  vesViewport(const vesViewport&);

  /// Not implemented.
  void operator=(const vesViewport&);
};


#endif // vesViewport_H
