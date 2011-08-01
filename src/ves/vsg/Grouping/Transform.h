// ============================================================================
/**
 * @file   Transform.h
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
 *
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __Transform_h
#define __Transform_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgGroupingNode.h"
namespace vsg{
// -------------------------------------------------------------pre-defines
class TransformInternal;

// -------------------------------------------------------------------class
class Transform : public vsgGroupingNode
{
public:
  // ........................................................public-methods
  Transform();
  virtual ~Transform();
  InOutSF(center,SFVec3f)
  InOutSF(rotation,SFRotation)
  InOutSF(scale,SFVec3f)
  InOutSF(scaleOrientation,SFVec4f)
  InOutSF(translation,SFVec3f)
  SFMatrix4f eval();
  bool accept(vsgVisitor* vsgVisitor);
protected:
  // .......................................................protected-ivars

private:
  // .........................................................private-ivars
  TransformInternal *_internal;
  void setInternals();
  SFMatrix4f computeTransform();
};
}
#endif // __Transform_h
