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

#include "vesUniform.h"

// VES includes
#include "vesShaderProgram.h"

// C++ includes
#include <limits>
#include <iostream>

vesUniform::vesUniform() :
  m_type            (Undefined),
  m_numberElements  (0)
{
  this->setMinimalDefaults();
}


vesUniform::~vesUniform()
{
  delete this->m_floatArray; this->m_floatArray = 0;
  delete this->m_intArray;   this->m_intArray = 0;
}


void vesUniform::update(const vesRenderState   &renderState,
                        const vesShaderProgram &program)
{
  vesNotUsed(renderState);
  vesNotUsed(program);
}


bool vesUniform::setType(Type type)
{
  if (this->m_type == type) {
    return true;
  }

  if (m_type != Undefined) {
    return false;
  }

  this->m_type = type;

  allocateDataArray();

  return true;
}


void vesUniform::setName(const std::string &name)
{
  if(this->m_name != std::string())
    return;

  this->m_name = name;
}


vesUniform::vesUniform(const std::string &name, float value) :
  m_type            (Float),
  m_numberElements  (1)
{
  this->setMinimalDefaults();
  this->setName(name);
  this->allocateDataArray();
  this->set(value);
}


vesUniform::vesUniform(const std::string &name, int value) :
  m_type            (Int),
  m_numberElements  (1)
{
  this->setMinimalDefaults();
  this->setName(name);
  this->allocateDataArray();
  this->set(value);
}


vesUniform::vesUniform(const std::string &name, bool value) :
  m_type            (Bool),
  m_numberElements  (1)
{
  this->setMinimalDefaults();
  this->setName(name);
  this->allocateDataArray();
  this->set(value);
}


vesUniform::vesUniform(const std::string &name, const vesVector2f& vector) :
  m_type            (FloatVec2),
  m_numberElements  (1)
{
  this->setMinimalDefaults();
  this->setName(name);
  this->allocateDataArray();
  this->set(vector);
}


vesUniform::vesUniform(const std::string &name, const vesVector3f& vector) :
  m_type            (FloatVec3),
  m_numberElements  (1)
{
  this->setMinimalDefaults();
  this->setName(name);
  this->allocateDataArray();
  this->set(vector);
}


vesUniform::vesUniform(const std::string &name, const vesVector4f& vector) :
  m_type            (FloatVec4),
  m_numberElements  (1)
{
  this->setMinimalDefaults();
  this->setName(name);
  this->allocateDataArray();
  this->set(vector);
}


vesUniform::vesUniform(const std::string &name, const vesMatrix3x3f& matrix) :
  m_type            (FloatMat3),
  m_numberElements  (1)
{
  this->setMinimalDefaults();
  this->setName(name);
  this->allocateDataArray();
  this->set(matrix);
}


vesUniform::vesUniform(const std::string &name, const vesMatrix4x4f& matrix) :
  m_type            (FloatMat4),
  m_numberElements  (1)
{
  this->setMinimalDefaults();
  this->setName(name);
  this->allocateDataArray();
  this->set(matrix);
}


bool vesUniform::set(float value)
{
  if (this->m_numberElements == 0)
    m_numberElements = 1;

  return isScalar() ? this->setElement(0, value) : false;
}


bool vesUniform::set(int value)
{
  if (this->m_numberElements == 0)
    this->m_numberElements = 1;

  return isScalar() ? this->setElement(0, value) : false;
}


bool vesUniform::set(bool value)
{
  if (this->m_numberElements == 0)
    this->m_numberElements = 1;

  return isScalar() ? this->setElement(0, value) : false;
}


bool vesUniform::set(const vesVector2f &vector)
{
  if (this->m_numberElements == 0)
    m_numberElements = 1;

  return isScalar() ? setElement(0, vector) : false;
}


bool vesUniform::set(const vesVector3f &vector)
{
  if (this->m_numberElements == 0)
    this->m_numberElements = 1;

  return isScalar() ? setElement(0, vector) : false;
}


bool vesUniform::set(const vesVector4f &vector)
{
  if (this->m_numberElements == 0)
    this->m_numberElements = 1;

  return isScalar() ? setElement(0, vector) : false;
}


bool vesUniform::set(const vesMatrix3x3f &matrix)
{
  if (this->m_numberElements == 0)
    this->m_numberElements = 1;

  return isScalar() ? setElement(0, matrix) : false;
}


bool vesUniform::set(const vesMatrix4x4f &matrix)
{
  if (this->m_numberElements == 0)
    this->m_numberElements = 1;

  return isScalar() ? setElement(0, matrix) : false;
}


bool vesUniform::get(float &value) const
{
  return isScalar() ? getElement(0, value) : false;
}


bool vesUniform::get(vesVector2f &vector) const
{
  return isScalar() ? getElement(0, vector) : false;
}


bool vesUniform::get(vesVector3f &vector) const
{
  return isScalar() ? getElement(0, vector) : false;
}


bool vesUniform::get(vesMatrix3x3f &matrix) const
{
  return isScalar() ? getElement(0, matrix) : false;
}


