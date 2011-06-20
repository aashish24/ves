//
//  vtkSetGet.h
//  kiwi
//
//  Created by kitware on 6/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef __vtkSetGet_h
#define __vtkSetGet_h

#define vtkSetMacro(name,type)                  \
  virtual void Set##name (type _arg)            \
  {                                             \
    if (this->name != _arg)                     \
      {                                         \
        this->name = _arg;                      \
      }                                         \
  }

#define vtkGetMacro(name,type)                  \
  virtual type Get##name ()                     \
  {                                             \
    return this->name;                          \
  }

#define vtkSetGetMacro(name,type)               \
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

#define vtkSetVectorMacro(name,type)            \
  void Set##name(const std::vector<type> &_arg) \
  {                                             \
    for(int i =0; i< _arg.size(); i++)          \
      {                                         \
        this->name.push_back( _arg[i]);         \
      }                                         \
  }

#define vtkGetVectorMacro(name,type)            \
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

#define vtkSetGetVectorMacro(name,type)         \
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
