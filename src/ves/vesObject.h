#ifndef VESOBJECT_H
#define VESOBJECT_H

class vesObject
{
public:
  vesObject() :
    m_dirtyState(true)
  {
  }

  void        setDirty(const bool &value)
                                    { this->m_dirtyState = value; }
  void        setDirtyStateOn()     { this->setDirty(true); }
  void        setDirtyStateOff()    { this->setDirty(false); }
  bool        dirtyState()          { return this->m_dirtyState; }
  const bool& dirtyState() const    { return this->m_dirtyState; }

protected:

  bool m_dirtyState;
};


#endif // VESOBJECT_H
