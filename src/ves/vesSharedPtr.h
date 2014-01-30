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

#ifndef VESSHAREDPTR_H
#define VESSHAREDPTR_H

// C/C++ includes
/* Where is the shared_ptr declaration? (What header and what namespace?)
 * We must include <ciso646> (or some other C++ header) in order to make
 * it clear to some compilers (clang on OS X 10.9) that C++ definitions
 * (and _LIBCPP_VERSION in particular) should be turned on.
 */
#include <ciso646>
#ifdef _MSC_VER

#  define VES_MEMORY_HDR <memory>
#  define VES_SPTR_NS std

#elif defined(_LIBCPP_VERSION)
/* libc++ does not implement the tr1 namespace as it provides
 * only C++11 support; instead of tr1, the equivalent functionality
 * is placed in namespace std, so use when it targeting such
 * systems (OS X 10.7 onwards, various Linux distributions) */

#  define VES_MEMORY_HDR <memory>
#  define VES_SPTR_NS std

#else

#  define VES_MEMORY_HDR <tr1/memory>
#  define VES_SPTR_NS std::tr1

#endif

#include VES_MEMORY_HDR

/**\brief An abbreviation for a shared pointer to a class.
  *
  * Use this macro like so: <pre>
  *   class X;
  *   vesSharedPtr<X> = vesSharedPtr<X>(new X);</pre>
  *
  * It is possible on some platforms this might get defined
  * to something other than std::shared_ptr (such as
  * boost_ptr). But mainly this macro exists because it
  * is easier to type.
  */
#define vesSharedPtr VES_SPTR_NS::shared_ptr

/// An abbreviation for dynamically-casting a shared_ptr.
#define vesDynamicPtrCast VES_SPTR_NS::dynamic_pointer_cast

/// An abbreviation for statically-casting a shared_ptr.
#define vesStaticPtrCast VES_SPTR_NS::static_pointer_cast

/**\brief An abbreviation for a weak pointer to a class.
  *
  * \sa vesSharedPtr
  */
#define vesWeakPtr VES_SPTR_NS::weak_ptr

/**\brief Add static create() methods to a class.
  *
  * This macro takes a single parameter naming either
  * the class of interest (if no ancestor classes use
  * enable_shared_from_this() or vesEnableSharedPtr())
  * or the first -- and only -- ancestor class that inherits
  * enable_shared_from_this() or vesEnableSharedPtr().
  *
  * This macro also requires the use of vesTypeMacro()
  * as it needs SelfType defined.
  *
  * Two static class functions are declared: both return
  * a shared pointer to a newly created class instance
  * but one takes no arguments and the other takes one.
  * The 1-argument version also sets the passed shared
  * pointer to refer to the newly created instance.
  * This is useful in declarative APIs for referring
  * back to an instance created as part of a statement
  * whose value is transformed before the returned value
  * can be assigned to a variable.
  */
#define vesCreateMacro(...) \
  static vesSharedPtr<SelfType> create() \
    { \
    vesSharedPtr< __VA_ARGS__ > shared(new SelfType); \
    return VES_SPTR_NS::static_pointer_cast<SelfType>(shared); \
    } \
  /* variant for declarative programming: */ \
  static vesSharedPtr<SelfType> create(vesSharedPtr<SelfType>& ref) \
    { \
    ref = SelfType::create(); \
    return ref; \
    }

/**\brief An abbreviation for enabling shared pointers.
  *
  * Use like so:<pre>
  * class X : vesEnableSharedPtr(X)
  * {
  * public:
  *   vesTypeMacro(X);
  *   vesCreateMacro(X);
  * };
  * </pre>
  * Note that this may be complicated on some systems by the
  * C preprocessor's inability to handle macros whose arguments
  * include a multiple-parameter template. (The comma separating
  * the template parameters is taken as an additional macro
  * argument.)
  * However, in general shared pointers will be enabled on
  * non-templated base classes from which templated classes may
  * be derived.
  *
  * It is recommended that you make constructors protected or private
  * to avoid heap allocation of objects that may return shared pointers.
  *
  * For non-abstract classes, it is recommended that you
  * call vesCreateMacro() as a safe way to expose public construction.
  */
#define vesEnableSharedPtr(...) \
  public VES_SPTR_NS::enable_shared_from_this< __VA_ARGS__ >

/**\brief A macro to help with derived classes whose bases enable shared_from_this().
  *
  * Use like so:<pre>
  * class X : vesEnableSharedPtr(X)
  * {
  * public:
  *   vesTypeMacro(X);
  * };
  *
  * class Y : public X
  * {
  * public:
  *   vesTypeMacro(Y);
  *   vesSharedFromThisMacro(X);
  *   ...
  *   Y::Ptr method()
  *     {
  *     return shared_from_this();
  *     }
  * };
  * </pre>
  *
  * Note that the macro argument is the <b>base class</b>
  * on which shared pointers are enabled (or another
  * inherited class in between which also defines a
  * shared_from_this() method).
  *
  * This macro implements a shared_from_this() method
  * in the derived class that returns a shared pointer
  * of the proper type.
  */
#define vesSharedFromThisMacro(...) \
  typedef __VA_ARGS__ SharedPtrBaseType; \
  vesSharedPtr<SelfType> shared_from_this() \
    { \
    return VES_SPTR_NS::static_pointer_cast<SelfType>( \
      SharedPtrBaseType::shared_from_this()); \
    }

/// A convenience macro for declaring shared_from_this and create methods.
#define vesSharedPtrCreateMacro(...) \
  vesSharedFromThisMacro( __VA_ARGS__ ); \
  vesCreateMacro( __VA_ARGS__ );

/**\brief A convenience macro to use in the body of create methods that take arguments.
  *
  * This macro acts like a function that takes a pointer
  * to an instance of your class and returns a shared pointer
  * to the instance. For example: <pre>
  * class X : vesEnabledSharedPtr(X)
  * {
  * public:
  *   vesTypeMacro(X);
  *   static Ptr create(int a, double b);
  * protected:
  *   X(int a, double b);
  * };
  *
  * X::Ptr create(int a, double b)
  * {
  *   return vesSharedPtrHelper(
  *     new X(a, b));
  * }
  * </pre>
  *
  * It is important to use this method in classes derived
  * from those that use vesEnabledSharedPtr rather than
  * naively constructing a shared pointer of the proper type,
  * since that will likely result in an exception being thrown.
  */
#define vesSharedPtrHelper(...) \
  VES_SPTR_NS::static_pointer_cast<SelfType>( \
    SharedPtrBaseType::Ptr( \
    __VA_ARGS__ \
    ) \
  );

#endif // VESSHAREDPTR_H
