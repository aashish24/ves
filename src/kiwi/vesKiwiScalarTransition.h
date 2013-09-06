#ifndef __vesKiwiScalarTransition_h
#define __vesKiwiScalarTransition_h

#include "vesKiwiTransition.h"

/// \class vesKiwiScalarTransition
/// \ingroup KiwiPlatform
/// \brief A transition which updates a single scalar value.
///
/// This templated class takes a pointer to a scalar value
/// of a primitive type (e.g., double, int).
///
/// You are responsible for ensuring that the pointer is
/// valid for as long as the transition exists.
template<typename T>
class vesKiwiScalarTransition : public vesKiwiTransition
{
public:
  vesTypeMacro(vesKiwiScalarTransition<T>);
  vesSharedFromThisMacro(vesKiwiTransition);

  static typename vesKiwiScalarTransition<T>::Ptr create(T* value);
  static typename vesKiwiScalarTransition<T>::Ptr create(
    T* value, typename vesKiwiScalarTransition<T>::Ptr& transition);

  vesKiwiScalarTransition(T* value);
  virtual ~vesKiwiScalarTransition();

  /// Set/get/take the initial value for the scalar.
  //@{
  virtual vesKiwiScalarTransition<T>::Ptr setInitialValue(const T& d);
  virtual vesKiwiScalarTransition<T>::Ptr takeInitialValue();
  T initialValue() { return this->InitialValue; }
  //@}

  /// Set/get/take the final value for the scalar.
  //@{
  virtual vesKiwiScalarTransition<T>::Ptr setFinalValue(const T& d);
  virtual vesKiwiScalarTransition<T>::Ptr takeFinalValue();
  T finalValue() { return this->FinalValue; }
  //@}

  virtual vesKiwiTransition::TransitionStatus willBegin();
  virtual vesKiwiTransition::TransitionStatus prepareState(double state);

protected:
  T* Value;

  T InitialValue;
  bool InitialValueSet;

  T FinalValue;
  bool FinalValueSet;
};

/// A public, static method to construct instance of this transition class.
//@{
template<typename T>
typename vesKiwiScalarTransition<T>::Ptr vesKiwiScalarTransition<T>::create(T* value)
{
  typename vesKiwiScalarTransition<T>::Ptr transition;
  return vesKiwiScalarTransition<T>::create(value, transition);
}


template<typename T>
typename vesKiwiScalarTransition<T>::Ptr vesKiwiScalarTransition<T>::create(
  T* value, typename vesKiwiScalarTransition<T>::Ptr& transition)
{
  transition =
    typename vesKiwiScalarTransition<T>::Ptr(
      new vesKiwiScalarTransition<T>(value));

  return transition;
}
//@}

/// A private constructor. Use the static \a create method instead.
template<typename T>
vesKiwiScalarTransition<T>::vesKiwiScalarTransition(T* value)
{
  assert(value);
  this->Value = value;
  this->InitialValueSet = false;
  this->FinalValueSet = false;
}

template<typename T>
vesKiwiScalarTransition<T>::~vesKiwiScalarTransition()
{
  this->Value = NULL;
}

/// \brief Set/take the value of the variable to impose at the start of the transition.
//@{
template<typename T>
typename vesKiwiScalarTransition<T>::Ptr vesKiwiScalarTransition<T>::setInitialValue(const T& v)
{
  this->InitialValue = v;
  this->InitialValueSet = true;
  return shared_from_this();
}

template<typename T>
typename vesKiwiScalarTransition<T>::Ptr vesKiwiScalarTransition<T>::takeInitialValue()
{
  this->InitialValue = *this->Value;
  this->InitialValueSet = true;
  return shared_from_this();
}
//@}

/// \brief Set/take the value of the variable to impose at the end of the transition.
//@{
template<typename T>
typename vesKiwiScalarTransition<T>::Ptr vesKiwiScalarTransition<T>::setFinalValue(const T& v)
{
  this->FinalValue = v;
  this->FinalValueSet = true;
  return shared_from_this();
}

template<typename T>
typename vesKiwiScalarTransition<T>::Ptr vesKiwiScalarTransition<T>::takeFinalValue()
{
  this->FinalValue = *this->Value;
  this->FinalValueSet = true;
  return shared_from_this();
}
//@}

template<typename T>
vesKiwiTransition::TransitionStatus vesKiwiScalarTransition<T>::willBegin()
{
  if (!this->InitialValueSet)
    {
    this->takeInitialValue();
    }
  if (!this->FinalValueSet)
    {
    return vesKiwiTransition::TRANSITION_ERROR;
    }
  return vesKiwiTransition::TRANSITION_OK;
}

template<typename T>
vesKiwiTransition::TransitionStatus vesKiwiScalarTransition<T>::prepareState(double state)
{
  *this->Value = state * this->FinalValue + (1. - state) * this->InitialValue;
  return vesKiwiTransition::TRANSITION_OK;
}

#endif // __vesKiwiScalarTransition_h
