//
//  vesSetGet.h
//  kiwi
//
//  Created by kitware on 6/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

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
        _arg->push_back(this->name[ii]);         \
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
        _arg->push_back(this->name[ii]);         \
      }                                         \
    return true;                                \
  }


#endif
