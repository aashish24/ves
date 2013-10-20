#ifndef __vesKiwiEasing_h
#define __vesKiwiEasing_h

#include "vesObject.h"

#include <cmath>

/// \brief Provide a virtual evaluate method that mirrors a class-static method.
///
/// For simple easing functions that do not store any state as
/// instance variables, the easing function is generally implemented
/// as a static method named "eval" so that it can be used without
/// creating a class instance. This macro provides a virtual override
/// of vesKiwiEasing::evaluate() which simply calls the static method.
#define vesKiwiEasingEvaluate(className) \
  virtual double evaluate(double t) const \
    { \
    return className ::eval(t); \
    }

/// \brief A base class for easing functions.
///
/// Easings are functions composed with a linear ramp from 0 to 1
/// in order to provide nonlinear animations without encoding
/// fixed nonlinear behavior in each animation.
///
/// They work by mapping the domain [0,1] into a range (usually [0,1],
/// in which case the transform is automorphic).
/// Subclasses provide many different mappings, each of which may be
/// configured and used with vesKiwiTransition instances rather than
/// including their effect in the transition object.
class vesKiwiEasing : public vesObject, vesEnableSharedPtr(vesKiwiEasing)
{
public:
  vesTypeMacro(vesKiwiEasing);

  /// Transform \a t (in the domain [0,1]) into a new return value (<b>usually</b> also in [0,1]).
  virtual double evaluate(double t) const = 0;

  /// \brief Will the easing evaluate to an out-of-range number?
  ///
  /// Returns a non-zero value if there is a <b>possibility</b>
  /// that some \a t in [0,1] that will be mapped outside [0,1].
  /// It should never return 0 unless it can guarantee there
  /// will be no overshoot.
  /// However, it may return a non-zero value on some occasions
  /// where there may not be overshoot.
  virtual int doesOvershoot() const = 0;
};

/// \brief A two-stage easing with a programmable midpoint.
///
/// This is used to construct "InOut" versions of easings
/// from their "In" and "Out" brethren.
/// The template parameters the class names of the two
/// easings to piece together and they may be configured
/// using the easeIn() and easeOut() methods that return
/// instances held by this two-stage easing.
template<typename Ein, typename Eout>
class vesKiwiTwoStageEasing : public vesKiwiEasing
{
public:
  vesTypeMacro(vesKiwiTwoStageEasing<Ein,Eout>);
  vesSharedPtrCreateMacro(vesKiwiEasing);

  vesKiwiTwoStageEasing() {
    this->EaseIn = Ein::create();
    this->EaseOut = Eout::create();
    this->SplitPoint = 0.5;
    this->SplitValue = 0.5;
  }
  vesKiwiTwoStageEasing(double splitPoint, double splitValue) {
    this->EaseIn = Ein::create();
    this->EaseOut = Eout::create();
    this->SplitPoint = splitPoint;
    this->SplitValue = splitValue;
  }
  virtual ~vesKiwiTwoStageEasing() { }

  /// Return a reference to the easing function used for the first stage.
  typename Ein::Ptr easeIn() { return this->EaseIn; }
  typename Ein::ConstPtr easeIn() const { return this->EaseIn; }

  /// Return a reference to the easing function used for the second stage.
  typename Ein::Ptr easeOut() { return this->EaseOut; }
  typename Ein::ConstPtr easeOut() const { return this->EaseOut; }

  /// Set/get the fraction of time the first stage is in effect.
  ///
  /// The fraction must be in the open interval ]0,1[.
  /// The default is 0.5.
  //@{
  double& splitPoint() { return this->SplitPoint; }
  double splitPoint() const { return this->SplitPoint; }
  typename vesKiwiTwoStageEasing<Ein,Eout>::Ptr setSplitPoint(double s) {
    this->SplitPoint = s;
    return shared_from_this();
  }
  //@}

