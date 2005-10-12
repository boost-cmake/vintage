///////////////////////////////////////////////////////////////////////////////
// type_traits.hpp
//
//  Copyright 2004 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_XPRESSIVE_DETAIL_STATIC_TYPE_TRAITS_HPP_EAN_10_04_2005
#define BOOST_XPRESSIVE_DETAIL_STATIC_TYPE_TRAITS_HPP_EAN_10_04_2005

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <boost/ref.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/xpressive/detail/detail_fwd.hpp>

namespace boost { namespace xpressive { namespace detail
{

///////////////////////////////////////////////////////////////////////////////
// is_static_xpression
//
template<typename T>
struct is_static_xpression
  : mpl::false_
{
};

template<typename MatcherT, typename NextT>
struct is_static_xpression<static_xpression<MatcherT, NextT> >
  : mpl::true_
{
};

template<typename TopT, typename NextT>
struct is_static_xpression<stacked_xpression<TopT, NextT> >
  : mpl::true_
{
};

///////////////////////////////////////////////////////////////////////////////
// is_regex
//
template<typename XprT>
struct is_regex
  : mpl::false_
{
};

template<typename BidiIterT>
struct is_regex<basic_regex<BidiIterT> >
  : mpl::true_
{
};

template<typename BidiIterT>
struct is_regex<reference_wrapper<basic_regex<BidiIterT> > >
  : mpl::true_
{
};

///////////////////////////////////////////////////////////////////////////////
// is_modifiable
//
template<typename XprT>
struct is_modifiable
  : mpl::not_<is_regex<XprT> >
{
};

//////////////////////////////////////////////////////////////////////////
// is_random
//
template<typename BidiIterT>
struct is_random
  : is_convertible
    <
        typename iterator_category<BidiIterT>::type
      , std::random_access_iterator_tag
    >
{
};

}}} // namespace boost::xpressive::detail

#endif