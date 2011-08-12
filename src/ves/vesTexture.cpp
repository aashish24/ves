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
static const GLfloat squareVertices[] = {
  -1.0f, -1.0f,
  1.0f, -1.0f,
  -1.0f,  1.0f,
  1.0f,  1.0f,
};

static const GLfloat textureVertices[] = {
  1.0f, 1.0f,
  1.0f, 0.0f,
  0.0f,  1.0f,
  0.0f,  0.0f,
};
// -------------------------------------------------------------------cnstr
vesTexture::vesTexture(vesShaderProgram *shader,
                       SFImage image)
{
  _internal = new vesTextureInternal();
  this->ShaderProgram = shader;
  this->Image = image;
  this->loaded = false;
}

// -------------------------------------------------------------------destr
vesTexture::~vesTexture()
{
  delete _internal;
}

// ------------------------------------------------------------------public
void vesTexture::Render()
{

  if(!loaded)
    {
      glGenTextures(1,&texID);
      glBindTexture(GL_TEXTURE_2D,texID);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexImage2D(GL_TEXTURE_2D,
                   0,
                   GL_RGBA,
                   this->Image.width,
                   this->Image.height,
                   0,
                   GL_RGBA,
                   GL_UNSIGNED_BYTE,
                   this->Image.data);
      loaded = true;
    }
  this->ShaderProgram->Use();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texID);

  // Set uniforms
  vesMatrix4x4f orthoProjection = vesOrtho(-1,1,-1,1,-1,1000);
  this->ShaderProgram->SetUniformMatrix4x4f("u_ortho",orthoProjection);

  // Set Attributes
  // Enable Vertex Attribs


  // Assign data
  glVertexAttribPointer(this->ShaderProgram->GetAttribute("a_position"),
                        2,
                        GL_FLOAT,
                        0,
                        0,
                        squareVertices);
 this->ShaderProgram->EnableVertexArray("a_position");
  glVertexAttribPointer(this->ShaderProgram->GetAttribute("a_texCoord"),
                        2,
                        GL_FLOAT,
                        0,
                        0,
                        textureVertices);
  this->ShaderProgram->EnableVertexArray("a_texCoord");
  // Draw arrays
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  // Disable vertex attributes
  this->ShaderProgram->DisableVertexArray("a_position");
  this->ShaderProgram->DisableVertexArray("a_texCoord");
}
// -----------------------------------------------------------------private