  /// Set/get the scale factor applied to the first stage's output.
  ///
  /// The second stage scale factor is (1 - splitValue()).
  ///
  /// The fraction must be in the open interval ]0,1[ and defaults
  /// to 0.5.
  //@{
  double& splitValue() { return this->SplitValue; }
  double splitValue() const { return this->SplitValue; }
  typename vesKiwiTwoStageEasing<Ein,Eout>::Ptr setSplitValue(double s) {
    this->SplitValue = s;
    return shared_from_this();
  }
  //@}

  virtual double evaluate(double t) const {
    double ds = t - this->SplitPoint;
    return
      ds <= 0. ?
      this->EaseIn->evaluate(t / this->SplitPoint) * this->SplitValue :
      this->SplitValue + this->EaseOut->evaluate(ds / (1. - this->SplitPoint)) * (1. - this->SplitValue);
  }

  virtual int doesOvershoot() const {
    return
      this->EaseIn->doesOvershoot() ||
      this->EaseOut->doesOvershoot() ||
      this->SplitPoint < 0. || this->SplitPoint > 1. ||
      this->SplitValue < 0. || this->SplitValue > 1.
      ;
  }

protected:
  typename Ein::Ptr EaseIn;
  typename Eout::Ptr EaseOut;
  double SplitPoint;
  double SplitValue;
};

/// \brief A linear-in, linear-out easing with no adjustable midpoint or rate limit.
///
/// This is the equivalent of no easing at all.
/// There is no overshoot.
class vesKiwiLinearInOutEasing : public vesKiwiEasing
{
public:
  vesTypeMacro(vesKiwiLinearInOutEasing);
  vesSharedPtrCreateMacro(vesKiwiEasing);

  static double eval(double t) { return t; }
  vesKiwiEasingEvaluate(vesKiwiLinearInOutEasing);

  virtual int doesOvershoot() const { return 0; }
};

/// \brief A quadratic-in easing.
class vesKiwiQuadraticInEasing : public vesKiwiEasing
{
public:
  vesTypeMacro(vesKiwiQuadraticInEasing);
  vesSharedPtrCreateMacro(vesKiwiEasing);

  static double eval(double t) { return t * t; }
  vesKiwiEasingEvaluate(vesKiwiQuadraticInEasing);

  virtual int doesOvershoot() const { return 0; }
};

/// \brief A quadratic-out easing.
class vesKiwiQuadraticOutEasing : public vesKiwiEasing
{
public:
  vesTypeMacro(vesKiwiQuadraticOutEasing);
  vesSharedPtrCreateMacro(vesKiwiEasing);

  static double eval(double t) {
    double s = 1. - t;
    return 1. - s * s;
  }
  vesKiwiEasingEvaluate(vesKiwiQuadraticOutEasing);

  virtual int doesOvershoot() const { return 0; }
};

/// \brief A quadratic-in, quadratic-out easing.
typedef vesKiwiTwoStageEasing<vesKiwiQuadraticInEasing,vesKiwiQuadraticOutEasing> vesKiwiQuadraticInOutEasing;

/// \brief A polynomial-in easing.
template<int Degree>
class vesKiwiPolynomialInEasing : public vesKiwiEasing
{
public:
  vesTypeMacro(vesKiwiPolynomialInEasing<Degree>);
  vesSharedPtrCreateMacro(vesKiwiEasing);

  static double eval(double t) { return pow(t, Degree); }
  vesKiwiEasingEvaluate(vesKiwiPolynomialInEasing<Degree>);

  virtual int doesOvershoot() const { return 0; }
};

/// \brief A polynomial-out easing.
template<int Degree>
class vesKiwiPolynomialOutEasing : public vesKiwiEasing
{
public:
  vesTypeMacro(vesKiwiPolynomialOutEasing<Degree>);
  vesSharedPtrCreateMacro(vesKiwiEasing);

