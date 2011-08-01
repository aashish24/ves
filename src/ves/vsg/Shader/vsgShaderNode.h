// ============================================================================
/**
 * @file   vsgShaderNode.h
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
 * This abstract node type is the base type for all node types that specify a
 * programmable shader.
 *
 * The isSelected output field is used to indicate that this shader instance is
 * the one selected for use by the browser. A TRUE value indicates that this
 * instance is in use. A FALSE value indicates that this instance is not in
 * use. The rules for when a browser decides to select a particular node
 * instance are described in 31.2.2.3 Selecting an appropriate shader.
 *
 * The isValid field is used to indicate whether the current shader objects can
 * be run as a shader program.
 *
 * EXAMPLE There are no syntax errors and the hardware can support all the
 * required features.
 *
 * The definition of this field may also add additional semantics on a
 * per-language basis.
 *
 * The language field is used to indicate to the browser which shading language
 * is used for the source file(s). This field may be used as a hint for the
 * browser if the shading language is not immediately determinable from the
 * source (e.g., a generic MIME type of text/plain is returned). A browser may
 * use this field for determining which node instance will be selected and to
 * ignore languages that it is not capable of supporting. Three basic language
 * types are defined for this specification and others may be optionally
 * supported by a browser.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgShaderNode_h
#define __vsgShaderNode_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Shape/vsgAppearanceChildNode.h"

// -----------------------------------------------------------------------class
class vsgShaderNode: public vsgAppearanceChildNode
{
public:
  // ............................................................public-methods
  vsgShaderNode();
  virtual ~vsgShaderNode();
  InOnlySF(activate,SFBool)
  OutOnlySF(isSelected,SFBool)
  OutOnlySF(isValid,SFBool)
  protected:
  // ...........................................................protected-ivars
  SFString _language;
};

#endif // __vsgShaderNode_h
