//
//  vtkSetGet.h
//  kiwi
//
//  Created by kitware on 6/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef __vtkSetGet_h
#define __vtkSetGet_h

#define vtkSetMacro(name,type) \
virtual void Set##name (type _arg) \
{ \
  if (this->name != _arg) \
  { \
    this->name = _arg; \
  } \
}

#define vtkGetMacro(name,type) \
virtual type Get##name () \
{ \
  return this->name; \
}


#define vtkSetGetMacro(name,type) \
virtual void Set##name (type _arg) \
{ \
if (this->name != _arg) \
{ \
this->name = _arg; \
} \
}\
\
virtual type Get##name () \
{ \
  return this->name; \
}


#endif