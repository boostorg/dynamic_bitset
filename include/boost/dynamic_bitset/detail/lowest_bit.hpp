// -----------------------------------------------------------
//                       lowest_bit()
//
//          Position of the lowest bit that is set.
//
//      Copyright (c) 2003-2004, 2008, 2025 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

#ifndef BOOST_LOWEST_BIT_HPP_GP_20030301
#define BOOST_LOWEST_BIT_HPP_GP_20030301

#include "boost/assert.hpp"
#include "boost/integer/integer_log2.hpp"

namespace boost {
namespace detail {

template <typename T>
int
lowest_bit(T x)
{
    BOOST_ASSERT(x >= 1);

    // Clear all the bits that are set except the rightmost one,
    // then calculate the logarithm to base 2.
    //
    return boost::integer_log2<T>(x - (x & (x - 1)));
}

}
}

#endif // include guard