  static double eval(double t) { return 1. - pow(1. - t, Degree); }
  vesKiwiEasingEvaluate(vesKiwiPolynomialOutEasing<Degree>);

  virtual int doesOvershoot() const { return 0; }
};

/// \brief A polynomial-in, polynomial-out easing.
template<int Din, int Dout>
class vesKiwiPolynomialInOutEasing :
  public vesKiwiTwoStageEasing<vesKiwiPolynomialInEasing<Din>,vesKiwiPolynomialOutEasing<Dout> >
{
public:
  vesTypeMacro(vesKiwiPolynomialInOutEasing<Din,Dout>);
  vesSharedPtrCreateMacro(vesKiwiEasing);
};

/// \brief A cubic-in easing.
typedef vesKiwiPolynomialInEasing<3> vesKiwiCubicInEasing;
/// \brief A cubic-out easing.
typedef vesKiwiPolynomialOutEasing<3> vesKiwiCubicOutEasing;
/// \brief A cubic-in-out easing.
typedef vesKiwiPolynomialInOutEasing<3,3> vesKiwiCubicInOutEasing;

/// \brief A quartic-in easing.
typedef vesKiwiPolynomialInEasing<4> vesKiwiQuarticInEasing;
/// \brief A quartic-out easing.
typedef vesKiwiPolynomialOutEasing<4> vesKiwiQuarticOutEasing;
/// \brief A quartic-in-out easing.
typedef vesKiwiPolynomialInOutEasing<4,4> vesKiwiQuarticInOutEasing;

/// \brief A quintic-in easing.
typedef vesKiwiPolynomialInEasing<5> vesKiwiQuinticInEasing;
/// \brief A quintic-out easing.
typedef vesKiwiPolynomialOutEasing<5> vesKiwiQuinticOutEasing;
/// \brief A quintic-in-out easing.
typedef vesKiwiPolynomialInOutEasing<5,5> vesKiwiQuinticInOutEasing;

/// \brief A cubic-bezier easing.
///
/// Note that the first and last control point
/// values (p0 & p3) should be 0 and 1, respectively,
/// for any sane easing.
class vesKiwiCubicBezierEasing : public vesKiwiEasing
{
public:
  vesTypeMacro(vesKiwiCubicBezierEasing);
  vesSharedPtrCreateMacro(vesKiwiEasing);

  vesKiwiCubicBezierEasing()
    : P0(0.), P1(0.5), P2(0.25), P3(1.) { }
  vesKiwiCubicBezierEasing(double p0, double p1, double p2, double p3)
    : P0(p0), P1(p1), P2(p2), P3(p3) { }

  vesKiwiCubicBezierEasing::Ptr setCoefficients(
    double p0, double p1, double p2, double p3) {
    this->P0 = p0;
    this->P1 = p1;
    this->P2 = p2;
    this->P3 = p3;
    return shared_from_this();
  }

  static double eval(
    double t, double p0, double p1, double p2, double p3) {
    double t2 = t * t;
    double s = 1. - t;
    double s2 = s * s;
    return
      s2 * s * p0 +
      3 * s2 * t * p1 +
      3 * t2 * s * p2 +
      t2 * t * p3;
  }

  virtual double evaluate(double t) const {
    return vesKiwiCubicBezierEasing::eval(
      t, this->P0, this->P1, this->P2, this->P3);
  }

  /// Will the curve with the given coefficients stay in [0,1] for inputs in [0,1]?
  static int evalOvershoot(
    double p0, double p1, double p2, double p3) {
    // TODO: Bounds could be tighter, but this will never
    // return 0 when it should return 1.
    bool r0 = (p0 >= 0. && p0 <= 1.);
    bool r1 = (p1 >= 0. && p1 <= 1.);
    bool r2 = (p2 >= 0. && p2 <= 1.);
    bool r3 = (p3 >= 0. && p3 <= 1.);
    return (r0 && r1 && r2 && r3) ? 0 : 1;
  }

