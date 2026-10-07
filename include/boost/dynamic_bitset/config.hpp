// -----------------------------------------------------------
//
//   Copyright (c) 2001-2002 Chuck Allison and Jeremy Siek
//      Copyright (c) 2003-2006, 2008, 2025 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

#ifndef BOOST_DYNAMIC_BITSET_CONFIG_HPP_GP_20040424
#define BOOST_DYNAMIC_BITSET_CONFIG_HPP_GP_20040424

#include "boost/config.hpp"

#if ! defined( BOOST_NO_CXX11_HDR_FUNCTIONAL ) && ! defined( BOOST_DYNAMIC_BITSET_NO_STD_HASH )
#    define BOOST_DYNAMIC_BITSET_SPECIALIZE_STD_HASH
#endif

#if BOOST_CXX_VERSION >= 202002L
#    define BOOST_DYNAMIC_BITSET_CONSTEXPR20 constexpr
#else
#    define BOOST_DYNAMIC_BITSET_CONSTEXPR20
#endif

#endif // include guard
