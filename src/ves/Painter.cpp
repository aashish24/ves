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
#include "Painter.h"

// VES includes
#include "vsgChildNode.h"
#include "vsgGeometryNode.h"
#include "vsg/Shape/Appearance.h"
#include "vsg/Shape/Shape.h"

#include "vesActor.h"
#include "vesActorCollection.h"
#include "vesMapper.h"
#include "vesCamera.h"
#include "vesShader.h"
#include "vesMultitouchWidget.h"
#include "vesTexture.h"
#include "vesShaderProgram.h"
#include "vesTriangleData.h"
#include "vesUniform.h"

// C/C++ includes
#include <cassert>
#include <iostream>
#include <vector>

namespace
{
  void PrintMatrix(vesMatrix4x4f mv)
  {
    for (int i = 0; i < 4; ++i) {
      std::cerr << mv[i][0] << "," << mv[i][1] << "," << mv[i][2] << ","
                << mv[i][3] << std::endl;
    }
  }
}


Painter::Painter()
{
  m_textureBackground = NULL;
}


Painter::~Painter()
{
}


void Painter::Texture(vesTexture* textureBackground)
{
  textureBackground->Render();
}


void Painter::setCamera(vesCamera *camera)
{
  this->push(camera->eval());
  // If there are children nodes then tternate through and render
  MFNode children = camera->get_children();
  if (children.size()) {
    for (int i = 0; i < children.size(); ++i)
      children[i]->render(this);
  }
  // Pop the transformation
  this->pop();
}


void Painter::Shader(vesShaderProgram* program)
{
  program->render(this);
}


void Painter::setShaderProgram(vesShaderProgram *shaderProg)
{
  shaderProg->use();
}

void Painter::Actor(vesActor * actor)
{
  if(actor->isSensor()) {
    if(actor->widget()->isActive()) {
      actor->set_translation(actor->widget()->GetTranslation());
      actor->set_rotation(actor->widget()->GetRotation());
      actor->set_scale(actor->widget()->GetScale());
    }
  }
  this->push(actor->eval());
  MFNode temp;
  temp = actor->get_children();
  temp[0]->render(this);
  this->pop();
}

void Painter::ActorCollection(vesActorCollection *actor)
{
  if(m_textureBackground)
    this->Texture(m_textureBackground);

  // Push the transformation
  this->push(actor->Eval());

  // If there are children nodes then tternate through and render
  MFNode children = actor->get_children();;
  if (children.size())
    for (int i = 0; i < children.size(); ++i)
      children[i]->render(this);

  // Pop the transformation
  this->pop();
}

