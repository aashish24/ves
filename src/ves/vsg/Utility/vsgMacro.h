// ============================================================================
/**
 * @file   vsgMacro.h
 *
 * @section COPYRIGHT
 *
 * Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 * All rights reserved.
 * See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 *
 *   This software is distributed WITHOUT ANY WARRANTY; without even
 *   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *   PURPOSE.  See the above copyright notice for more information.
 *
 * @section DESCRIPTION
 *
 * This file constains all the macro definitions used in VTK Scene Graph
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgMacro_h
#define __vsgMacro_h
// --------------------------------------------------------------------includes

// -----------------------------------------------------------------------macro
#define InOnlySF(name,type)                     \
  private:                                      \
  type _##name ;                                \
public:                                         \
virtual void set_##name (type _arg)             \
{                                               \
  _##name = _arg;                               \
}

// -----------------------------------------------------------------------macro
#define OutOnlySF(name,type)                    \
  private:                                      \
  type _##name;                                 \
  bool name##Changed;                           \
public:                                         \
virtual type get_##name()                       \
{                                               \
  return _##name;                               \
}                                               \
                                                \
virtual bool name##_changed ()                  \
{                                               \
  if(name##Changed)                             \
    {                                           \
      name##Changed =false;                     \
      return true;                              \
    }                                           \
  else                                          \
    {                                           \
      return false;                             \
    }                                           \
}

// -----------------------------------------------------------------------macro
#define InOutSF(name,type)                      \
  private:                                      \
  type _##name ;                                \
  bool name##Changed ;                          \
public:                                         \
virtual void set_##name (type _arg)             \
{                                               \
  _##name = _arg;                               \
  name##Changed = true;                         \
}                                               \
                                                \
virtual type get_##name()                       \
{                                               \
  return _##name;                               \
}                                               \
                                                \
virtual bool name##_changed ()                  \
{                                               \
  if(name##Changed)                             \
    {                                           \
      name##Changed =false;                     \
      return true;                              \
    }                                           \
  else                                          \
    {                                           \
      return false;                             \
    }                                           \
}

// -----------------------------------------------------------------------macro
#define InOnlyMF(name,type)                     \
  private:                                      \
  type _##name;                                 \
public:                                         \
virtual void set_##name ( const type &_arg)     \
{                                               \
  _##name = _arg;                               \
}

// -----------------------------------------------------------------------macro
#define OutOnlyMF(name,type)                    \
  private:                                      \
  type _##name;                                 \
  bool name##Changed;                           \
public:                                         \
virtual bool name##_changed (type &_arg)        \
{                                               \
  if(name##Changed)                             \
    {                                           \
      name##Changed =false;                     \
      _arg = _##name;                           \
      return true;                              \
    }                                           \
  else                                          \
    {                                           \
      return false;                             \
    }                                           \
}

// -----------------------------------------------------------------------macro
#define InOutMF(name,type)                      \
  private:                                      \
  type _##name;                                 \
  bool name##Changed;                           \
public:                                         \
virtual void set_##name ( const type _arg)      \
{                                               \
  for(int i =0; i< _arg.size(); ++i)            \
    {                                           \
      _##name.push_back(_arg[i]);               \
    }                                           \
  if(_arg.size())                               \
    name##Changed = true;                       \
}                                               \
                                                \
virtual const type& get_##name ()               \
{                                               \
  return _##name;                               \
}                                               \
                                                \
virtual bool name##_changed (type &_arg)        \
{                                               \
  if(name##Changed)                             \
    {                                           \
      name##Changed =false;                     \
      return true;                              \
    }                                           \
  else                                          \
    {                                           \
      return false;                             \
    }                                           \
}

// -----------------------------------------------------------------------macro
#define InOutMFNode(name)                       \
  private:                                      \
  MFNode _##name;                               \
  bool name##Changed;                           \
public:                                         \
virtual void set_##name ( const type _arg)      \
{                                               \
  for(int i =0; i< _arg.size(); ++i)            \
    {                                           \
      _##name.push_back(_arg[i]);               \
    }                                           \
  if(_arg.size())                               \
    name##Changed = true;                       \
}                                               \
                                                \
virtual bool name##_changed (type &_arg)        \
{                                               \
  if(name##Changed)                             \
    {                                           \
      name##Changed =false;                     \
      _arg = _##name;                           \
      return true;                              \
    }                                           \
  else                                          \
    {                                           \
      return false;                             \
    }                                           \
}

// -----------------------------------------------------------------------macro
#define vsgSetVectorMacro(name,type)            \
  void Set##name(const std::vector<type> &_arg) \
  {                                             \
    for(int i =0; i< _arg.size(); i++)          \
      {                                         \
        this->name.push_back( _arg[i]);         \
      }                                         \
  }

// -----------------------------------------------------------------------macro
#define vsgGetVectorMacro(name,type)            \
  bool get##name (std::vector<type> *_arg)      \
  {                                             \
    if(this->name.size() ==0)                   \
      return false;                             \
    for(int ii=0; ii < this->name.size(); ii++) \
      {                                         \
        _arg->push_back(this->name[ii]);        \
      }                                         \
    return true;                                \
  }

// -----------------------------------------------------------------------macro
#define vsgSetGetVectorMacro(name,type)         \
  void set##name(const std::vector<type> &_arg) \
  {                                             \
    for(int i =0; i< _arg.size(); i++)          \
      {                                         \
        this->name.push_back( _arg[i]);         \
      }                                         \
  }                                             \
                                                \
  bool get##name (std::vector<type> *_arg)      \
  {                                             \
    if(this->name.size() ==0)                   \
      return false;                             \
    for(int ii=0; ii < this->name.size(); ii++) \
      {                                         \
        _arg->push_back(this->name[ii]);        \
      }                                         \
    return true;                                \
  }

// -----------------------------------------------------------------------macro
#define vsgSetSFMacro(name,type)                \
  virtual void set##name (SF##type _arg)        \
  {                                             \
    if (this->name != _arg)                     \
      {                                         \
        this->name = _arg;                      \
      }                                         \
  }

// -----------------------------------------------------------------------macro
#define vsgGetSFMacro(name,type)                \
  virtual SF##type get##name ()                 \
  {                                             \
    return this->name;                          \
  }

// -----------------------------------------------------------------------macro
#define vsgSetGetSFMacro(name,type)             \
  virtual void set##name (SF##type _arg)        \
  {                                             \
    if (this->name != _arg)                     \
      {                                         \
        this->name = _arg;                      \
      }                                         \
  }                                             \
                                                \
  virtual type get##name ()                     \
  {                                             \
    return this->name;                          \
  }

// -----------------------------------------------------------------------macro
#define vsgSetMFMacro(name,type)                        \
  void Set##name(const std::vector<SF##type> &_arg)     \
  {                                                     \
    for(int i =0; i< _arg.size(); i++)                  \
      {                                                 \
        this->name.push_back( _arg[i]);                 \
      }                                                 \
  }

// -----------------------------------------------------------------------macro
#define vsgGetMFMacro(name,type)                \
  bool Get##name (std::vector<SF##type> &_arg)  \
  {                                             \
    if(this->name.size() ==0)                   \
      return false;                             \
    for(int ii=0; ii < this->name.size(); ii++) \
      {                                         \
        _arg.push_back(this->name[ii]);         \
      }                                         \
    return true;                                \
  }

// -----------------------------------------------------------------------macro
#define vsgSetGetMFMacro(name,type)                     \
  void Set##name(const std::vector<SF##type> &_arg)     \
  {                                                     \
    for(int i =0; i< _arg.size(); i++)                  \
      {                                                 \
        this->name.push_back( _arg[i]);                 \
      }                                                 \
  }                                                     \
                                                        \
  bool Get##name (std::vector<SF##type> &_arg)          \
  {                                                     \
    if(this->name.size() ==0)                           \
      return false;                                     \
    for(int ii=0; ii < this->name.size(); ii++)         \
      {                                                 \
        _arg.push_back(this->name[ii]);                 \
      }                                                 \
    return true;                                        \
  }

#endif // __vsgMacro_h
