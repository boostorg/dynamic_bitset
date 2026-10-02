// -----------------------------------------------------------
//              Copyright (c) 2026 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

// Checks that the library can be used when the compiler is in C++17
// mode but the standard library does not provide std::basic_string_view
// (as is the case, e.g., for Clang with the libstdc++ of GCC 6). We can
// simulate that only with the standard library of MSVC, whose C++17
// components can be disabled by defining _HAS_CXX17 to 0. Elsewhere,
// and in other modes, this is just a basic sanity test.

#if defined( _MSC_VER ) && defined( _MSVC_LANG ) && _MSVC_LANG == 201703L
#    define BOOST_DYNAMIC_BITSET_TEST_NO_STRING_VIEW
#    define _HAS_CXX17 0
#endif

#include "boost/config.hpp"
#include "boost/core/lightweight_test.hpp"
#include "boost/dynamic_bitset.hpp"
#include <string>

#if defined( BOOST_DYNAMIC_BITSET_TEST_NO_STRING_VIEW ) \
    && ! defined( BOOST_NO_CXX17_HDR_STRING_VIEW )
#    error "The simulation of a standard library without string_view failed"
#endif

int
main()
{
    const boost::dynamic_bitset<> b( std::string( "1101" ) );
    BOOST_TEST_EQ( b.size(), 4u );
    BOOST_TEST_EQ( b.to_ulong(), 13ul );

    return boost::report_errors();
}
