/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.
  Copyright 2012 Willow Garage, Inc.

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

#include "vesActor.h"

// VES includes
#include "vesMapper.h"
#include "vesMaterial.h"
#include "vesTransformNode.h"
#include "vesTransformPrivate.h"
#include "vesVisitor.h"

vesActor::vesActor() : vesNode()
{
  // \todo: Create a default apperance.
  this->m_implementation = new vesTransformPrivate();
  this->m_center = vesVector3f (0,0,0);
  this->m_rotation = vesVector4f(0,0,1,0);
  this->m_scale = vesVector3f(1,1,1);
  this->m_scaleOrientation = vesVector4f(0,0,1,0);
  this->m_translation = vesVector3f(0,0,0);
  this->m_referenceFrame = Relative;
}


vesActor::~vesActor()
{
  delete this->m_implementation;
}


vesMatrix4x4f vesActor::modelViewMatrix()
{
  this->setInternals();
  return this->m_implementation->eval();
}


void vesActor::setCenter(const vesVector3f &center)
{
  this->m_center = center;
  this->setBoundsDirty(true);
}


const vesVector3f& vesActor::center() const
{
  return this->m_center;
}


void vesActor::setRotation(const vesVector4f &rotation)
{
  this->m_rotation = rotation;
  this->setBoundsDirty(true);
}


const vesVector4f& vesActor::rotation() const
{
  return this->m_rotation;
}


void vesActor::setScale(const vesVector3f &scale)
{
  this->m_scale = scale;
  this->setBoundsDirty(true);
}


const vesVector3f& vesActor::scale() const
{
  return this->m_scale;
}


void vesActor::setScaleOrientation(const vesVector4f &scaleOrientation)
{
  this->m_scaleOrientation = scaleOrientation;
  this->setBoundsDirty(true);
}


const vesVector4f& vesActor::scaleOrientation() const
{
  return this->m_scaleOrientation;
}


void vesActor::setTranslation(const vesVector3f &translation)
{
  this->m_translation = translation;
  this->setBoundsDirty(true);
}


const vesVector3f& vesActor::translation() const
{
  return this->m_translation;
}


bool vesActor::setReferenceFrame(ReferenceFrame referenceFrame)
{
  bool success = true;

  if (this->m_referenceFrame != referenceFrame) {
    this->setBoundsDirty(true);
    this->m_referenceFrame = referenceFrame;
    return success;
  }

  return !success;
}


vesTransformNode::ReferenceFrame vesActor::referenceFrame() const
{
  return this->m_referenceFrame;
}


void vesActor::setInternals()
{
  m_implementation->setTranslation(m_translation);
  m_implementation->setCenter(m_center);
  m_implementation->setRotation(m_rotation);
  m_implementation->setScaleOrientation(m_scaleOrientation);
  m_implementation->setScale(m_scale);
}


bool vesActor::computeLocalToWorldMatrix(vesMatrix4x4f &matrix,
                                         vesVisitor &visitor)
{
  vesNotUsed(visitor);

  if (this->m_referenceFrame == Absolute) {
    matrix = this->modelViewMatrix();
  }
  else  {
    matrix = matrix * this->modelViewMatrix();
  }

  return true;
}


bool vesActor::computeWorldToLocalMatrix(vesMatrix4x4f &matrix,
                                         vesVisitor &visitor)
{
  vesNotUsed(visitor);

  vesMatrix4x4f inverseMatrix = this->modelViewMatrix().inverse();

  if (this->m_referenceFrame == Absolute) {
    matrix  = inverseMatrix;
  }
  else {
    matrix = inverseMatrix * matrix;
  }

  return true;
}


void vesActor::setMapper(vesSharedPtr<vesMapper> mapper)
{
  if (mapper && mapper != this->m_mapper) {
    this->m_mapper = mapper;
    this->setBoundsDirty(true);
  }
}


void vesActor::accept(vesVisitor &visitor)
{
  assert(this->m_mapper);

  visitor.visit(*this);
}


void vesActor::ascend(vesVisitor &visitor)
{
  assert(this->m_mapper);

  // \todo: Implement this.
  vesNotUsed(visitor);
}


void vesActor::computeBounds()
{
  assert(this->m_mapper);

  if (this->m_mapper && this->m_mapper->boundsDirty()) {
    this->m_mapper->computeBounds();

    vesVector3f min = transformPoint3f(this->matrix(), this->m_mapper->boundsMinimum());
    vesVector3f max = transformPoint3f(this->matrix(), this->m_mapper->boundsMaximum());

    this->setBounds(min, max);

    // Since now we have new internal bounds, we would have to
    // calculate whole bounds for this actor once again.
    this->setBoundsDirty(true);
    this->setBoundsDirty(false);
    this->setParentBoundsDirty(true);
  }
  else if (!this->m_mapper && this->boundsDirty()) {
    this->resetBounds();
  }
}
