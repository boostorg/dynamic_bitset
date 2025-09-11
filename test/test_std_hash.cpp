//
// Copyright (C) 2019 James E. King III
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "boost/config.hpp"
#include "boost/detail/lightweight_test.hpp"
#include "boost/dynamic_bitset.hpp"
#include <unordered_set>

int
main( int, char *[] )
{
    typedef boost::dynamic_bitset< unsigned long > bitset_type;
    const std::string                              long_string =
        "01001110101110110101011010000000000011110101101111111111";

    const bitset_type                 zeroes( long_string.size(), 0 );
    const bitset_type                 stuff( long_string );
    const bitset_type                 one( long_string.size(), 1 );

    std::unordered_set< bitset_type > bitsets;
    bitsets.insert( zeroes );
    bitsets.insert( stuff );
    bitsets.insert( one );

    BOOST_TEST_EQ( bitsets.size(), 3 );

    return boost::report_errors();
}
