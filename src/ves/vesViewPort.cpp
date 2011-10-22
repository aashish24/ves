
#include "vesViewport.h"

#include "vesGL.h"

vesViewport::vesViewport()
{
  this->m_x = 0;
  this->m_y = 0;
  this->m_width = 800;
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


void vesViewport::render(vesRenderState &renderState)
{
  glViewport(static_cast<GLint>(this->m_x), static_cast<GLint>(this->m_y),
             static_cast<GLsizei>(this->m_width), static_cast<GLsizei>(this->m_height));
}


