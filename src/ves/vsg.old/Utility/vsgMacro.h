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

/**
 * @section DESCRIPTION
 *
 * This file constains all the macro definitions used in VTK Scene Graph
 */

#ifndef __vsgMacro_h
#define __vsgMacro_h

#define InOnlySF(name,type)                     \
  private:                                      \
  type _##name ;                                \
public:                                         \
virtual void set_##name (type _arg)             \
{                                               \
  _##name = _arg;                               \
}

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
virtual type get_##name() const                      \
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

#define InOnlyMF(name,type)                     \
  private:                                      \
  type _##name;                                 \
public:                                         \
virtual void set_##name ( const type &_arg)     \
{                                               \
  _##name = _arg;                               \
}

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

#define vsgSetVectorMacro(name,type)            \
  void Set##name(const std::vector<type> &_arg) \
  {                                             \
    for(int i =0; i< _arg.size(); i++)          \
      {                                         \
        this->name.push_back( _arg[i]);         \
      }                                         \
  }

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

#define vsgSetSFMacro(name,type)                \
  virtual void set##name (SF##type _arg)        \
  {                                             \
    if (this->name != _arg)                     \
      {                                         \
        this->name = _arg;                      \
      }                                         \
  }

#define vsgGetSFMacro(name,type)                \
  virtual SF##type get##name ()                 \
  {                                             \
    return this->name;                          \
  }

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

#define vsgSetMFMacro(name,type)                        \
  void Set##name(const std::vector<SF##type> &_arg)     \
  {                                                     \
    for(int i =0; i< _arg.size(); i++)                  \
      {                                                 \
        this->name.push_back( _arg[i]);                 \
      }                                                 \
  }

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