bool vesUniform::get(vesMatrix4x4f &matrix) const
{
  return isScalar() ? getElement(0, matrix) : false;
}


bool vesUniform::get(int &value) const
{
  return isScalar() ? getElement(0, value) : false;
}


bool vesUniform::get(bool &value) const
{
  return isScalar() ? getElement(0, value) : false;
}


bool vesUniform::setElement(unsigned int index, float value)
{
  if (index >= this->m_numberElements || !isCompatibleType(Float))
    return false;

  unsigned int j = index * getTypeNumberOfComponents(this->m_type);

  (*this->m_floatArray)[j] = value;

  // \todo: Make state dirty.

  return true;
}


bool vesUniform::setElement(unsigned int index, int value)
{
  if (index >= this->m_numberElements || !isCompatibleType(Int))
    return false;

  unsigned int j = index * getTypeNumberOfComponents(this->m_type);

  (*this->m_intArray)[j] = value;

  // \todo: Make state dirty.

  return true;
}


bool vesUniform::setElement(unsigned int index, bool value)
{
  if (index >= this->m_numberElements || !isCompatibleType(Bool))
    return false;

    unsigned int j = index * getTypeNumberOfComponents(getType());

    (*this->m_intArray)[j] = value;

    // \todo: Make state dirty.

    return true;
}


bool vesUniform::setElement(unsigned int index, const vesVector2f &vector)
{
  if (index >= this->m_numberElements || !isCompatibleType(FloatVec2))
    return false;

  unsigned int j = index * getTypeNumberOfComponents(this->m_type);

  (*this->m_floatArray)[j]   = vector[0];
  (*this->m_floatArray)[j+1] = vector[1];

  // \todo: Make state dirty.

  return true;
}


bool vesUniform::setElement(unsigned int index, const vesVector3f &vector)
{
  if (index >= this->m_numberElements || !isCompatibleType(FloatVec3))
    return false;

  unsigned int j = index * getTypeNumberOfComponents(this->m_type);

  (*this->m_floatArray)[j]   = vector[0];
  (*this->m_floatArray)[j+1] = vector[1];
  (*this->m_floatArray)[j+2] = vector[2];

  // \todo: Make state dirty.

  return true;
}


bool vesUniform::setElement(unsigned int index, const vesVector4f &vector)
{
  if (index >= this->m_numberElements || !isCompatibleType(FloatVec4))
    return false;

  unsigned int j = index * getTypeNumberOfComponents(this->m_type);

  (*this->m_floatArray)[j]   = vector[0];
  (*this->m_floatArray)[j+1] = vector[1];
  (*this->m_floatArray)[j+2] = vector[2];
  (*this->m_floatArray)[j+3] = vector[3];

  // \todo: Make state dirty.

  return true;
}


bool vesUniform::setElement(unsigned int index, const vesMatrix3x3f &matrix)
{
  if (index >= this->m_numberElements || !isCompatibleType(FloatMat3))
    return false;

  unsigned int j = index * getTypeNumberOfComponents(this->m_type);

  for (int i = 0; i < 9; ++i)
    (*this->m_floatArray)[j+i] = matrix.data()[i];

  // \todo: Make state dirty.

  return true;
}


bool vesUniform::setElement(unsigned int index, const vesMatrix4x4f &matrix)
{
  if (index >= this->m_numberElements || !isCompatibleType(FloatMat4))
    return false;

  unsigned int j = index * getTypeNumberOfComponents(this->m_type);

  for (int i = 0; i < 16; ++i)
    (*this->m_floatArray)[j+i] = matrix.data()[i];

  // \todo: Make state dirty.

  return true;
}


bool vesUniform::getElement(unsigned int index, int &value) const
{
  if (index >= this->m_numberElements || !isCompatibleType(Int))
    return false;

  unsigned int j = index * getTypeNumberOfComponents(this->m_type);

  value = (*this->m_intArray)[j];
  return true;
}


bool vesUniform::getElement(unsigned int index, bool &value) const
{
  if (index >= this->m_numberElements || !isCompatibleType(Bool))
    return false;

  unsigned int j = index * getTypeNumberOfComponents(this->m_type);

  value = (*this->m_intArray)[j];
  return true;
}


bool vesUniform::getElement(unsigned int index, float &value) const
{
  if (index >= this->m_numberElements || !isCompatibleType(Float))
    return false;

  unsigned int j = index * getTypeNumberOfComponents(this->m_type);

  value = (*this->m_floatArray)[j];
  return true;
}


bool vesUniform::getElement(unsigned int index, vesVector2f &value) const
{
  if (index >= this->m_numberElements || !isCompatibleType(Float))
    return false;

  unsigned int j = index * getTypeNumberOfComponents(this->m_type);

  value[0] = (*this->m_floatArray)[j];
  value[1] = (*this->m_floatArray)[j+1];
  return true;
}


