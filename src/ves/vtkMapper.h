//
//  vtkMapper.h
//  kiwi
//
//  Created by kitware on 5/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef __vtkMapper_h
#define __vtkMapper_h

#include "vtkFileReader.h"
#include "vtkShaderProgram.h"

class vtkMapper
{
public:
  vtkMapper(vtkFileReader* reader);
  ~vtkMapper();
  
  bool Read();
  void Reload(vtkFileReader *reader);
  void Print(vtkShaderProgram *program);
  
  vtkPoint3f GetMin();
  
  vtkPoint3f GetMax();
protected:
  
  vtkFileReader *mFileReader;
  bool mIsNew;

  vtkTriangleData *mData;
  bool m_initialized;
  unsigned int mMapperVBO[2];
};

#endif