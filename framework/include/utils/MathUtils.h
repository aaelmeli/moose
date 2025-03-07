//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Moose.h"
#include "MooseError.h"
#include "libmesh/libmesh.h"
#include "libmesh/utility.h"
#include "libmesh/numeric_vector.h"
#include "libmesh/compare_types.h"

namespace MathUtils
{

Real poly1Log(Real x, Real tol, unsigned int derivative_order);
Real poly2Log(Real x, Real tol, unsigned int derivative_order);
Real poly3Log(Real x, Real tol, unsigned int derivative_order);
Real poly4Log(Real x, Real tol, unsigned int derivative_order);
Real taylorLog(Real x);

template <typename T>
T
round(T x)
{
  return ::round(x); // use round from math.h
}

template <typename T>
T
sign(T x)
{
  return x >= 0.0 ? 1.0 : -1.0;
}

template <typename T>
T
pow(T x, int e)
{
  bool neg = false;
  T result = 1.0;

  if (e < 0)
  {
    neg = true;
    e = -e;
  }

  while (e)
  {
    // if bit 0 is set multiply the current power of two factor of the exponent
    if (e & 1)
      result *= x;

    // x is incrementally set to consecutive powers of powers of two
    x *= x;

    // bit shift the exponent down
    e >>= 1;
  }

  return neg ? 1.0 / result : result;
}

template <typename T>
T
heavyside(T x)
{
  return x < 0.0 ? 0.0 : 1.0;
}

template <typename T>
T
regularizedHeavyside(T x, Real smoothing_length)
{
  if (x <= -smoothing_length)
    return 0.0;
  else if (x < smoothing_length)
    return 0.5 * (1 + std::sin(libMesh::pi * x / 2 / smoothing_length));
  else
    return 1.0;
}

template <typename T>
T
regularizedHeavysideDerivative(T x, Real smoothing_length)
{
  if (x < smoothing_length && x > -smoothing_length)
    return 0.25 * libMesh::pi / smoothing_length *
           (std::cos(libMesh::pi * x / 2 / smoothing_length));
  else
    return 0.0;
}

template <typename T>
T
positivePart(T x)
{
  return x > 0.0 ? x : 0.0;
}

template <typename T>
T
negativePart(T x)
{
  return x < 0.0 ? x : 0.0;
}

template <typename T,
          typename T2,
          typename T3,
          typename std::enable_if<ScalarTraits<T>::value && ScalarTraits<T2>::value &&
                                      ScalarTraits<T3>::value,
                                  int>::type = 0>
void
addScaled(const T & a, const T2 & b, T3 & result)
{
  result += a * b;
}

template <typename T,
          typename T2,
          typename T3,
          typename std::enable_if<ScalarTraits<T>::value, int>::type = 0>
void
addScaled(const T & scalar, const NumericVector<T2> & numeric_vector, NumericVector<T3> & result)
{
  result.add(scalar, numeric_vector);
}

template <
    typename T,
    typename T2,
    template <typename>
    class W,
    template <typename>
    class W2,
    typename std::enable_if<std::is_same<typename W<T>::index_type, unsigned int>::value &&
                                std::is_same<typename W2<T2>::index_type, unsigned int>::value,
                            int>::type = 0>
typename libMesh::CompareTypes<T, T2>::supertype
dotProduct(const W<T> & a, const W2<T2> & b)
{
  return a * b;
}

template <typename T,
          typename T2,
          template <typename>
          class W,
          template <typename>
          class W2,
          typename std::enable_if<std::is_same<typename W<T>::index_type,
                                               std::tuple<unsigned int, unsigned int>>::value &&
                                      std::is_same<typename W2<T2>::index_type,
                                                   std::tuple<unsigned int, unsigned int>>::value,
                                  int>::type = 0>
typename libMesh::CompareTypes<T, T2>::supertype
dotProduct(const W<T> & a, const W2<T2> & b)
{
  return a.contract(b);
}

/**
 * Evaluate a polynomial with the coefficients c at x. Note that the Polynomial
 * form is
 *   c[0]*x^s + c[1]*x^(s-1) + c[2]*x^(s-2) + ... + c[s-2]*x^2 + c[s-1]*x + c[s]
 * where s = c.size()-1 , which is counter intuitive!
 *
 * This function will be DEPRECATED soon (10/22/2020)
 *
 * The coefficient container type can be any container that provides an index
 * operator [] and a .size() method (e.g. std::vector, std::array). The return
 * type is the supertype of the container value type and the argument x.
 * The supertype is the type that can represent both number types.
 */
template <typename C,
          typename T,
          typename R = typename libMesh::CompareTypes<typename C::value_type, T>::supertype>
R
poly(const C & c, const T x, const bool derivative = false)
{
  const auto size = c.size();
  if (size == 0)
    return 0.0;

  R value = c[0];
  if (derivative)
  {
    value *= size - 1;
    for (std::size_t i = 1; i < size - 1; ++i)
      value = value * x + c[i] * (size - i - 1);
  }
  else
  {
    for (std::size_t i = 1; i < size; ++i)
      value = value * x + c[i];
  }

  return value;
}

/**
 * Evaluate a polynomial with the coefficients c at x. Note that the Polynomial
 * form is
 *   c[0] + c[1] * x + c[2] * x^2 + ...
 * The coefficient container type can be any container that provides an index
 * operator [] and a .size() method (e.g. std::vector, std::array). The return
 * type is the supertype of the container value type and the argument x.
 * The supertype is the type that can represent both number types.
 */
template <typename C,
          typename T,
          typename R = typename libMesh::CompareTypes<typename C::value_type, T>::supertype>
R
polynomial(const C & c, const T x)
{
  auto size = c.size();
  if (size == 0)
    return 0.0;

  size--;
  R value = c[size];
  for (std::size_t i = 1; i <= size; ++i)
    value = value * x + c[size - i];

  return value;
}

/**
 * Returns the derivative of polynomial(c, x) with respect to x
 */
template <typename C,
          typename T,
          typename R = typename libMesh::CompareTypes<typename C::value_type, T>::supertype>
R
polynomialDerivative(const C & c, const T x)
{
  auto size = c.size();
  if (size <= 1)
    return 0.0;

  size--;
  R value = c[size] * size;
  for (std::size_t i = 1; i < size; ++i)
    value = value * x + c[size - i] * (size - i);

  return value;
}

template <typename T, typename T2>
T
clamp(const T & x, T2 lowerlimit, T2 upperlimit)
{
  if (x < lowerlimit)
    return lowerlimit;
  if (x > upperlimit)
    return upperlimit;
  return x;
}

template <typename T, typename T2>
T
smootherStep(T x, T2 start, T2 end, bool derivative = false)
{
  if (end == start)
    return 0.0;
  x = clamp((x - start) / (end - start), 0.0, 1.0);
  if (x == 0.0)
    return 0.0;
  if (derivative)
  {
    if (x == 1.0)
      return 0.0;
    return 30.0 * Utility::pow<2>(x) * (x * (x - 2.0) + 1.0) / (end - start);
  }
  if (x == 1.0)
    return 1.0;
  return Utility::pow<3>(x) * (x * (x * 6.0 - 15.0) + 10.0);
}

/**
 * Helper function templates to set a variable to zero.
 * Specializations may have to be implemented (for examples see
 * RankTwoTensor, RankFourTensor).
 */
template <typename T>
inline void
mooseSetToZero(T & v)
{
  /**
   * The default for non-pointer types is to assign zero.
   * This should either do something sensible, or throw a compiler error.
   * Otherwise the T type is designed badly.
   */
  v = 0;
}
template <typename T>
inline void
mooseSetToZero(T *&)
{
  mooseError("mooseSetToZero does not accept pointers");
}

template <>
inline void
mooseSetToZero(std::vector<Real> & vec)
{
  for (auto & v : vec)
    v = 0.;
}

} // namespace MathUtils
