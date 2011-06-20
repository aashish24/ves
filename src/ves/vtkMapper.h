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
#include "vtkGeometryNode.h"
#include "vtkBoundedObject.h"
#include "vtkGMTL.h"

class vtkMapper :public vtkGeometryNode, public vtkBoundedObject
{
public:
  vtkMapper(vtkFileReader* reader);
  ~vtkMapper();
  bool Read();
  void Reload(vtkFileReader *reader);
  vtkMatrix4x4f Eval();
  void Render(vtkPainter* render);
  void Render(vtkShaderProgram *program);
  vtkTriangleData* GetTriangleData();
  vtkTriangleData* GetData();
private:
  void ComputeBounds(vtkVector3f min, vtkVector3f max);
  void Normalize();
  vtkMatrix4x4f NormalizedMatrix;
protected:
  vtkFileReader *mFileReader;
  bool mIsNew;
  vtkTriangleData *Data;
  bool m_initialized;
  // unsigned int mMapperVBO[2];
};

#endif
