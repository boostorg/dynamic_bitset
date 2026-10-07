//
// Copyright 2026 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

// Tests the constructor from a range of blocks with arguments that it
// takes as a size and a value, and the ranges of blocks whose
// value_type isn't Block.
//
// If BOOST_DYNAMIC_BITSET_TEST_REJECTED_CASE is defined, this file also
// contains one of the constructs that must not compile (see the
// Jamfile).

#include "boost/core/lightweight_test.hpp"
#include "boost/dynamic_bitset.hpp"
#include <climits>
#include <cstddef>
#include <iterator>
#include <list>
#include <vector>

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

// A class type which converts implicitly to any integer type (like,
// e.g., the arithmetic types of Boost.Endian).
struct wrapped_value
{
    unsigned long long value;

    operator unsigned long long() const
    {
        return value;
    }
};

// A minimal input iterator, to exercise the code paths for input
// iterators.
template< typename T >
class test_input_iterator
{
public:
    typedef std::input_iterator_tag iterator_category;
    typedef T                       value_type;
    typedef std::ptrdiff_t          difference_type;
    typedef const T *               pointer;
    typedef const T &               reference;

    explicit test_input_iterator( const T * p )
        : m_p( p )
    {
    }

    reference
    operator*() const
    {
        return *m_p;
    }

    test_input_iterator &
    operator++()
    {
        ++m_p;
        return *this;
    }

    test_input_iterator
    operator++( int )
    {
        const test_input_iterator old( *this );
        ++m_p;
        return old;
    }

    friend bool
    operator==( const test_input_iterator & a, const test_input_iterator & b )
    {
        return a.m_p == b.m_p;
    }

    friend bool
    operator!=( const test_input_iterator & a, const test_input_iterator & b )
    {
        return ! ( a == b );
    }

private:
    const T * m_p;
};

// Appends [first, last) to bitsets of all the sizes up to twice
// bits_per_block, and compares the results with those of appending the
// corresponding blocks one at a time.
template< typename Bitset, typename Iterator >
void
test_append( Iterator first, Iterator last, const std::vector< typename Bitset::block_type > & blocks )
{
    const std::size_t max_size = 2 * Bitset::bits_per_block;
    for ( std::size_t size = 0; size <= max_size; ++size ) {
        Bitset b( size );
        b.set();
        Bitset expected( b );
        for ( std::size_t i = 0; i < blocks.size(); ++i ) {
            expected.append( blocks[ i ] );
        }
        b.append( first, last );
        BOOST_TEST( b == expected );
    }
}

// Checks that the constructor from a range of blocks, from_block_range()
// and append( first, last ) treat the (non-empty) range [first, last) as
// the range of blocks `blocks`.
template< typename Bitset, typename Iterator >
void
test_range( Iterator first, Iterator last, const std::vector< typename Bitset::block_type > & blocks )
{
    const Bitset expected( blocks.begin(), blocks.end() );
    BOOST_TEST( Bitset( first, last ) == expected );

    Bitset whole( expected.size() );
    boost::from_block_range( first, last, whole );
    BOOST_TEST( whole == expected );

    Bitset part( expected.size() - 1 );
    boost::from_block_range( first, last, part );
    Bitset expected_part( expected );
    expected_part.resize( expected.size() - 1 );
    BOOST_TEST( part == expected_part );

    test_append< Bitset >( first, last, blocks );
}

template< typename Block, typename T >
void
test_range_of( const std::vector< T > & values )
{
    typedef boost::dynamic_bitset< Block > bitset_type;

    std::vector< Block > blocks;
    for ( std::size_t i = 0; i < values.size(); ++i ) {
        blocks.push_back( static_cast< Block >( values[ i ] ) );
    }

    const std::list< T >           list( values.begin(), values.end() );
    const test_input_iterator< T > input_first( values.data() );
    const test_input_iterator< T > input_last( values.data() + values.size() );

    test_range< bitset_type >( values.begin(), values.end(), blocks );
    test_range< bitset_type >( list.begin(), list.end(), blocks );
    test_range< bitset_type >( input_first, input_last, blocks );
}

template< typename Block >
void
test_elements_are_converted_to_block()
{
    const unsigned char      uc[]  = { 0xFF, 0x00, 0x81 };
    const signed char        sc[]  = { -1, 0, -128, 127 };
    const int                i[]   = { -1, 0, 1, -2, INT_MIN };
    const unsigned long long ull[] = { ~0ull, 0, 0x0123456789ABCDEFull };
    const unscoped_enum      e[]   = { width, init };
    const wrapped_value      w[]   = { { ~0ull }, { 5 } };

    test_range_of< Block >( std::vector< unsigned char >( std::begin( uc ), std::end( uc ) ) );
    test_range_of< Block >( std::vector< signed char >( std::begin( sc ), std::end( sc ) ) );
    test_range_of< Block >( std::vector< int >( std::begin( i ), std::end( i ) ) );
    test_range_of< Block >( std::vector< unsigned long long >( std::begin( ull ), std::end( ull ) ) );
    test_range_of< Block >( std::vector< unscoped_enum >( std::begin( e ), std::end( e ) ) );
    test_range_of< Block >( std::vector< wrapped_value >( std::begin( w ), std::end( w ) ) );
    test_range_of< Block >( std::vector< int >( 1, -1 ) );
}

template< typename Block >
void
test_block_type()
{
    test_enumerators_are_taken_as_a_size_and_a_value< Block >();
    test_elements_are_converted_to_block< Block >();
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
#    elif BOOST_DYNAMIC_BITSET_TEST_REJECTED_CASE == 3
    // The elements of a range of blocks must convert implicitly to
    // Block, and scoped enumerators don't.
    const std::vector< scoped_enum > v( 1, scoped_enum::a );
    default_bitset                   b;
    b.append( v.begin(), v.end() );
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