bool vesUniform::getElement(unsigned int index, vesVector3f &value) const
{
  if (index >= this->m_numberElements || !isCompatibleType(Float))
    return false;

  unsigned int j = index * getTypeNumberOfComponents(this->m_type);

  value[0] = (*this->m_floatArray)[j];
  value[1] = (*this->m_floatArray)[j+1];
  value[2] = (*this->m_floatArray)[j+2];
  return true;
}


bool vesUniform::getElement(unsigned int index, vesVector4f &value) const
{
  if (index >= this->m_numberElements || !isCompatibleType(Float))
    return false;

  unsigned int j = index * getTypeNumberOfComponents(this->m_type);

  value[0] = (*this->m_floatArray)[j];
  value[1] = (*this->m_floatArray)[j+1];
  value[2] = (*this->m_floatArray)[j+2];
  value[3] = (*this->m_floatArray)[j+3];
  return true;
}


bool vesUniform::getElement(unsigned int index, vesMatrix3x3f &value) const
{
  if (index >= this->m_numberElements || !isCompatibleType(Float))
    return false;

  for (unsigned int i = 0; i < 9; ++i)
    value.data()[i] = (&m_floatArray->front())[i];

  return true;
}


bool vesUniform::getElement(unsigned int index, vesMatrix4x4f &value) const
{
  if (index >= this->m_numberElements || !isCompatibleType(FloatMat4))
    return false;

  for (unsigned int i = 0; i < 16; ++i)
    value.data()[i] = (&m_floatArray->front())[i];

  return true;
}


void vesUniform::callGL(int location) const
{
  if (this->m_numberElements < 1)
    return;

  switch (this->m_type)
  {
    case Bool:
    case Int:
        if (this->m_intArray)
          glUniform1iv(location, this->m_numberElements,
                       &this->m_intArray->front());
        break;

    case Float:
        if (this->m_floatArray)
          glUniform1fv(location, this->m_numberElements,
                       &this->m_floatArray->front());
        break;

    case FloatVec2:
        if (this->m_floatArray)
          glUniform2fv(location, this->m_numberElements,
                       &this->m_floatArray->front());
        break;

    case FloatVec3:
        if (this->m_floatArray)
          glUniform3fv(location, this->m_numberElements,
                       &this->m_floatArray->front());
        break;

    case FloatVec4:
      if (this->m_floatArray)
        glUniform4fv(location, this->m_numberElements,
                     &this->m_floatArray->front());
      break;

    case FloatMat3:
        if (m_floatArray)
          glUniformMatrix3fv(location, this->m_numberElements, GL_FALSE,
                             &this->m_floatArray->front());
        break;

    case FloatMat4:
        if (this->m_floatArray)
          glUniformMatrix4fv(location, this->m_numberElements, GL_FALSE,
                             &this->m_floatArray->front());
        break;

    default:
        std::cerr << "ERROR: " << __FILE__ ":" << __LINE__ << std::endl;
        break;
  }
}


void vesUniform::setMinimalDefaults()
{
  m_intArray    = 0;
  m_floatArray  = 0;
}


bool vesUniform::isCompatibleType(Type type) const
{
  if (type == Undefined || this->m_type == Undefined)
    return false;
  if (type == this->m_type)
    return true;

  return false;
}


unsigned int vesUniform::getInternalArrayNumberOfElements() const
{
  if (m_numberElements < 1 || this->m_type == Undefined)
    return 0;

  return m_numberElements * this->getTypeNumberOfComponents(this->m_type);
}


int vesUniform::getTypeNumberOfComponents(Type type) const
{
  switch (type)
  {
    case Float:
    case Int:
    case Bool:
      return 1;

    case FloatVec2:
    case IntVec2:
    case BoolVec2:
      return 2;

    case FloatVec3:
    case IntVec3:
    case BoolVec3:
      return 3;

    case FloatVec4:
    case IntVec4:
    case BoolVec4:
      return 4;

    case FloatMat3:
      return 9;

    case FloatMat4:
      return 16;

    default:
        return 0;
  }
}


GLenum vesUniform::getInternalArrayType(Type type) const
{
  switch (type)
  {
  case Float:
  case FloatVec2:
  case FloatVec3:
  case FloatVec4:
  case FloatMat3:
  case FloatMat4:
    return GL_FLOAT;

  case Int:
  case IntVec2:
  case IntVec3:
  case IntVec4:
  case Bool:
  case BoolVec2:
  case BoolVec3:
  case BoolVec4:
    return GL_INT;

  default:
      return 0;
  }
}


void vesUniform::allocateDataArray()
{
  // Once allocated and valid, job is done.
  if (this->m_intArray || this->m_floatArray)
  {
    return;
  }

  int arrayNumberOfElements = this->getInternalArrayNumberOfElements();

  if (arrayNumberOfElements)
  {
    switch (this->getInternalArrayType(this->m_type))
    {
      case GL_FLOAT:
        this->m_floatArray = new FloatArray(arrayNumberOfElements);
        this->m_intArray   = 0;
        return;

      case GL_INT:
        this->m_intArray   = new IntArray(arrayNumberOfElements);
        this->m_floatArray = 0;
        return;

      default:
        break;
    }
  }
}
