// -----------------------------------------------------------
// lowest_bit.hpp
//
//           Position of the lowest bit 'on'
//
//         Copyright (c) 2003-2004, 2008 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

#ifndef BOOST_LOWEST_BIT_HPP_GP_20030301
#define BOOST_LOWEST_BIT_HPP_GP_20030301

#include <assert.h>
#include "boost/pending/integer_log2.hpp"


namespace boost {

    template <typename T>
    int lowest_bit(T x) {

        assert(x >= 1); // PRE

        // clear all bits on except the rightmost one,
        // then calculate the logarithm base 2
        //
        return boost::integer_log2<T>( x - ( x & (x-1) ) );

    }

#if defined(BOOST_MSVC) && (defined(_M_IX86) || defined(_M_X64))
    template<>
    BOOST_FORCEINLINE int lowest_bit(unsigned int x) {
        assert(x >= 1); // PRE

        unsigned long result;
        _BitScanForward(&result, x);
        return result;
    }

    template<>
    BOOST_FORCEINLINE int lowest_bit(unsigned long x) {
        assert(x >= 1); // PRE

        unsigned long result;
        _BitScanForward(&result, x);
        return result;
    }

#ifdef _M_X64
    template<>
    BOOST_FORCEINLINE int lowest_bit(unsigned long long x) {
        assert(x >= 1); // PRE

        unsigned long result;
        _BitScanForward64(&result, x);
        return result;
    }
#endif

#elif defined(BOOST_GCC) || defined(__clang__) || (defined(BOOST_INTEL) && defined(__GNUC__))
    template<>
    BOOST_FORCEINLINE int lowest_bit(unsigned int x) {
        assert(x >= 1); // PRE

        return __builtin_ctz( x );
    }

    template<>
    BOOST_FORCEINLINE int lowest_bit(unsigned long x) {
        assert(x >= 1); // PRE

        return __builtin_ctzl( x );
    }

    template<>
    BOOST_FORCEINLINE int lowest_bit(unsigned long long x) {
        assert(x >= 1); // PRE

        return __builtin_ctzll( x );
    }
#endif

}


#endif // include guard
