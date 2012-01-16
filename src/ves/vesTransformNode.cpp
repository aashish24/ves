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

#include "vesTransformNode.h"

// VES includes
#include "vesVisitor.h"

class vesTransformNode::vesInternal
{
private:
  vesMatrix4x4f T,C,R,SR,S,SRinv,Cinv;

public:
  vesMatrix4x4f Eval()
  {
    return T * C * R * SR * S * SRinv * Cinv;
  }

  void SetTranslation(vesVector3f trans)
  {
    T = makeTranslationMatrix4x4(trans);
  }

  void SetCenter(vesVector3f center)
  {
    C = makeTranslationMatrix4x4(center);
    Cinv = makeInverseMatrix4x4(C);
  }

  void SetRotation(vesVector4f rot)
  {
    R = makeRotationMatrix4x4(rot[3], rot[0], rot[1], rot[2]);
  }

  void SetScaleOrientation(vesVector4f sr)
  {
    SR = makeRotationMatrix4x4(sr[3], sr[0], sr[1], sr[2]);
    SRinv = makeInverseMatrix4x4(SR);
  }

  void SetScale(vesVector3f scale)
  {
    S = makeScaleMatrix4x4(scale[0],scale[1],scale[2]);
  }
};


vesTransformNode::vesTransformNode() : vesGroupNode()
{
  this->m_internal = new vesInternal();
  this->m_center = vesVector3f (0,0,0);
  this->m_rotation = vesVector4f(0,0,1,0);
  this->m_scale = vesVector3f(1,1,1);
  this->m_scaleOrientation = vesVector4f(0,0,1,0);
  this->m_translation = vesVector3f(0,0,0);
  this->m_referenceFrame = Relative;
}


vesTransformNode::~vesTransformNode()
{
  delete this->m_internal;
}


vesMatrix4x4f vesTransformNode::matrix()
{
  this->setInternals();
  return m_internal->Eval();
}


void vesTransformNode::setCenter(const vesVector3f &center)
{
  this->m_center = center;
  this->setBoundsDirty(true);
}


const vesVector3f& vesTransformNode::center() const
{
  return this->m_center;
}


void vesTransformNode::setRotation(const vesVector4f &rotation)
{
  this->m_rotation = rotation;
  this->setBoundsDirty(true);
}


const vesVector4f& vesTransformNode::rotation() const
{
  return this->m_rotation;
}


void vesTransformNode::setScale(const vesVector3f &scale)
{
  this->m_scale = scale;
  this->setBoundsDirty(true);
}


const vesVector3f& vesTransformNode::scale() const
{
  return this->m_scale;
}


void vesTransformNode::setScaleOrientation(const vesVector4f &scaleOrientation)
{
  this->m_scaleOrientation = scaleOrientation;
  this->setBoundsDirty(true);
}


const vesVector4f& vesTransformNode::scaleOrientation() const
{
  return this->m_scaleOrientation;
}


void vesTransformNode::setTranslation(const vesVector3f &translation)
{
  this->m_translation = translation;
  this->setBoundsDirty(true);
}


const vesVector3f& vesTransformNode::translation() const
{
  return this->m_translation;
}


bool vesTransformNode::setReferenceFrame(ReferenceFrame referenceFrame)
{
  bool success = true;

  if (this->m_referenceFrame != referenceFrame) {
    this->setBoundsDirty(true);
    this->m_referenceFrame = referenceFrame;
    return success;
  }

  return !success;
}


vesTransformNode::ReferenceFrame vesTransformNode::referenceFrame() const
{
  return this->m_referenceFrame;
}


void vesTransformNode::setInternals()
{
  m_internal->SetTranslation(m_translation);
  m_internal->SetCenter(m_center);
  m_internal->SetRotation(m_rotation);
  m_internal->SetScaleOrientation(m_scaleOrientation);
  m_internal->SetScale(m_scale);
}

vesMatrix4x4f vesTransformNode::computeTransform()
{
  setInternals();
  return m_internal->Eval();
}


void vesTransformNode::accept(vesVisitor &visitor)
{
  visitor.visit(*this);
}


bool vesTransformNode::computeLocalToWorldMatrix(vesMatrix4x4f &matrix,
                                                 vesVisitor &visitor)
{
  vesNotUsed(visitor);

  if (this->m_referenceFrame == Absolute) {
    matrix = this->matrix();
  }
  else  {
    matrix = matrix * this->matrix();
  }

  return true;
}


bool vesTransformNode::computeWorldToLocalMatrix(vesMatrix4x4f &matrix,
                                                 vesVisitor &visitor)
{
  vesNotUsed(visitor);

  vesMatrix4x4f inverseMatrix = this->matrix().inverse();

  if (this->m_referenceFrame == Absolute) {
    matrix  = inverseMatrix;
  }
  else {
    matrix = inverseMatrix * matrix;
  }

  return true;
}


void vesTransformNode::updateBounds(vesNode &child)
{
  if (!this->boundsDirty()) {
    return;
  }

  if (child.asTransformNode()
      && child.asTransformNode()->referenceFrame() == Absolute )
  {
    return;
  }

  if (child.isOverlayNode()) {
    return;
  }

  vesVector3f min = child.boundsMinimum();
  vesVector3f max = child.boundsMaximum();

  min = transformPoint3f(this->matrix(), min);
  max = transformPoint3f(this->matrix(), max);

  child.setBounds(min, max);

  for (int i = 0; i < 3; ++i) {
    if (max[i] > this->m_boundsMaximum[i]) {
      this->m_boundsMaximum[i] = max[i];
    }

    if (min[i] < this->m_boundsMinimum[i]) {
      this->m_boundsMinimum[i] = min[i];
    }
  }

  // Now update the bounds, bounds size and center.
  this->setBounds(this->m_boundsMinimum, this->m_boundsMaximum);
}
