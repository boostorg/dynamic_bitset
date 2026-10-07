//
// Copyright 2026 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

// Tests the constructor from a range of blocks with arguments that it
// takes as a size and a value.
//
// If BOOST_DYNAMIC_BITSET_TEST_REJECTED_CASE is defined, this file also
// contains one of the constructs that must not compile (see the
// Jamfile).

#include "boost/core/lightweight_test.hpp"
#include "boost/dynamic_bitset.hpp"

typedef boost::dynamic_bitset<> default_bitset;

enum unscoped_enum
{
    width = 16,
    init  = 7
};

enum sized_enum : unsigned char
{
    sized_width = 70,
    sized_init  = 0xA5
};

enum class scoped_enum
{
    a,
    b
};

template< typename Block >
void
test_enumerators_are_taken_as_a_size_and_a_value()
{
    typedef boost::dynamic_bitset< Block > bitset_type;

    const bitset_type a( width, init );
    BOOST_TEST_EQ( a.size(), 16u );
    BOOST_TEST_EQ( a.to_ulong(), 7ul );

    const bitset_type b( sized_width, sized_init, typename bitset_type::allocator_type() );
    BOOST_TEST_EQ( b.size(), 70u );
    BOOST_TEST( b == bitset_type( 70, 0xA5ul ) );
}

template< typename Block >
void
test_block_type()
{
    test_enumerators_are_taken_as_a_size_and_a_value< Block >();
}

#if defined( BOOST_DYNAMIC_BITSET_TEST_REJECTED_CASE )

void
rejected_case()
{
#    if BOOST_DYNAMIC_BITSET_TEST_REJECTED_CASE == 1
    // Floating-point values are neither iterators nor integers.
    const default_bitset b( 3.0, 5.0 );
#    elif BOOST_DYNAMIC_BITSET_TEST_REJECTED_CASE == 2
    // Scoped enumerators don't convert implicitly to integers.
    const default_bitset b( scoped_enum::a, scoped_enum::b );
#    endif
}

#endif

int
main()
{
    test_block_type< unsigned char >();
    test_block_type< unsigned short >();
    test_block_type< unsigned int >();
    test_block_type< unsigned long >();
    test_block_type< unsigned long long >();

    return boost::report_errors();
}