void Painter::visitShape(vsg::Shape* shape)
{
  shape->get_appearance()->render(this);
  if(shape->get_geometry())
    shape->get_geometry()->render(this);
  else
    return;

  vesShaderProgram * program;
  vsg::Appearance *appear = (vsg::Appearance*) shape->get_appearance();

  // Using only one shader.
  program = (vesShaderProgram*) appear->attribute(0);

  vesMapper* mapper = (vesMapper*)shape->get_geometry();

  // Model-view matrix is everything except the top level matrix (the projection
  // matrix). This is needed for normal calculation.
  vesMatrix4x4f mv = this->eval(1);

  // The model-view-projection matrix includes everything.
  vesMatrix4x4f mvp = this->eval(0);

  vesMatrix3x3f normal_matrix =
    makeNormalMatrix3x3f(makeTransposeMatrix4x4(makeInverseMatrix4x4 (mv)));
  vtkPoint3f lightDir = vtkPoint3f(0.0,0.0,.650);

  vesVector3f light(lightDir.mData[0],lightDir.mData[1],lightDir.mData[2]);

  // \todo: This is definately broken. This is not the best way to set
  // unifroms, primarily because there is no guarentee that a program
  // will contain these uniforms.
  vesUniform *modelViewProjectionUniform =
    program->uniform("modelViewProjectionMatrix");
  assert(modelViewProjectionUniform && "Uniform not present in the program");
  modelViewProjectionUniform->set(mvp);

  vesUniform *normalMatrixUniform = program->uniform("normalMatrix");
  assert(normalMatrixUniform && "Uniform not present in the program");
  normalMatrixUniform->set(normal_matrix);

  vesUniform *lightDirectionUniform = program->uniform("lightDirection");
  assert(lightDirectionUniform && "Uniform not present in the program");
  lightDirectionUniform->set(light);

  vesUniform *opacityUniform = program->uniform("opacity");
  assert(opacityUniform && "Uniform not present in the program");
  opacityUniform->set(mapper->alpha());

  program->updateUniforms();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Enable our attribute arrays
  glEnableVertexAttribArray(vesShaderProgram::Position);
  glEnableVertexAttribArray(vesShaderProgram::Normal);

  if (mapper->data()->GetVertexColors().size() == 0) {
    glDisableVertexAttribArray(vesShaderProgram::Color);

    // FIXME: This could be reduced to one call if color was stored in
    // vtkColor4f or similar, and then use a call similar to the one in the else.
    glVertexAttrib3f(vesShaderProgram::Color,
                     mapper->red(),
                     mapper->green(),
                     mapper->blue());
    }
  else {
    glEnableVertexAttribArray(vesShaderProgram::Color);

    glVertexAttribPointer(vesShaderProgram::Color,
                          3,
                          GL_FLOAT,
                          0,
                          3*sizeof(float),
                          &(mapper->data()->GetVertexColors()[0]));
    }

  glVertexAttribPointer(vesShaderProgram::Position,
                        3,
                        GL_FLOAT,
                        0,
                        6 * sizeof(float),
                        &(mapper->data()->GetPoints()[0]));

  glVertexAttribPointer(vesShaderProgram::Normal,
                        3,
                        GL_FLOAT,
                        0,
                        6 * sizeof(float),
                        mapper->data()->GetPoints()[0].normal.mData);

  // draw vertices
  if (mapper->drawPoints()) {

    vesUniform *scalarRangeUniform = program->uniform("scalarRange");
    if (scalarRangeUniform)
      scalarRangeUniform->set(mapper->data()->GetPointScalarRange());

    glEnableVertexAttribArray(vesShaderProgram::Scalar);

    glVertexAttribPointer(vesShaderProgram::Scalar,
                          1,
                          GL_FLOAT,
                          0,
                          sizeof(float),
                          &(mapper->data()->GetPointScalars()[0]));

    glDrawArrays(GL_POINTS, 0, mapper->data()->GetPoints().size());
  }
  else {
    // Draw triangles
    glDrawElements(GL_TRIANGLES,
                   mapper->data()->GetTriangles().size() * 3,
                   GL_UNSIGNED_SHORT,
                   &mapper->data()->GetTriangles()[0]);

    // Draw lines
    vesUniform *enableDiffuseUniform = program->uniform("enableDiffuse");
    assert(enableDiffuseUniform && "Uniform not present in the program");
    enableDiffuseUniform->set(0);

    program->updateUniforms();

    glDrawElements(GL_LINES,
                   mapper->data()->GetLines().size() * 2,
                   GL_UNSIGNED_SHORT,
                   &mapper->data()->GetLines()[0]);

    if(enableDiffuseUniform)
      enableDiffuseUniform->set(1);
  }

  glDisable(GL_CULL_FACE);
  glDisable(GL_BLEND);

  glDisableVertexAttribArray(vesShaderProgram::Position);
  glDisableVertexAttribArray(vesShaderProgram::Normal);
  glDisableVertexAttribArray(vesShaderProgram::Color);
}

void Painter::push(const vesMatrix4x4f& mat)
{
  m_matrixStack.push_back(mat);
}

void Painter::pop()
{
  m_matrixStack.pop_back();
}

vesMatrix4x4f Painter::eval(int startIndex)
{
  vesMatrix4x4f temp;
  for (int i = startIndex; i < m_matrixStack.size(); ++i)
    temp *= m_matrixStack[i];

  return temp;
}

void Painter::setBackgroundTexture(vesTexture* background)
{
  m_textureBackground = background;
}
