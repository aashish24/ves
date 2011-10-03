
#include "vesMaterial.h"

// C++ includes
#include <map>


class vesMaterial::vesInternal
{
public:
  typedef std::map<vesMaterialAttribute::AttributeType,
    vesMaterialAttribute*> Attributes;

  Attributes m_attributes;
};


vesMaterial::vesMaterial() : vsgNode()
{
  this->m_internal = new vesInternal;
}


vesMaterial::~vesMaterial()
{
  delete this->m_internal; this->m_internal = 0x0;
}


bool vesMaterial::addAttribute(vesMaterialAttribute *attribute)
{
  if (!attribute)
  {
    return false;
  }

  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.find( attribute->type() );

  if (itr != this->m_internal->m_attributes.end() && ( (itr->second) != attribute )) {
    this->m_internal->m_attributes[attribute->type()] = attribute;
  }
}


vesMaterialAttribute* vesMaterial::attribute(vesMaterialAttribute::AttributeType type)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.find( type );

  if (itr != this->m_internal->m_attributes.end()) {
    return (itr->second);
  }

  return 0x0;
}


void vesMaterial::render(const vesRenderState &renderState)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (itr; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->activateGeneral(renderState);
  }
}


void vesMaterial::remove(const vesRenderState &renderState)
{
  // \todo: Implement this.
}


void vesMaterial::setupVertexSpecific(const vesRenderState &renderState)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (itr; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->setupVertexSpecific(renderState);
  }
}


void vesMaterial::activateVertexSpecific(const vesRenderState &renderState)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (itr; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->activateVertexSpecific(renderState);
  }
}


void vesMaterial::deActivateVertexSpecific(const vesRenderState &renderState)
{
  vesInternal::Attributes::iterator itr =
    this->m_internal->m_attributes.begin();

  for (itr; itr != this->m_internal->m_attributes.end(); ++itr) {
    itr->second->deActivateVertexSpecific(renderState);
  }
}
