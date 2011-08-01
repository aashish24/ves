// ============================================================================
/**
 * @file   vsgNode.h
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
#ifndef __vsgNode_h
#define __vsgNode_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include <vector>

// -----------------------------------------------------------------pre-defines
class vsgVisitor;
class Painter;
// -----------------------------------------------------------------------class
class vsgNode
{
public:
  // ............................................................public-methods
  vsgNode();
  ~vsgNode();
  virtual bool accept(vsgVisitor *vsgVisitor){return true;} // This has to be pure virtual
  virtual bool Read() = 0;
  virtual void Render(Painter * render) =0;
protected:
  // ...........................................................protected-ivars

};

typedef vsgNode* SFNode;
typedef std::vector<SFNode> MFNode;

#endif // __vsgNode_h
