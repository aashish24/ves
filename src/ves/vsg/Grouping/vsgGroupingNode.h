// ============================================================================
/**
 * @file   vsgGroupingNode.h
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
 * Abstract base class
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgGroupingNode_h
#define __vsgGroupingNode_h
// --------------------------------------------------------------------includes
# include "vsg/Utility/vsgMacro.h"
# include "vsg/Utility/vsgTypes.h"
# include "vsg/Core/vsgChildNode.h"
# include "vsg/Grouping/vsgBoundedObject.h"

// -----------------------------------------------------------------pre-defines

// -----------------------------------------------------------------------class
class vsgGroupingNode: public vsgChildNode, public vsgBoundedObject
{
public:
  // ............................................................public-methods
  vsgGroupingNode();
  virtual ~vsgGroupingNode();
  void addChildren(MFNode &children);
  void removeChildren(MFNode &children);
  InOutMF(children,MFNode)
protected:
  // ...........................................................protected-ivars

};
#endif // __vsgGroupingNode_h
