// -----------------------------------------------------------
//
//     Position of the lowest or highest bit that is set.
//
//   Copyright (c) 2003-2004, 2008, 2025-2026 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

#ifndef BOOST_LOWEST_HIGHEST_BIT_HPP_GP_20260109
#define BOOST_LOWEST_HIGHEST_BIT_HPP_GP_20260109

#include "boost/assert.hpp"
#include "boost/core/bit.hpp"
#include <limits>
#include <type_traits>

namespace boost {
namespace detail {

template< typename T >
int
lowest_bit( T x )
{
    BOOST_ASSERT( x >= 1 );

    return boost::core::countr_zero( static_cast< typename std::make_unsigned< T >::type >( x ) );
}

template< typename T >
int
highest_bit( T x )
{
    BOOST_ASSERT( x >= 1 );

    using Unsigned = typename std::make_unsigned< T >::type;
    return ( std::numeric_limits< Unsigned >::digits - 1 ) - boost::core::countl_zero( static_cast< Unsigned >( x ) );
}

}
}

#endif // include guard
