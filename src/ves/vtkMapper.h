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
#include "vesGMTL.h"

class vtkMapper :public vtkGeometryNode, public vtkBoundedObject
{
public:
  vtkMapper(vtkFileReader* reader);
  ~vtkMapper();
  bool Read();
  void Reload(vtkFileReader *reader);
  vesMatrix4x4f Eval();
  void Render(vtkPainter* render);
  void Render(vtkShaderProgram *program);
  vtkTriangleData* GetTriangleData();
  vtkTriangleData* GetData();
  void ComputeBounds();
  vesGetMacro(Min, vesVector3f)
  vesGetMacro(Max, vesVector3f)
  
private:
  void Normalize();
  vesMatrix4x4f NormalizedMatrix;
protected:
  vtkFileReader *mFileReader;
  bool mIsNew;
  vtkTriangleData *Data;
  bool m_initialized;
  // unsigned int mMapperVBO[2];
};

#endif
