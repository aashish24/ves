#ifndef VESMATERIAL_H
#define VESMATERIAL_H

class vesMaterialAttribute : public vsgNode
{
public:
  enum AttributeType
  {
    Shader    = 0x1,
    Texture   = 0x2
  };
};


class vesMaterial : public vsgNode
{
public:

           vesMaterial();
  virtual ~vesMaterial();

  void addAttribute(vesMaterialAttribute *attribute);
  vesMaterialAttribute* attribute(vesMaterialAttribute::AttributeType type);

protected:

  class vesInternal;
  vesInternal *m_internal;
};


#endif // VESMATERIAL_H
