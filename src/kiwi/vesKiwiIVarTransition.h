#ifndef __vesKiwiIVarTransition_h
#define __vesKiwiIVarTransition_h

#include "vesKiwiTransition.h"

/// \class vesKiwiIVarHelper
/// \ingroup KiwiPlatform
/// \brief A helper object used by vesKiwiIVarTransition to get and set class member values.
template<typename T, class C, const T& (C::*G)() const, void (C::*S)(const T&)>
struct vesKiwiIVarHelper
{
  typedef T ValueType;
  typedef C ClassType;
  typedef vesSharedPtr<C> ClassPtrType;

  vesKiwiIVarHelper(vesSharedPtr<C> instance)
    : Instance(instance) { }

  vesKiwiIVarHelper(const vesKiwiIVarHelper<T,C,G,S>& other)
    : Instance(other.Instance) { }

  T get() { return ((*this->Instance).*G)(); }
  void set(const T& value) { ((*this->Instance).*S)(value); }

protected:
  vesSharedPtr<C> Instance;
};

/// \brief A transition that updates a single member variable on a single instance.
template<typename T>
class vesKiwiIVarTransition : public vesKiwiTransition
{
public:
  vesTypeMacro(vesKiwiIVarTransition<T>);
  vesSharedFromThisMacro(vesKiwiTransition);

  static typename vesKiwiIVarTransition<T>::Ptr create(
    typename T::ClassPtrType instance);
  static typename vesKiwiIVarTransition<T>::Ptr create(
    typename T::ClassPtrType instance,
    typename vesKiwiIVarTransition<T>::Ptr& transition);

  vesKiwiIVarTransition(const T& ivarHelper);
  virtual ~vesKiwiIVarTransition();

  /// \brief Set/get/take the initial value for the scalar.
  virtual vesKiwiIVarTransition<T>::Ptr setInitialValue(const typename T::ValueType& d);
  virtual vesKiwiIVarTransition<T>::Ptr takeInitialValue();
  typename T::ValueType initialValue() { return this->InitialValue; }

  /// \brief Set/get/take the final value for the scalar.
  virtual vesKiwiIVarTransition<T>::Ptr setFinalValue(const typename T::ValueType& d);
  virtual vesKiwiIVarTransition<T>::Ptr takeFinalValue();
  typename T::ValueType finalValue() { return this->FinalValue; }

  virtual vesKiwiTransition::TransitionStatus willBegin();
  virtual vesKiwiTransition::TransitionStatus prepareState(double state);

protected:
  T Helper;

  typename T::ValueType InitialValue;
  bool InitialValueSet;

  typename T::ValueType FinalValue;
  bool FinalValueSet;
};

template<typename T>
typename vesKiwiIVarTransition<T>::Ptr vesKiwiIVarTransition<T>::create(
  typename T::ClassPtrType instance)
{
  typename vesKiwiIVarTransition<T>::Ptr transition;
  return vesKiwiIVarTransition<T>::create(instance, transition);
}

template<typename T>
typename vesKiwiIVarTransition<T>::Ptr vesKiwiIVarTransition<T>::create(
  typename T::ClassPtrType instance,
  typename vesKiwiIVarTransition<T>::Ptr& transition)
{
  T helper(instance);
  transition =
    typename vesKiwiIVarTransition<T>::Ptr(
      new vesKiwiIVarTransition<T>(helper));

  return transition;
}

template<typename T>
vesKiwiIVarTransition<T>::vesKiwiIVarTransition(const T& helper)
  : Helper(helper)
{
  this->InitialValueSet = false;
  this->FinalValueSet = false;
}

template<typename T>
vesKiwiIVarTransition<T>::~vesKiwiIVarTransition()
{
}

template<typename T>
typename vesKiwiIVarTransition<T>::Ptr
vesKiwiIVarTransition<T>::setInitialValue(const typename T::ValueType& v)
{
  this->InitialValue = v;
  this->InitialValueSet = true;
  return shared_from_this();
}

template<typename T>
typename vesKiwiIVarTransition<T>::Ptr
vesKiwiIVarTransition<T>::takeInitialValue()
{
  this->InitialValue = this->Helper.get();
  this->InitialValueSet = true;
  return shared_from_this();
}

template<typename T>
typename vesKiwiIVarTransition<T>::Ptr
vesKiwiIVarTransition<T>::setFinalValue(const typename T::ValueType& v)
{
  this->FinalValue = v;
  this->FinalValueSet = true;
  return shared_from_this();
}

template<typename T>
typename vesKiwiIVarTransition<T>::Ptr
vesKiwiIVarTransition<T>::takeFinalValue()
{
  this->FinalValue = this->Helper.get();
  this->FinalValueSet = true;
  return shared_from_this();
}

template<typename T>
vesKiwiTransition::TransitionStatus vesKiwiIVarTransition<T>::willBegin()
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
vesKiwiTransition::TransitionStatus
vesKiwiIVarTransition<T>::prepareState(double state)
{
  this->Helper.set(state * this->FinalValue + (1. - state) * this->InitialValue);
  return vesKiwiTransition::TRANSITION_OK;
}

#endif // __vesKiwiIVarTransition_h
