//
//  vesMapper.h
//  kiwi
//
//  Created by kitware on 5/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef __vesMapper_h
#define __vesMapper_h

#include "vesFileReader.h"
#include "vesShaderProgram.h"
#include "vsgGeometryNode.h"
#include "vsgBoundedObject.h"
#include "vesGMTL.h"

class vesMapper :public vsgGeometryNode, public vsgBoundedObject
{
public:
  vesMapper(vesFileReader* reader);
  ~vesMapper();
  bool Read();
  void Reload(vesFileReader *reader);
  vesMatrix4x4f Eval();
  void Render(Painter* render);
  void Render(vesShaderProgram *program);
  vesTriangleData* GetTriangleData();
  vesTriangleData* GetData();
  void ComputeBounds();
  vesGetMacro(Min, vesVector3f)
  vesGetMacro(Max, vesVector3f)
  void SetColor(float r, float g, float b, float a);
  vesGetMacro(Red, float)
  vesGetMacro(Green, float)
  vesGetMacro(Blue, float)
  vesGetMacro(Alpha, float)
private:
  void Normalize();
  vesMatrix4x4f NormalizedMatrix;
protected:
  float Red,Green,Blue,Alpha;
  vesFileReader *mFileReader;
  bool mIsNew;
  vesTriangleData *Data;
  bool m_initialized;

  // unsigned int mMapperVBO[2];
};

#endif
