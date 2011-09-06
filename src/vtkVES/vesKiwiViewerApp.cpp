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

#include "vesKiwiViewerApp.h"
#include "vesActor.h"
#include "vesMapper.h"
#include "vesRenderer.h"
#include "vesShader.h"
#include "vesShaderProgram.h"

#include <cassert>

//----------------------------------------------------------------------------
class vesKiwiViewerApp::vesInternal
{
public:

  vesInternal()
  {
    this->Actor = 0;
    this->Mapper = 0;
    this->Renderer = 0;
    this->Shader = 0;
    this->ShaderProgram = 0;
  }

  ~vesInternal()
  {
    delete this->Actor;
    delete this->Mapper;
    delete this->Renderer;
    delete this->Shader;
    delete this->ShaderProgram;
  }
  
  vesActor* Actor;
  vesMapper* Mapper;
  vesRenderer* Renderer;
  vesShader* Shader;
  vesShaderProgram* ShaderProgram;

  std::string VertexShaderSource;
  std::string FragmentShaderSource;
};

//----------------------------------------------------------------------------
vesKiwiViewerApp::vesKiwiViewerApp()
{
  this->Internal = new vesInternal();
}

//----------------------------------------------------------------------------
vesKiwiViewerApp::~vesKiwiViewerApp()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
vesActor* vesKiwiViewerApp::actor() const
{
  return this->Internal->Actor;
}

//----------------------------------------------------------------------------
vesMapper* vesKiwiViewerApp::mapper() const
{
  return this->Internal->Mapper;
}

//----------------------------------------------------------------------------
vesCamera* vesKiwiViewerApp::camera() const
{
  assert(this->Internal->Renderer);
  return this->Internal->Renderer->GetCamera();
}

//----------------------------------------------------------------------------
vesRenderer* vesKiwiViewerApp::renderer() const
{
  return this->Internal->Renderer;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setVertexShaderSource(const std::string& source)
{
  this->Internal->VertexShaderSource = source;
}

//----------------------------------------------------------------------------
void vesKiwiViewerApp::setFragmentShaderSource(const std::string& source)
{
  this->Internal->FragmentShaderSource = source;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initializeShaderProgram()
{
  this->Internal->ShaderProgram = new vesShaderProgram(
                                   const_cast<char*>(this->Internal->VertexShaderSource.c_str()),
                                   const_cast<char*>(this->Internal->FragmentShaderSource.c_str()),
                                   (_uni("u_mvpMatrix"),
                                    _uni("u_normalMatrix"),
                                    _uni("u_ecLightDir"),
                                    _uni("u_opacity"),
                                    _uni("u_enable_diffuse")),
                                   (_att("a_vertex"),
                                    _att("a_normal"),
                                    _att("a_vertex_color")));

  this->Internal->Shader = new vesShader(this->Internal->ShaderProgram);
  this->Internal->ShaderProgram->Use();

  return true;
}

//----------------------------------------------------------------------------
bool vesKiwiViewerApp::initializeRendering()
{
  assert(this->Internal->Shader);

  this->Internal->Renderer = new vesRenderer();
  this->Internal->Mapper = new vesMapper();
  this->Internal->Actor = new vesActor(this->Internal->Shader, this->Internal->Mapper);
  this->Internal->Actor->setColor(0.8, 0.8, 0.8, 1.0);
  this->Internal->Renderer->AddActor(this->Internal->Actor);

  return true;
}
