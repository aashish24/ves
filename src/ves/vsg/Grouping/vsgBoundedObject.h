// ============================================================================
/**
 * @file   vsgBoundedObject.h
 *
 * @section COPYRIGHT
 *
 * Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 * All rights reserved.
 * See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 *
 *   This software is distributed WITHOUT ANY WARRANTY; without even
 *   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *   PURPOSE.  See the above copyright notice for more information.
 *
 * @section DESCRIPTION
 *
 * Specifies bounds of an object
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgBoundedObject_h
#define __vsgBoundedObject_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"

// -----------------------------------------------------------------pre-defines

// -----------------------------------------------------------------------class
class vsgBoundedObject
{
public:
  // ............................................................public-methods
  vsgBoundedObject();
  ~vsgBoundedObject();
  virtual void ComputeBounds()=0;
  InOutSF(BBoxCenter,SFVec3f)
  InOutSF(BBoxSize, SFVec3f)
  InOutSF(min,SFVec3f)
  InOutSF(max,SFVec3f)

  void set_BBoxCenter(vesVector3f min, vesVector3f max);
  void set_BBoxSize(vesVector3f min, vesVector3f max);
  virtual float GetBBoxRadius();
  protected:
  // ...........................................................protected-ivars
};
#endif // __vsgBoundedObject_h
