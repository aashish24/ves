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

#ifndef __vesSetGet_h
#define __vesSetGet_h

#define vesSetMacro(name,type)                  \
  virtual void Set##name (type _arg)            \
  {                                             \
    if (this->name != _arg)                     \
      {                                         \
        this->name = _arg;                      \
      }                                         \
  }

#define vesGetMacro(name,type)                  \
  virtual type Get##name ()                     \
  {                                             \
    return this->name;                          \
  }

#define vesSetGetMacro(name,type)               \
  virtual void Set##name (type _arg)            \
  {                                             \
    if (this->name != _arg)                     \
      {                                         \
        this->name = _arg;                      \
      }                                         \
  }                                             \
                                                \
  virtual type Get##name ()                     \
  {                                             \
    return this->name;                          \
  }

#define vesSetVectorMacro(name,type)            \
  void Set##name(const std::vector<type> &_arg) \
  {                                             \
    for(int i =0; i< _arg.size(); i++)          \
      {                                         \
        this->name.push_back( _arg[i]);         \
      }                                         \
  }

#define vesGetVectorMacro(name,type)            \
  bool Get##name (std::vector<type> *_arg)      \
  {                                             \
    if(this->name.size() ==0)                   \
      return false;                             \
    for(int ii=0; ii < this->name.size(); ii++) \
      {                                         \
        _arg->push_back(this->name[ii]);        \
      }                                         \
    return true;                                \
  }

#define vesSetGetVectorMacro(name,type)         \
  void Set##name(const std::vector<type> &_arg) \
  {                                             \
    for(int i =0; i< _arg.size(); i++)          \
      {                                         \
        this->name.push_back( _arg[i]);         \
      }                                         \
  }                                             \
                                                \
  bool Get##name (std::vector<type> *_arg)      \
  {                                             \
    if(this->name.size() ==0)                   \
      return false;                             \
    for(int ii=0; ii < this->name.size(); ii++) \
      {                                         \
        _arg->push_back(this->name[ii]);        \
      }                                         \
    return true;                                \
  }

#endif
