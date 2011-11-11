/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

#include "vesBlendFunction.h"

// VES includes
#include "vesRenderState.h"

vesBlendFunction::vesBlendFunction(Parameter source, Parameter destination) :
  m_source     (source),
  m_destination(destination)
{
}


vesBlendFunction::~vesBlendFunction()
{
}


void vesBlendFunction::apply(const vesRenderState &renderState)
{
  vesNotUsed(renderState);

  glBlendFunc(m_source, m_destination);
}

