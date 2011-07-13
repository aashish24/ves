/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vsgBoundedObject.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vsgBoundedObject - Specifies the bounding box for any dataset
// .SECTION Description
// vsgBoundedObject specifies the bounding volume for any data. It holds the
// coordinates of the center in local coordinate system and the sizes of the
// corresponding x,y,z sides.

#ifndef __vsgBoundedObject_h
#define __vsgBoundedObject_h

#include "vesGMTL.h"
#include "vesSetGet.h"

// -----------------------------------------------------------------------class
class vsgBoundedObject
{
public:
  // ............................................................public-methods
  vsgBoundedObject();
  ~vsgBoundedObject();
  virtual void ComputeBounds()=0;
  virtual vesVector3f GetMin()=0;
  virtual vesVector3f GetMax()=0;
  vesSetMacro(BBoxCenter,vesVector3f)
  vesGetMacro(BBoxCenter,vesVector3f)
  vesSetMacro(BBoxSize,vesVector3f)
  vesGetMacro(BBoxSize,vesVector3f)

  void SetBBoxCenter(vesVector3f min, vesVector3f max);
  void SetBBoxSize(vesVector3f min, vesVector3f max);
//  vesGetMacro(Min,vesVector3f)
//  vesGetMacro(Max,vesVector3f)
  virtual float GetBBoxRadius();
protected:
  vesVector3f Min;
  vesVector3f Max;
  vesVector3f BBoxCenter;
  vesVector3f BBoxSize;
};

#endif // __vsgBoundedObject_h
