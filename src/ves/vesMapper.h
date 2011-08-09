//
//  vesMapper.h
//  kiwi
//
//  Created by kitware on 5/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef __vesMapper_h
#define __vesMapper_h

#include "vesShaderProgram.h"
#include "vsgGeometryNode.h"
#include "vsgBoundedObject.h"
#include "vesGMTL.h"
#include "vesTriangleData.h"

class vesMapper :public vsgGeometryNode, public vsgBoundedObject
{
public:
  vesMapper();
  ~vesMapper();
  bool Read();
  vesMatrix4x4f Eval();
  void Render(Painter* render);
  void Render(vesShaderProgram *program);
  void SetTriangleData(vesTriangleData* data);
  vesTriangleData* GetTriangleData();
  vesTriangleData* GetData();
  void ComputeBounds();
  void SetColor(float r, float g, float b, float a);
  vesGetMacro(Red, float)
  vesGetMacro(Green, float)
  vesGetMacro(Blue, float)
  vesGetMacro(Alpha, float)
  vesGetMacro(DrawPoints, bool)
  vesSetMacro(DrawPoints, bool)
private:
  void Normalize();
  vesMatrix4x4f NormalizedMatrix;
protected:
  float Red,Green,Blue,Alpha;
  bool mIsNew;
  vesTriangleData *Data;
  bool m_initialized;
  bool DrawPoints;

  // unsigned int mMapperVBO[2];
};

#endif
