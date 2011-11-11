
#include "vesViewport.h"

// VES includes.
#include "vesGL.h"

vesViewport::vesViewport()
{
  this->m_x = 0;
  this->m_y = 0;
  this->m_width = 600;
  this->m_height = 600;
}


vesViewport::vesViewport(int x, int y, int width, int height) :
  m_x(x),
  m_y(y),
  m_width(width),
  m_height(height)
{
}


vesViewport::~vesViewport()
{
}


double vesViewport::aspect() const
{
  return static_cast<double>(this->m_width) / this->m_height;
}


double vesViewport::inverseAspect() const
{
  return static_cast<double>(m_height) / this->m_width;
}


void vesViewport::render(vesRenderState &renderState)
{
  vesNotUsed(renderState);

  glViewport(static_cast<GLint>(this->m_x), static_cast<GLint>(this->m_y),
             static_cast<GLsizei>(this->m_width), static_cast<GLsizei>(this->m_height));
}


