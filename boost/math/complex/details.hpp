//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_COMPLEX_DETAILS_INCLUDED
#define BOOST_MATH_COMPLEX_DETAILS_INCLUDED
//
// This header contains all the support code that is common to the
// inverse trig complex functions, it also contains all the includes
// that we need to implement all these functions.
//
#include <complex>
#include <boost/limits.hpp>
#include <math.h> // isnan where available
#include <cmath>

namespace boost{ namespace math{ namespace detail{

template <class T>
inline std::complex<T> mult_i(const std::complex<T>& t)
{
   return std::complex<T>(-t.imag(), t.real());
}

template <class T>
inline std::complex<T> mult_minus_i(const std::complex<T>& t)
{
   return std::complex<T>(t.imag(), -t.real());
}

template <class T>
inline bool test_is_nan(T t)
{
   // Comparisons with Nan's always fail:
   return !(t <= std::numeric_limits<T>::infinity());
}
#ifdef isnan
template<> inline bool test_is_nan<float>(float t) { return isnan(t); }
template<> inline bool test_is_nan<double>(double t) { return isnan(t); }
template<> inline bool test_is_nan<long double>(long double t) { return isnan(t); }
#endif


} } } // namespaces

#endif // BOOST_MATH_COMPLEX_DETAILS_INCLUDED