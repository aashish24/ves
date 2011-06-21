/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBoundedObject.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkBoundedObject - Specifies the bounding box for any dataset
// .SECTION Description
// vtkBoundedObject specifies the bounding volume for any data. It holds the
// coordinates of the center in local coordinate system and the sizes of the
// corresponding x,y,z sides.

#ifndef __vtkBoundedObject_h
#define __vtkBoundedObject_h

#include "vtkGMTL.h"
#include "vtkSetGet.h"

// -----------------------------------------------------------------------class
class vtkBoundedObject
{
public:
  // ............................................................public-methods
  vtkBoundedObject();
  ~vtkBoundedObject();
  virtual void ComputeBounds()=0;
  virtual vtkVector3f GetMin()=0;
  virtual vtkVector3f GetMax()=0;
  vtkSetMacro(BBoxCenter,vtkVector3f)
  vtkGetMacro(BBoxCenter,vtkVector3f)
  vtkSetMacro(BBoxSize,vtkVector3f)
  vtkGetMacro(BBoxSize,vtkVector3f)

  void SetBBoxCenter(vtkVector3f min, vtkVector3f max);
  void SetBBoxSize(vtkVector3f min, vtkVector3f max);
//  vtkGetMacro(Min,vtkVector3f)
//  vtkGetMacro(Max,vtkVector3f)
  virtual float GetBBoxRadius();
protected:
  vtkVector3f Min;
  vtkVector3f Max;
  vtkVector3f BBoxCenter;
  vtkVector3f BBoxSize;
};

#endif // __vtkBoundedObject_h
