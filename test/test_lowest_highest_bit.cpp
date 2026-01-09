//
// Copyright (C) 2018 James E. King III
// Copyright 2026 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "boost/core/lightweight_test.hpp"
#include "boost/cstdint.hpp"
#include "boost/dynamic_bitset/detail/lowest_highest_bit.hpp"

int
main( int, char *[] )
{
    for ( boost::int32_t i = 1; i < 32; ++i ) {
        BOOST_TEST_EQ( i, boost::detail::lowest_bit( 1u << i ) );
        BOOST_TEST_EQ( i, boost::detail::highest_bit( 1u << i ) );
    }

    BOOST_TEST_EQ( 2, boost::detail::lowest_bit( 123456788 ) );
    BOOST_TEST_EQ( 30, boost::detail::lowest_bit( static_cast< boost::int64_t >( 1507208177123328 ) ) );
    BOOST_TEST_EQ( 15, boost::detail::highest_bit( 0b1000100101111000));
    BOOST_TEST_EQ( 53, boost::detail::highest_bit( static_cast< boost::int64_t >( 0x20000000000000 ) ) );

    return boost::report_errors();
}
