// ============================================================================
/**
 * @file   vesTexture.h
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
#ifndef __vesTexture_h
#define __vesTexture_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"

// -------------------------------------------------------------pre-defines
class vesTextureInternal;
class vesShaderProgram;

// -------------------------------------------------------------------class
class vesTexture
{
public:
  // ........................................................public-methods
  vesTexture(vesShaderProgram *program, SFImage image);
  virtual ~vesTexture();

protected:
  // .......................................................protected-ivars

private:
  // .........................................................private-ivars
  vesTextureInternal *_internal;
  vesShaderProgram *ShaderProgram;
  SFImage Image;
};
#endif // __vesTexture_h
