// -----------------------------------------------------------
//              Copyright (c) 2001 Jeremy Siek
//         Copyright (c) 2003-2006, 2025 Gennaro Prota
//             Copyright (c) 2014 Ahmed Charles
//          Copyright (c) 2014 Riccardo Marcangelo
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

#include "bitset_test.hpp"
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#include <assert.h>
#include <limits>

template< typename Block, typename AllocatorOrContainer = std::allocator< Block > >
void
run_test_cases()
{
    // a bunch of typedefs which will be handy later on
    typedef boost::dynamic_bitset< Block, AllocatorOrContainer > bitset_type;
    typedef bitset_test< bitset_type >     Tests;

    const std::string                      long_string = get_long_string();
    const std::size_t                      ul_width    = std::numeric_limits< unsigned long >::digits;

    //=====================================================================
    // Test b.empty()
    {
        bitset_type b;
        Tests::empty( b );
    }
    {
        bitset_type b( 1, 1ul );
        Tests::empty( b );
    }
    {
        bitset_type b( bitset_type::bits_per_block + bitset_type::bits_per_block / 2, 15ul );
        Tests::empty( b );
    }
    //=====================================================================
    // Test b.to_long()
    {
        bitset_type b;
        Tests::to_ulong( b );
    }
    {
        bitset_type b( std::string( "1" ) );
        Tests::to_ulong( b );
    }
    {
        bitset_type b( bitset_type::bits_per_block, static_cast< unsigned long >( -1 ) );
        Tests::to_ulong( b );
    }
    {
        std::string                    str( ul_width - 1, '1' );
        bitset_type                    b( str );
        Tests::to_ulong( b );
    }
    {
        std::string                    ul_str( ul_width, '1' );
        bitset_type                    b( ul_str );
        Tests::to_ulong( b );
    }
    { // case overflow
        bitset_type b( long_string );
        Tests::to_ulong( b );
    }
    //=====================================================================
    // Test b.to_number<T>()
    {
        bitset_type b;
        Tests::template to_number< unsigned char >( b );
        Tests::template to_number< unsigned short >( b );
        Tests::template to_number< unsigned int >( b );
        Tests::template to_number< unsigned long >( b );
        Tests::template to_number< unsigned long long >( b );
    }
    {
        bitset_type b( std::string( "1" ) );
        Tests::template to_number< unsigned char >( b );
        Tests::template to_number< unsigned short >( b );
        Tests::template to_number< unsigned int >( b );
        Tests::template to_number< unsigned long >( b );
        Tests::template to_number< unsigned long long >( b );
    }
    {
        bitset_type b( bitset_type::bits_per_block, static_cast< unsigned long >( -1 ) );
        Tests::template to_number< unsigned long >( b );
        Tests::template to_number< unsigned long long >( b );
    }
    {
        std::string str( ul_width - 1, '1' );
        bitset_type b( str );
        Tests::template to_number< unsigned long >( b );
        Tests::template to_number< unsigned long long >( b );
    }
    {
        std::string ul_str( ul_width, '1' );
        bitset_type b( ul_str );
        Tests::template to_number< unsigned long >( b );
        Tests::template to_number< unsigned long long >( b );
    }
    {
        bitset_type b( 8, 255ul );
        Tests::template to_number< unsigned char >( b );
    }
    {
        bitset_type b( 16, 256ul );
        Tests::template to_number< unsigned char >( b );
    }
    { // Overflow case
        bitset_type b( long_string );
        Tests::template to_number< unsigned char >( b );
        Tests::template to_number< unsigned short >( b );
        Tests::template to_number< unsigned long long >( b );
    }
    //=====================================================================
    // Test to_string(b, str)
    {
        bitset_type b;
        Tests::to_string( b );
    }
    {
        bitset_type b( std::string( "0" ) );
        Tests::to_string( b );
    }
    {
        bitset_type b( long_string );
        Tests::to_string( b );
    }
    //=====================================================================
    // Test b.count()
    {
        bitset_type b;
        Tests::count( b );
    }
    {
        bitset_type b( std::string( "0" ) );
        Tests::count( b );
    }
    {
        bitset_type b( std::string( "1" ) );
        Tests::count( b );
    }
    {
        bitset_type b( 8, 255ul );
        Tests::count( b );
    }
    {
        bitset_type b( long_string );
        Tests::count( b );
    }
    //=====================================================================
    // Test b.size()
    {
        bitset_type b;
        Tests::size( b );
    }
    {
        bitset_type b( std::string( "0" ) );
        Tests::size( b );
    }
    {
        bitset_type b( long_string );
        Tests::size( b );
    }
    //=====================================================================
    // Test b.capacity()
    {
        bitset_type b;
        Tests::capacity( b );
    }
    {
        bitset_type b( 100 );
        Tests::capacity( b );
    }
    //=====================================================================
    // Test b.reserve()
    {
        bitset_type b;
        Tests::reserve_test_one( b );
    }
    {
        bitset_type b( 100 );
        Tests::reserve_test_two( b );
    }
    //=====================================================================
    // Test b.shrink_to_fit()
    {
        bitset_type b;
        Tests::shrink_to_fit_test_one( b );
    }
    {
        bitset_type b( 100 );
        Tests::shrink_to_fit_test_two( b );
    }
    //=====================================================================
    // Test b.all()
    {
        bitset_type b;
        Tests::all( b );
        Tests::all( ~b );
        Tests::all( b.set() );
        Tests::all( b.reset() );
    }
    {
        bitset_type b( std::string( "0" ) );
        Tests::all( b );
        Tests::all( ~b );
        Tests::all( b.set() );
        Tests::all( b.reset() );
    }
    {
        bitset_type b( long_string );
        Tests::all( b );
        Tests::all( ~b );
        Tests::all( b.set() );
        Tests::all( b.reset() );
    }
    //=====================================================================
    // Test b.any()
    {
        bitset_type b;
        Tests::any( b );
        Tests::any( ~b );
        Tests::any( b.set() );
        Tests::any( b.reset() );
    }
    {
        bitset_type b( std::string( "0" ) );
        Tests::any( b );
        Tests::any( ~b );
        Tests::any( b.set() );
        Tests::any( b.reset() );
    }
    {
        bitset_type b( long_string );
        Tests::any( b );
        Tests::any( ~b );
        Tests::any( b.set() );
        Tests::any( b.reset() );
    }
    //=====================================================================
    // Test b.none()
    {
        bitset_type b;
        Tests::none( b );
        Tests::none( ~b );
        Tests::none( b.set() );
        Tests::none( b.reset() );
    }
    {
        bitset_type b( std::string( "0" ) );
        Tests::none( b );
        Tests::none( ~b );
        Tests::none( b.set() );
        Tests::none( b.reset() );
    }
    {
        bitset_type b( long_string );
        Tests::none( b );
        Tests::none( ~b );
        Tests::none( b.set() );
        Tests::none( b.reset() );
    }
    //=====================================================================
    // Test a.is_subset_of(b)
    {
        bitset_type a, b;
        Tests::subset( a, b );
    }
    {
        bitset_type a( std::string( "0" ) ), b( std::string( "0" ) );
        Tests::subset( a, b );
    }
    {
        bitset_type a( std::string( "1" ) ), b( std::string( "1" ) );
        Tests::subset( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        Tests::subset( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        a[ long_string.size() / 2 ].flip();
        Tests::subset( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        b[ long_string.size() / 2 ].flip();
        Tests::subset( a, b );
    }
    {
        // neither is a subset of the other
        bitset_type a( std::string( "01" ) ), b( std::string( "10" ) );
        Tests::subset( a, b );
        Tests::subset( b, a );
    }
    {
        bitset_type a( std::string( "01" ) ), b( std::string( "11" ) );
        Tests::subset( a, b );
        Tests::subset( b, a );
    }
    {
        const bitset_type a( long_string );
        const bitset_type b = ~a;
        Tests::subset( a, b );
        Tests::subset( b, a );
    }
    {
        // neither is a subset of the other, but they share most bits
        bitset_type       a( long_string ), b( long_string );
        const std::size_t first_off = a.find_first_off();
        a.set( first_off );
        b.set( b.find_next_off( first_off ) );
        Tests::subset( a, b );
        Tests::subset( b, a );
    }
    //=====================================================================
    // Test a.is_proper_subset_of(b)
    {
        bitset_type a, b;
        Tests::proper_subset( a, b );
    }
    {
        bitset_type a( std::string( "0" ) ), b( std::string( "0" ) );
        Tests::proper_subset( a, b );
    }
    {
        bitset_type a( std::string( "1" ) ), b( std::string( "1" ) );
        Tests::proper_subset( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        Tests::proper_subset( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        a[ long_string.size() / 2 ].flip();
        Tests::proper_subset( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        b[ long_string.size() / 2 ].flip();
        Tests::proper_subset( a, b );
    }
    {
        // neither is a subset of the other
        bitset_type a( std::string( "01" ) ), b( std::string( "10" ) );
        Tests::proper_subset( a, b );
        Tests::proper_subset( b, a );
    }
    {
        bitset_type a( std::string( "01" ) ), b( std::string( "11" ) );
        Tests::proper_subset( a, b );
        Tests::proper_subset( b, a );
    }
    {
        const bitset_type a( long_string );
        const bitset_type b = ~a;
        Tests::proper_subset( a, b );
        Tests::proper_subset( b, a );
    }
    {
        // neither is a subset of the other, but they share most bits
        bitset_type       a( long_string ), b( long_string );
        const std::size_t first_off = a.find_first_off();
        a.set( first_off );
        b.set( b.find_next_off( first_off ) );
        Tests::proper_subset( a, b );
        Tests::proper_subset( b, a );
    }
    //=====================================================================
    // Test intersects
    {
        bitset_type a; // empty
        bitset_type b;
        Tests::intersects( a, b );
    }
    {
        bitset_type a;
        bitset_type b( 5, 8ul );
        Tests::intersects( a, b );
    }
    {
        bitset_type a( 8, 0ul );
        bitset_type b( 15, 0ul );
        b[ 9 ] = 1;
        Tests::intersects( a, b );
    }
    {
        bitset_type a( 15, 0ul );
        bitset_type b( 22, 0ul );
        a[ 14 ] = b[ 14 ] = 1;
        Tests::intersects( a, b );
    }
    //=====================================================================
    // Test find_first
    {
        // empty bitset
        bitset_type b;
        Tests::find_first( b );
        Tests::find_first( b, 0, false );
    }
    {
        // bitset of size 1
        bitset_type b( 1, 1ul );
        Tests::find_first( b );
        Tests::find_first( b, 0, false );
    }
    {
        // all-0s bitset
        bitset_type b( 4 * bitset_type::bits_per_block, 0ul );
        Tests::find_first( b );
        Tests::find_first( b, 0, false );
    }
    {
        // first bit on or off
        bitset_type b( 1, 1ul );
        Tests::find_first( b );
        b.set( 0, false );
        Tests::find_first( b, 0, false );
    }
    {
        // last bit on or off
        bitset_type b( 4 * bitset_type::bits_per_block - 1, 0ul );
        b.set( b.size() - 1 );
        Tests::find_first( b );
        b.set( b.size() - 1, false );
        Tests::find_first( b, 0, false );
    }
    //=====================================================================
    // Test find_next, find_next_off, offset find_first and offset find_first_off
    {
        // empty bitset
        bitset_type b;

        // check
        Tests::find_pos( b, 0 );
        Tests::find_pos( b, 1 );
        Tests::find_pos( b, 200 );
        Tests::find_pos( b, b.npos );
        Tests::find_pos( b, 0, false );
        Tests::find_pos( b, 1, false );
        Tests::find_pos( b, 200, false );
        Tests::find_pos( b, b.npos, false );
    }
    {
        // bitset of size 1 (find_next can never find)
        bitset_type b( 1, 1ul );

        // check
        Tests::find_pos( b, 0 );
        Tests::find_pos( b, 1 );
        Tests::find_pos( b, 200 );
        Tests::find_pos( b, b.npos );
        Tests::find_pos( b, 0, false );
        Tests::find_pos( b, 1, false );
        Tests::find_pos( b, 200, false );
        Tests::find_pos( b, b.npos, false );
    }
    {
        // all-1s bitset
        bitset_type b( 16 * bitset_type::bits_per_block );
        b.set();

        // check
        const typename bitset_type::size_type larger_than_size = 5 + b.size();
        for ( typename bitset_type::size_type i = 0; i <= larger_than_size; ++i ) {
            Tests::find_pos( b, i );
            Tests::find_pos( b, i, false );
        }
        Tests::find_pos( b, b.npos );
        Tests::find_pos( b, b.npos, false );
    }
    {
        // a bitset with 1s at block boundary only
        const int                       num_blocks  = 32;
        const int                       block_width = bitset_type::bits_per_block;

        bitset_type                     b( num_blocks * block_width );
        typename bitset_type::size_type i = block_width - 1;
        for ( ; i < b.size(); i += block_width ) {
            b.set( i );
            typename bitset_type::size_type first_in_block = i - ( block_width - 1 );
            b.set( first_in_block );
        }

        // check
        const typename bitset_type::size_type larger_than_size = 5 + b.size();
        for ( i = 0; i <= larger_than_size; ++i ) {
            Tests::find_pos( b, i );
            Tests::find_pos( b, i, false );
        }
        Tests::find_pos( b, b.npos );
        Tests::find_pos( b, b.npos, false );
    }
    {
        // bitset with alternate 1s and 0s
        const typename bitset_type::size_type sz = 1000;
        bitset_type                           b( sz );

        typename bitset_type::size_type       i = 0;
        for ( ; i < sz; ++i ) {
            b[ i ] = ( i % 2 == 0 );
        }

        // check
        const typename bitset_type::size_type larger_than_size = 5 + b.size();
        for ( i = 0; i <= larger_than_size; ++i ) {
            Tests::find_pos( b, i );
            Tests::find_pos( b, i, false );
        }
        Tests::find_pos( b, b.npos );
        Tests::find_pos( b, b.npos, false );
    }
    {
        // all-0s (all-1s) bitsets with a single 1 (0), or none, searched
        // from a few positions on, or next to, a block boundary, so that
        // the search may have to skip whole blocks; the last block is
        // only partially used
        const typename bitset_type::size_type bpb      = bitset_type::bits_per_block;
        const typename bitset_type::size_type sz       = 4 * bpb - 1;
        const typename bitset_type::size_type starts[] = { 0, bpb - 1, bpb, bpb + 1 };
        for ( typename bitset_type::size_type i = 0; i <= sz; ++i ) {
            bitset_type b( sz );
            if ( i < sz ) {
                b.set( i );
            }
            for ( typename bitset_type::size_type pos : starts ) {
                Tests::find_pos( b, pos );
            }
            b.flip();
            for ( typename bitset_type::size_type pos : starts ) {
                Tests::find_pos( b, pos, false );
            }
        }
    }
    //=====================================================================
    // Test operator==
    {
        bitset_type a, b;
        Tests::operator_equal( a, b );
    }
    {
        bitset_type a( std::string( "0" ) ), b( std::string( "0" ) );
        Tests::operator_equal( a, b );
    }
    {
        bitset_type a( std::string( "1" ) ), b( std::string( "1" ) );
        Tests::operator_equal( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        Tests::operator_equal( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        a[ long_string.size() / 2 ].flip();
        Tests::operator_equal( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        b[ long_string.size() / 2 ].flip();
        Tests::operator_equal( a, b );
    }
    //=====================================================================
    // Test operator!=
    {
        bitset_type a, b;
        Tests::operator_not_equal( a, b );
    }
    {
        bitset_type a( std::string( "0" ) ), b( std::string( "0" ) );
        Tests::operator_not_equal( a, b );
    }
    {
        bitset_type a( std::string( "1" ) ), b( std::string( "1" ) );
        Tests::operator_not_equal( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        Tests::operator_not_equal( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        a[ long_string.size() / 2 ].flip();
        Tests::operator_not_equal( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        b[ long_string.size() / 2 ].flip();
        Tests::operator_not_equal( a, b );
    }
    //=====================================================================
    // Test operator<
    {
        bitset_type a, b;
        Tests::operator_less_than( a, b );
    }
    {
        bitset_type a;
        bitset_type b( std::string( "1" ) );
        Tests::operator_less_than( a, b );
    }
    {
        bitset_type a( std::string( "0" ) ), b( std::string( "0" ) );
        Tests::operator_less_than( a, b );
    }
    {
        bitset_type a( std::string( "1" ) ), b( std::string( "1" ) );
        Tests::operator_less_than( a, b );
    }
    {
        bitset_type a( std::string( "10" ) ), b( std::string( "11" ) );
        Tests::operator_less_than( a, b );
    }
    {
        bitset_type a( std::string( "101" ) ), b( std::string( "11" ) );
        Tests::operator_less_than( a, b );
    }
    {
        bitset_type a( std::string( "10" ) ), b( std::string( "111" ) );
        Tests::operator_less_than( a, b );
    }
    {
        bitset_type a( std::string( "11" ) ), b( std::string( "111" ) );
        Tests::operator_less_than( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        Tests::operator_less_than( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        a[ long_string.size() / 2 ].flip();
        Tests::operator_less_than( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        b[ long_string.size() / 2 ].flip();
        Tests::operator_less_than( a, b );
    }
    // check for consistency with ulong behaviour when the sizes are equal
    {
        bitset_type a( 3, 4ul ), b( 3, 5ul );
        BOOST_TEST( a < b );
    }
    {
        bitset_type a( 3, 4ul ), b( 3, 4ul );
        BOOST_TEST( ! ( a < b ) );
    }
    {
        bitset_type a( 3, 5ul ), b( 3, 4ul );
        BOOST_TEST( ! ( a < b ) );
    }
    // when the sizes are not equal lexicographic compare does not necessarily correspond to ulong behavior
    {
        bitset_type a( 4, 4ul ), b( 3, 5ul );
        BOOST_TEST( a < b );
    }
    {
        bitset_type a( 3, 4ul ), b( 4, 5ul );
        BOOST_TEST( ! ( a < b ) );
    }
    {
        bitset_type a( 4, 4ul ), b( 3, 4ul );
        BOOST_TEST( a < b );
    }
    {
        bitset_type a( 3, 4ul ), b( 4, 4ul );
        BOOST_TEST( ! ( a < b ) );
    }
    {
        bitset_type a( 4, 5ul ), b( 3, 4ul );
        BOOST_TEST( a < b );
    }
    {
        bitset_type a( 3, 5ul ), b( 4, 4ul );
        BOOST_TEST( ! ( a < b ) );
    }
    //=====================================================================
    // Test operator<=
    {
        bitset_type a, b;
        Tests::operator_less_than_eq( a, b );
    }
    {
        bitset_type a( std::string( "0" ) ), b( std::string( "0" ) );
        Tests::operator_less_than_eq( a, b );
    }
    {
        bitset_type a( std::string( "1" ) ), b( std::string( "1" ) );
        Tests::operator_less_than_eq( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        Tests::operator_less_than_eq( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        a[ long_string.size() / 2 ].flip();
        Tests::operator_less_than_eq( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        b[ long_string.size() / 2 ].flip();
        Tests::operator_less_than_eq( a, b );
    }
    // check for consistency with ulong behaviour
    {
        bitset_type a( 3, 4ul ), b( 3, 5ul );
        BOOST_TEST( a <= b );
    }
    {
        bitset_type a( 3, 4ul ), b( 3, 4ul );
        BOOST_TEST( a <= b );
    }
    {
        bitset_type a( 3, 5ul ), b( 3, 4ul );
        BOOST_TEST( ! ( a <= b ) );
    }
    //=====================================================================
    // Test operator>
    {
        bitset_type a, b;
        Tests::operator_greater_than( a, b );
    }
    {
        bitset_type a( std::string( "0" ) ), b( std::string( "0" ) );
        Tests::operator_greater_than( a, b );
    }
    {
        bitset_type a( std::string( "1" ) ), b( std::string( "1" ) );
        Tests::operator_greater_than( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        Tests::operator_greater_than( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        a[ long_string.size() / 2 ].flip();
        Tests::operator_greater_than( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        b[ long_string.size() / 2 ].flip();
        Tests::operator_greater_than( a, b );
    }
    // check for consistency with ulong behaviour
    {
        bitset_type a( 3, 4ul ), b( 3, 5ul );
        BOOST_TEST( ! ( a > b ) );
    }
    {
        bitset_type a( 3, 4ul ), b( 3, 4ul );
        BOOST_TEST( ! ( a > b ) );
    }
    {
        bitset_type a( 3, 5ul ), b( 3, 4ul );
        BOOST_TEST( a > b );
    }
    //=====================================================================
    // Test operator>=
    {
        bitset_type a, b;
        Tests::operator_greater_than_eq( a, b );
    }
    {
        bitset_type a( std::string( "0" ) ), b( std::string( "0" ) );
        Tests::operator_greater_than_eq( a, b );
    }
    {
        bitset_type a( std::string( "1" ) ), b( std::string( "1" ) );
        Tests::operator_greater_than_eq( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        Tests::operator_greater_than_eq( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        a[ long_string.size() / 2 ].flip();
        Tests::operator_greater_than_eq( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        b[ long_string.size() / 2 ].flip();
        Tests::operator_greater_than_eq( a, b );
    }
    // check for consistency with ulong behaviour
    {
        bitset_type a( 3, 4ul ), b( 3, 5ul );
        BOOST_TEST( ! ( a >= b ) );
    }
    {
        bitset_type a( 3, 4ul ), b( 3, 4ul );
        BOOST_TEST( a >= b );
    }
    {
        bitset_type a( 3, 5ul ), b( 3, 4ul );
        BOOST_TEST( a >= b );
    }
    //=====================================================================
    // Test b.test(pos)
    { // case pos >= b.size()
        bitset_type b;
        Tests::test_bit( b, 0 );
    }
    { // case pos < b.size()
        bitset_type b( std::string( "0" ) );
        Tests::test_bit( b, 0 );
    }
    { // case pos < b.size(), with the bit set
        bitset_type b( std::string( "1" ) );
        Tests::test_bit( b, 0 );
    }
    { // case pos == b.size() / 2
        bitset_type b( long_string );
        Tests::test_bit( b, long_string.size() / 2 );
    }
    { // case every pos < b.size()
        bitset_type b( long_string );
        for ( std::size_t pos = 0; pos < b.size(); ++pos )
            Tests::test_bit( b, pos );
    }
    //=====================================================================
    // Test b.test_set(pos)
    { // case pos >= b.size()
        bitset_type b;
        Tests::test_set_bit( b, 0, true );
        Tests::test_set_bit( b, 0, false );
    }
    { // case pos < b.size()
        bitset_type b( std::string( "0" ) );
        Tests::test_set_bit( b, 0, true );
        Tests::test_set_bit( b, 0, false );
    }
    { // case pos == b.size() / 2
        bitset_type b( long_string );
        Tests::test_set_bit( b, long_string.size() / 2, true );
        Tests::test_set_bit( b, long_string.size() / 2, false );
    }
    //=====================================================================
    // Test b << pos
    { // case pos == 0
        std::size_t                    pos = 0;
        bitset_type b( std::string( "1010" ) );
        Tests::operator_shift_left( b, pos );
    }
    { // case pos == size()/2
        std::size_t                    pos = long_string.size() / 2;
        bitset_type b( long_string );
        Tests::operator_shift_left( b, pos );
    }
    { // case pos >= n
        std::size_t                    pos = long_string.size();
        bitset_type b( long_string );
        Tests::operator_shift_left( b, pos );
    }
    //=====================================================================
    // Test b >> pos
    { // case pos == 0
        std::size_t                    pos = 0;
        bitset_type b( std::string( "1010" ) );
        Tests::operator_shift_right( b, pos );
    }
    { // case pos == size()/2
        std::size_t                    pos = long_string.size() / 2;
        bitset_type b( long_string );
        Tests::operator_shift_right( b, pos );
    }
    { // case pos >= n
        std::size_t                    pos = long_string.size();
        bitset_type b( long_string );
        Tests::operator_shift_right( b, pos );
    }
    //=====================================================================
    // Test b.extract( pos, len )
    {
        const bitset_type bitsets[] = { bitset_type(), bitset_type( std::string( "1" ) ), bitset_type( std::string( "0" ) ), bitset_type( long_string ) };
        const std::size_t bpb       = bitset_type::bits_per_block;
        for ( const bitset_type & b : bitsets ) {
            const std::size_t n           = b.size();
            const std::size_t positions[] = { 0, 1, bpb - 1, bpb, bpb + 1, n / 2, n - 1, n };
            const std::size_t lengths[]   = { 0, 1, bpb - 1, bpb + 1, n, bitset_type::npos };
            for ( const std::size_t pos : positions ) {
                for ( const std::size_t len : lengths ) {
                    Tests::extract( b, pos, len );
                }
            }
            BOOST_TEST( b.extract() == b );
        }
    }
    //=====================================================================
    // Test a & b
    {
        bitset_type lhs, rhs;
        Tests::operator_and( lhs, rhs );
    }
    {
        bitset_type lhs( std::string( "1" ) ), rhs( std::string( "0" ) );
        Tests::operator_and( lhs, rhs );
    }
    {
        bitset_type lhs( long_string.size(), 0 ), rhs( long_string );
        Tests::operator_and( lhs, rhs );
    }
    {
        bitset_type lhs( long_string.size(), 1 ), rhs( long_string );
        Tests::operator_and( lhs, rhs );
    }
    //=====================================================================
    // Test a | b
    {
        bitset_type lhs, rhs;
        Tests::operator_or( lhs, rhs );
    }
    {
        bitset_type lhs( std::string( "1" ) ), rhs( std::string( "0" ) );
        Tests::operator_or( lhs, rhs );
    }
    {
        bitset_type lhs( long_string.size(), 0 ), rhs( long_string );
        Tests::operator_or( lhs, rhs );
    }
    {
        bitset_type lhs( long_string.size(), 1 ), rhs( long_string );
        Tests::operator_or( lhs, rhs );
    }
    //=====================================================================
    // Test a^b
    {
        bitset_type lhs, rhs;
        Tests::operator_xor( lhs, rhs );
    }
    {
        bitset_type lhs( std::string( "1" ) ), rhs( std::string( "0" ) );
        Tests::operator_xor( lhs, rhs );
    }
    {
        bitset_type lhs( long_string.size(), 0 ), rhs( long_string );
        Tests::operator_xor( lhs, rhs );
    }
    {
        bitset_type lhs( long_string.size(), 1 ), rhs( long_string );
        Tests::operator_xor( lhs, rhs );
    }
    //=====================================================================
    // Test a-b
    {
        bitset_type lhs, rhs;
        Tests::operator_sub( lhs, rhs );
    }
    {
        bitset_type lhs( std::string( "1" ) ), rhs( std::string( "0" ) );
        Tests::operator_sub( lhs, rhs );
    }
    {
        bitset_type lhs( long_string.size(), 0 ), rhs( long_string );
        Tests::operator_sub( lhs, rhs );
    }
    {
        bitset_type lhs( long_string.size(), 1 ), rhs( long_string );
        Tests::operator_sub( lhs, rhs );
    }
    //=====================================================================
    // Test the odr-use of the static data members
    {
        // Storing the address of a constant in a volatile object odr-uses
        // it, even in an optimized build: before C++17, that requires a
        // definition of the constant at namespace scope, without which
        // this program doesn't link.
        const int * volatile bits_address                             = &bitset_type::bits_per_block;
        const typename bitset_type::size_type * volatile npos_address = &bitset_type::npos;
        const int * volatile iterator_bits_address                    = &bitset_type::iterator::bits_per_block;
        BOOST_TEST( *bits_address == bitset_type::bits_per_block );
        BOOST_TEST( *npos_address == bitset_type::npos );
        BOOST_TEST( *iterator_bits_address == bitset_type::bits_per_block );
    }
}

int
main()
{
    run_test_cases< unsigned char >();
    run_test_cases< unsigned char, small_vector< unsigned char > >();
    run_test_cases< unsigned short >();
    run_test_cases< unsigned short, small_vector< unsigned short > >();
    run_test_cases< unsigned int >();
    run_test_cases< unsigned int, small_vector< unsigned int > >();
    run_test_cases< unsigned long >();
    run_test_cases< unsigned long, small_vector< unsigned long > >();
    run_test_cases< unsigned long long >();
    run_test_cases< unsigned long long, small_vector< unsigned long long > >();

    return boost::report_errors();
}
