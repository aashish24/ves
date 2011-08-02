// ============================================================================
/**
 * @file   vesTexture.cpp
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
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#include "vesTexture.h"
// --------------------------------------------------------------------includes
#include "vesShaderProgram.h"

// -------------------------------------------------------------------macro

// ................................................................internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure
// to update constructor and destructor methods.
struct vesTextureInternal
{
  double value; // sample
};

// ................................................................internal

// -------------------------------------------------------------------cnstr
vesTexture::vesTexture(vesShaderProgram *shader)
{
  _internal = new vesTextureInternal();
  this->ShaderProgram = shader;
}

// -------------------------------------------------------------------destr
vesTexture::~vesTexture()
{
  delete _internal;
}

// ------------------------------------------------------------------public
// -----------------------------------------------------------------private