  virtual int doesOvershoot() const {
    return vesKiwiCubicBezierEasing::evalOvershoot(
      this->P0, this->P1, this->P2, this->P3);
  }

protected:
  double P0;
  double P1;
  double P2;
  double P3;
};

/// \brief A back-in-out easing (with overshoot)
///
/// The coefficients were taken from http://easings.net/ on 2013-08-16.
class vesKiwiBackInOutEasing :
  public vesKiwiTwoStageEasing<vesKiwiCubicBezierEasing,vesKiwiCubicBezierEasing>
{
public:
  vesTypeMacro(vesKiwiBackInOutEasing);
  vesSharedPtrCreateMacro(vesKiwiEasing);

  vesKiwiBackInOutEasing()
    {
    this->EaseIn->setCoefficients(0., -0.28, 0.045, 1.);
    this->EaseOut->setCoefficients(0., 0.885, 1.275, 1.);
    }
};

/// \brief Bounce easings.
///
/// The coefficients were taken from
/// http://gsgd.co.uk/sandbox/jquery/easing/jquery.easing.1.3.js
/// on 2013-08-16.
///
/// TERMS OF USE - EASING EQUATIONS
///
/// Open source under the BSD License.
///
/// Copyright Â© 2001 Robert Penner
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without modification,
/// are permitted provided that the following conditions are met:
///
/// Redistributions of source code must retain the above copyright notice, this list of
/// conditions and the following disclaimer.
/// Redistributions in binary form must reproduce the above copyright notice, this list
/// of conditions and the following disclaimer in the documentation and/or other materials
/// provided with the distribution.
///
/// Neither the name of the author nor the names of contributors may be used to endorse
/// or promote products derived from this software without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
/// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
/// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
/// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
/// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
/// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
/// AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
/// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
/// OF THE POSSIBILITY OF SUCH DAMAGE.
//@{
/// \brief A bounce-in easing using 4 quadratic curves.
class vesKiwiBounceInEasing : public vesKiwiEasing
{
public:
  vesTypeMacro(vesKiwiBounceInEasing);
  vesSharedPtrCreateMacro(vesKiwiEasing);

  static double eval(double t) {
    t = 1. - t;
    if (t < (1/2.75)) {
      return 1. - 7.5625*t*t;
    } else if (t < (2/2.75)) {
      t-=(1.5/2.75);
      return 1. - (7.5625*t*t + .75);
    } else if (t < (2.5/2.75)) {
      t-=(2.25/2.75);
      return 1. - (7.5625*t*t + .9375);
    } else {
      t-=(2.625/2.75);
      return 1. - (7.5625*t*t + .984375);
    }
  }
  vesKiwiEasingEvaluate(vesKiwiBounceInEasing);

  virtual int doesOvershoot() const { return 0; }
};

/// \brief A bounce-out easing using 4 quadratic curves.
class vesKiwiBounceOutEasing : public vesKiwiEasing
{
public:
  vesTypeMacro(vesKiwiBounceOutEasing);
  vesSharedPtrCreateMacro(vesKiwiEasing);

  static double eval(double t) {
    if (t < (1/2.75)) {
      return 7.5625*t*t;
    } else if (t < (2/2.75)) {
      t-=(1.5/2.75);
      return 7.5625*t*t + .75;
    } else if (t < (2.5/2.75)) {
      t-=(2.25/2.75);
      return 7.5625*t*t + .9375;
    } else {
      t-=(2.625/2.75);
      return 7.5625*t*t + .984375;
    }
  }
  vesKiwiEasingEvaluate(vesKiwiBounceOutEasing);

  virtual int doesOvershoot() const { return 0; }
};

/// \brief A bounce-in-out easing using piecewise quadratic curves.
typedef vesKiwiTwoStageEasing<vesKiwiBounceInEasing,vesKiwiBounceOutEasing> vesKiwiBounceInOutEasing;
//@}

#endif // __vesKiwiEasing_h
