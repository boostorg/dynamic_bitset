// -----------------------------------------------------------
//              Copyright 2026 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

// Checks that the members and the free functions of dynamic_bitset
// which are declared constexpr can be used in constant expressions,
// with each Block type. That's possible only in C++20 and later, and
// only if std::vector is usable in constant expressions; otherwise,
// this test is empty. It's also empty with libstdc++ in debug mode,
// because the checks which that mode adds to the standard algorithms
// (e.g., std::fill()) aren't usable in constant expressions, and with
// Clang and libstdc++ 12 or 13, some of whose std::vector and
// std::basic_string operations Clang can't evaluate in constant
// expressions.
//
// Each function called in constexpr_tests returns whether all of its
// checks pass. Most of the bitsets, and of the blocks and numbers which
// they're compared with, are described by the positions of their set
// bits.

#include "boost/config.hpp"
#include "boost/dynamic_bitset.hpp"
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <string>
#include <utility>

#if BOOST_CXX_VERSION >= 202002L && defined( __cpp_lib_constexpr_vector ) && __cpp_lib_constexpr_vector >= 201907L \
    && ! defined( _GLIBCXX_DEBUG ) && ! ( defined( __clang__ ) && defined( _GLIBCXX_RELEASE ) && _GLIBCXX_RELEASE < 14 )

#    include <string_view>

namespace {

// The positions of the set bits of a bitset, or of a number.
typedef std::initializer_list< std::size_t > bit_positions;

// Returns a bitset of num_bits bits, whose set bits are those at the
// given positions.
template< typename Bitset >
constexpr Bitset
make_bitset( std::size_t num_bits, bit_positions positions )
{
    Bitset b( num_bits );
    for ( const std::size_t pos : positions ) {
        b.set( pos );
    }
    return b;
}

// Returns the value of type T whose set bits are those at the given
// positions.
template< typename T >
constexpr T
make_value( bit_positions positions )
{
    T value = 0;
    for ( const std::size_t pos : positions ) {
        value |= static_cast< T >( T( 1 ) << pos );
    }
    return value;
}

// Returns the number of blocks which hold num_bits bits.
template< typename Bitset >
constexpr std::size_t
blocks_for( std::size_t num_bits )
{
    return ( num_bits + Bitset::bits_per_block - 1 ) / Bitset::bits_per_block;
}

// size is the size of most of the bitsets below, and last_bit is the
// position of their last bit: with any Block type, those bitsets span
// more than one block and have unused bits in the last one. Some
// bitsets have large_size bits, which span at least four blocks.
constexpr std::size_t size       = 70;
constexpr std::size_t last_bit   = size - 1;
constexpr std::size_t large_size = 200;

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

    constexpr explicit test_input_iterator( const T * p )
        : m_p( p )
    {
    }

    constexpr reference
    operator*() const
    {
        return *m_p;
    }

    constexpr test_input_iterator &
    operator++()
    {
        ++m_p;
        return *this;
    }

    constexpr test_input_iterator
    operator++( int )
    {
        const test_input_iterator old( *this );
        ++m_p;
        return old;
    }

    friend constexpr bool
    operator==( const test_input_iterator & a, const test_input_iterator & b )
    {
        return a.m_p == b.m_p;
    }

    friend constexpr bool
    operator!=( const test_input_iterator & a, const test_input_iterator & b )
    {
        return ! ( a == b );
    }

private:
    const T * m_p;
};

template< typename Bitset >
constexpr bool
constructs_from_sizes_and_values()
{
    const std::size_t               ullong_width = std::numeric_limits< unsigned long long >::digits;
    const bit_positions             value_bits   = { 0, 2, ullong_width - 1 };
    typename Bitset::allocator_type alloc;
    const Bitset                    a;
    const Bitset                    b( alloc );
    const Bitset                    c( size );
    const Bitset                    d( size, alloc );
    const Bitset                    e( size, make_value< unsigned long long >( value_bits ), alloc );
    // Two ints select the constructor from a range, which takes them as
    // a size and a value. Ten bits span two blocks with unsigned char.
    const Bitset                    f( 10, 0b10'0000'0101 );

    return a.empty() && b.empty() && c.size() == size && c.none() && d.size() == size && d.none()
        && e == make_bitset< Bitset >( size, value_bits ) && f == make_bitset< Bitset >( 10, { 0, 2, 9 } );
}

template< typename Bitset >
constexpr bool
constructs_from_ranges_of_blocks()
{
    typedef typename Bitset::block_type Block;

    // The bit p of blocks[ i ] goes to the position i * n + p.
    const std::size_t                   n        = Bitset::bits_per_block;
    const Block                         blocks[] = { make_value< Block >( { 0, n - 1 } ), make_value< Block >( { 1 } ) };
    const Bitset                        expected = make_bitset< Bitset >( 2 * n, { 0, n - 1, n + 1 } );
    const Bitset                        from_pointers( std::begin( blocks ), std::end( blocks ) );
    const Bitset                        from_input_iterators( test_input_iterator< Block >( std::begin( blocks ) ), test_input_iterator< Block >( std::end( blocks ) ) );

    return from_pointers == expected && from_input_iterators == expected;
}

template< typename Bitset >
constexpr bool
constructs_from_pointers_and_string_views()
{
    const char   digits[] = { '1', '0', '1', '1', 'x' };
    const Bitset a( digits, std::size( digits ) - 1 ); // all but the 'x'
    const Bitset b( "0110", 3, size );                 // only "011"
    const Bitset c( std::string_view( "10110" ) );

    return a == Bitset( 4, 0b1011ull ) && b == Bitset( size, 0b011ull ) && c == Bitset( 5, 0b10110ull );
}

template< typename Bitset >
constexpr bool
copies_and_moves()
{
    typename Bitset::allocator_type alloc;
    const Bitset                    original = make_bitset< Bitset >( size, { 0, 2, last_bit } );
    Bitset                          a( original );
    Bitset                          b( original, alloc );
    const Bitset                    c( std::move( a ) );
    const Bitset                    d( std::move( b ), alloc );
    Bitset                          e;
    Bitset                          f;
    e = original;
    f = std::move( e );

    return a.empty() && b.empty() && c == original && d == original && e.empty() && f == original;
}

template< typename Bitset >
constexpr bool
swaps()
{
    const Bitset x = make_bitset< Bitset >( size, { 0, 2, last_bit } );
    const Bitset y = make_bitset< Bitset >( 3, { 0, 2 } );
    Bitset       a( x );
    Bitset       b( y );
    a.swap( b );
    const bool swapped_by_member = a == y && b == x;
    boost::swap( a, b );

    return swapped_by_member && a == x && b == y;
}

template< typename Bitset >
constexpr bool
manages_capacity()
{
    Bitset b( size );
    b.reserve( large_size );
    const bool reserved = b.capacity() >= large_size;
    b.shrink_to_fit();

    return reserved && b.capacity() >= size && b.capacity() < large_size && b.size() == size
        && b.num_blocks() == blocks_for< Bitset >( size ) && b.max_size() >= large_size
        && b.get_allocator() == typename Bitset::allocator_type();
}

template< typename Bitset >
constexpr bool
pushes_and_pops()
{
    const Bitset bits = make_bitset< Bitset >( size, { 0, 2, last_bit } );
    Bitset       b;
    for ( std::size_t i = 0; i < size; ++i ) {
        b.push_back( bits[ i ] );
    }
    const bool pushed_back = b == bits;
    b.push_front( true ); // inserts at position 0, moving the other bits up
    const bool pushed_front = b == make_bitset< Bitset >( size + 1, { 0, 1, 3, last_bit + 1 } );
    b.pop_front();
    b.pop_back();

    return pushed_back && pushed_front && b == make_bitset< Bitset >( size - 1, { 0, 2 } );
}

template< typename Bitset >
constexpr bool
resizes_and_clears()
{
    const std::size_t n = Bitset::bits_per_block;
    Bitset            b = make_bitset< Bitset >( size, { 0, 2 } );
    b.resize( large_size, true ); // keeps bits 0 and 2, and sets the new ones
    const bool enlarged = b.size() == large_size && b.count() == 2 + ( large_size - size )
                       && b.find_next( 2 ) == size && b.find_first_off( size ) == Bitset::npos;
    b.resize( n + 1 ); // one bit in the last block, which pop_back() removes
    const bool shrunk = b == make_bitset< Bitset >( n + 1, { 0, 2 } ) && b.num_blocks() == 2;
    b.pop_back();
    const bool popped = b.num_blocks() == 1;
    b.clear();

    return enlarged && shrunk && popped && b.empty() && b.num_blocks() == 0;
}

// Whether appending the blocks to initial, one at a time, from pointers
// and from input iterators, gives expected.
template< typename Bitset, std::size_t N >
constexpr bool
appends_to( const Bitset & initial, const typename Bitset::block_type ( &blocks )[ N ], const Bitset & expected )
{
    typedef typename Bitset::block_type Block;

    Bitset                              one_at_a_time( initial );
    for ( const Block block : blocks ) {
        one_at_a_time.append( block );
    }
    Bitset from_pointers( initial );
    from_pointers.append( std::begin( blocks ), std::end( blocks ) );
    Bitset from_input_iterators( initial );
    from_input_iterators.append( test_input_iterator< Block >( std::begin( blocks ) ), test_input_iterator< Block >( std::end( blocks ) ) );

    return one_at_a_time == expected && from_pointers == expected && from_input_iterators == expected;
}

template< typename Bitset >
constexpr bool
appends()
{
    typedef typename Bitset::block_type Block;

    // The bit p of blocks[ i ] goes to the position m + i * n + p.
    const std::size_t                   n        = Bitset::bits_per_block;
    const Block                         blocks[] = { make_value< Block >( { 0, n - 1 } ), make_value< Block >( { 1 } ) };
    const Bitset                        initial  = make_bitset< Bitset >( 3, { 0, 2 } ); // its last block is partially filled
    const std::size_t                   m        = initial.size();
    const Bitset                        expected = make_bitset< Bitset >( m + 2 * n, { 0, 2, m, m + n - 1, m + n + 1 } );

    return appends_to( initial, blocks, expected )
        && appends_to( Bitset(), blocks, Bitset( std::begin( blocks ), std::end( blocks ) ) );
}

template< typename Bitset >
constexpr bool
sets_resets_and_flips_single_bits()
{
    Bitset b( size );
    b.set( 0 ).set( 1 ).set( last_bit, true ).set( 1, false ).flip( last_bit - 1 ).flip( 0 );
    const bool changed = b == make_bitset< Bitset >( size, { last_bit - 1, last_bit } );
    b.reset( last_bit );
    const bool one_reset = b == make_bitset< Bitset >( size, { last_bit - 1 } );
    b.set();
    const bool all_set = b.all() && b.count() == size;
    b.flip();
    const bool flipped = b.none();
    b.set( 0 ).reset();

    return changed && one_reset && all_set && flipped && b.none() && ( ~b ).all();
}

template< typename Bitset >
constexpr bool
sets_resets_and_flips_ranges()
{
    // [pos, pos + len) covers the second half of a block, the whole next
    // block and the first half of the one after it.
    const std::size_t n   = Bitset::bits_per_block;
    const std::size_t pos = n / 2;
    const std::size_t len = 2 * n;
    Bitset            b( large_size );
    b.set( pos, len, true );
    const bool were_set = b.count() == len && b.find_first() == pos && b.find_first_off( pos ) == pos + len;
    b.reset( pos + 1, len - 2 ); // all but the ends of the range
    const bool were_reset = b == make_bitset< Bitset >( large_size, { pos, pos + len - 1 } );
    b.flip( 0, 2 ).flip( 0, large_size );
    const bool were_flipped = b == ~make_bitset< Bitset >( large_size, { 0, 1, pos, pos + len - 1 } );
    b.set( 0, large_size, true );
    const bool all_set = b.all(); // with unsigned char, b has no unused bits
    b.set( n - 1, 2, false );     // the last bit of a block and the first bit of the next one

    return were_set && were_reset && were_flipped && all_set && b == ~make_bitset< Bitset >( large_size, { n - 1, n } );
}

template< typename Bitset >
constexpr bool
tests_bits()
{
    Bitset         b         = make_bitset< Bitset >( size, { 0, 2 } );
    const Bitset & cb        = b;
    const bool     tested    = b.test( 0 ) && ! b.test( 1 ) && cb[ 2 ] && b.at( 2 ) && ! cb.at( last_bit );
    const bool     was_set_1 = b.test_set( last_bit );
    const bool     was_set_2 = b.test_set( 0, false );

    return tested && ! was_set_1 && was_set_2 && b == make_bitset< Bitset >( size, { 2, last_bit } )
        && b.any() && ! b.none() && ! b.all();
}

template< typename Bitset >
constexpr bool
uses_the_reference_proxy()
{
    typedef typename Bitset::reference reference;

    Bitset                             b( size );
    reference                          r = b[ last_bit ];
    r                                    = true;
    const bool assigned                  = b.test( last_bit ) && r && ! ~r;
    r.flip();
    const bool      flipped = ! b.test( last_bit );
    const reference other( r );
    other = true; // the const operator=( bool ), through a copy of r

    return assigned && flipped && b == make_bitset< Bitset >( size, { last_bit } );
}

template< typename Bitset >
constexpr bool
assigns_through_the_reference_proxy()
{
    Bitset b = make_bitset< Bitset >( size, { last_bit } );
    b[ 3 ]   = b[ last_bit ]; // sets bit 3
    b[ 4 ] |= true;           // sets bit 4
    b[ 5 ] ^= true;           // sets bit 5
    b[ 5 ] &= false;          // resets it
    b[ 6 ] ^= true;           // sets bit 6
    b[ 6 ] -= true;           // resets it
    b[ 7 ] ^= true;           // sets bit 7
    swap( b[ 7 ], b[ 8 ] );   // moves the set bit from 7 to 8

    return b == make_bitset< Bitset >( size, { 3, 4, 8, last_bit } );
}

// Two bitsets whose bits take each combination of values at the
// positions 0 to 3, and at the last four positions.
template< typename Bitset >
constexpr std::pair< Bitset, Bitset >
make_operands()
{
    const Bitset a = make_bitset< Bitset >( size, { 1, 3, last_bit - 2, last_bit } );
    const Bitset b = make_bitset< Bitset >( size, { 2, 3, last_bit - 1, last_bit } );
    return std::pair< Bitset, Bitset >( a, b );
}

template< typename Bitset >
constexpr bool
does_bitwise_operations()
{
    const std::pair< Bitset, Bitset > operands = make_operands< Bitset >();
    const Bitset &                    a        = operands.first;
    const Bitset &                    b        = operands.second;

    return ( a & b ) == make_bitset< Bitset >( size, { 3, last_bit } )
        && ( a | b ) == make_bitset< Bitset >( size, { 1, 2, 3, last_bit - 2, last_bit - 1, last_bit } )
        && ( a ^ b ) == make_bitset< Bitset >( size, { 1, 2, last_bit - 2, last_bit - 1 } )
        && ( a - b ) == make_bitset< Bitset >( size, { 1, last_bit - 2 } ) && ( a ^ ~a ).all();
}

template< typename Bitset >
constexpr bool
does_compound_bitwise_assignments()
{
    const std::pair< Bitset, Bitset > operands = make_operands< Bitset >();
    const Bitset &                    a        = operands.first;
    const Bitset &                    b        = operands.second;
    Bitset                            r1( a );
    Bitset                            r2( a );
    Bitset                            r3( a );
    Bitset                            r4( a );

    return ( r1 &= b ) == ( a & b ) && ( r2 |= b ) == ( a | b ) && ( r3 ^= b ) == ( a ^ b )
        && ( r4 -= b ) == ( a - b );
}

// Whether shifting a by amount, in each direction and with each form of
// the operator, moves each bit by amount positions and shifts in zeros.
template< typename Bitset >
constexpr bool
shifts_by( const Bitset & a, std::size_t amount )
{
    Bitset left( a );
    Bitset right( a );
    left <<= amount;
    right >>= amount;
    for ( std::size_t i = 0; i < a.size(); ++i ) {
        if ( left[ i ] != ( i >= amount && a[ i - amount ] )
             || right[ i ] != ( i + amount < a.size() && a[ i + amount ] ) ) {
            return false;
        }
    }
    return left.size() == a.size() && right.size() == a.size() && ( a << amount ) == left
        && ( a >> amount ) == right;
}

template< typename Bitset >
constexpr bool
shifts()
{
    // a has bits on both sides of the first boundary between blocks, and
    // in the last block. whole_blocks is the largest multiple of n less
    // than size, so the shifts by whole_blocks and by last_bit move whole
    // blocks, without and with a remainder.
    const std::size_t n            = Bitset::bits_per_block;
    const std::size_t whole_blocks = last_bit / n * n;
    const Bitset      a            = make_bitset< Bitset >( size, { 0, 2, n - 1, n, last_bit } );

    return shifts_by( a, 0 ) && shifts_by( a, 1 ) && shifts_by( a, n ) && shifts_by( a, n + 1 )
        && shifts_by( a, whole_blocks ) && shifts_by( a, last_bit ) && shifts_by( a, size )
        && shifts_by( a, 2 * size );
}

template< typename Bitset >
constexpr bool
extracts()
{
    // The bits from pos on span more than one block.
    const std::size_t pos = 2;
    const Bitset      b   = make_bitset< Bitset >( size, { 0, pos, last_bit } );

    return b.extract( pos ) == make_bitset< Bitset >( size - pos, { 0, last_bit - pos } )
        && b.extract( pos, 1 ) == make_bitset< Bitset >( 1, { 0 } ) && b.extract( size ).empty();
}

template< typename Bitset >
constexpr bool
compares()
{
    // Bitsets compare lexicographically, from their highest bit.
    const Bitset a = make_bitset< Bitset >( size, { 0, 2 } );
    const Bitset b = make_bitset< Bitset >( size, { 1, last_bit } );
    const Bitset c = make_bitset< Bitset >( 3, { 0, 2 } );
    Bitset       longer_a( a );
    Bitset       longer_c( c );
    longer_a.push_front( true );  // a is a prefix of longer_a
    longer_c.push_front( false ); // c is a prefix of longer_c

    return a == Bitset( a ) && a != b && a < b && a <= b && b > a && b >= a && a <= a && a >= a
        && ! ( b < a ) && a < c && c > a && a < longer_a && ! ( longer_a < a ) && c < longer_c;
}

template< typename Bitset >
constexpr bool
checks_set_relations()
{
    const Bitset a        = make_bitset< Bitset >( size, { 0, 2, last_bit } );
    const Bitset b        = make_bitset< Bitset >( size, { 0, 1, 2, last_bit } );
    const Bitset disjoint = make_bitset< Bitset >( size, { 1 } );

    return a.is_subset_of( b ) && a.is_proper_subset_of( b ) && ! b.is_subset_of( a ) && a.is_subset_of( a )
        && ! a.is_proper_subset_of( a ) && a.intersects( b ) && ! a.intersects( disjoint );
}

template< typename Bitset >
constexpr bool
finds_set_bits()
{
    // far is in the third block, between two empty blocks.
    const std::size_t npos = Bitset::npos;
    const std::size_t far  = 2 * Bitset::bits_per_block + 1;
    const Bitset      b    = make_bitset< Bitset >( large_size, { 1, 3, far } );

    return Bitset( large_size ).find_first() == npos && b.find_first() == 1 && b.find_first( 2 ) == 3
        && b.find_first( 4 ) == far && b.find_first( far + 1 ) == npos && b.find_next( 1 ) == 3
        && b.find_next( 3 ) == far && b.find_next( far ) == npos && b.find_next( npos ) == npos;
}

template< typename Bitset >
constexpr bool
finds_unset_bits()
{
    // far is in the third block, between two blocks whose bits are all
    // set.
    const std::size_t npos = Bitset::npos;
    const std::size_t far  = 2 * Bitset::bits_per_block + 1;
    Bitset            full( size ); // the unused bits of the last block are zero
    Bitset            b( large_size );
    full.set();
    b.set().reset( 1 ).reset( far );

    return full.find_first_off() == npos && b.find_first_off() == 1 && b.find_first_off( 2 ) == far
        && b.find_next_off( 1 ) == far && b.find_next_off( far ) == npos && b.find_first_off( large_size ) == npos;
}

// Checks the operations of Iterator, over a bitset equal to
// make_bitset< Bitset >( size, { 0, 2, last_bit } ).
template< typename Iterator >
constexpr bool
has_iterator_operations( Iterator first, Iterator last )
{
    Iterator   it          = first;
    const bool incremented = *it++ && ! *it && *++it; // it is at bit 2
    it += last_bit - 2;
    const bool advanced = *it && it - first == last_bit && first[ 2 ] && *( 2 + first ) && *( last - 1 );
    it -= last_bit - 2;
    const bool decremented = *it-- && ! *it && *--it; // it is at bit 0

    return incremented && advanced && decremented && it == first && it != last && first < last
        && first <= it && last > it && last >= first && last - first == size && Iterator() == Iterator();
}

template< typename Bitset >
constexpr bool
iterates()
{
    Bitset                                b     = make_bitset< Bitset >( size, { 0, 2 } );
    const Bitset &                        cb    = b;
    const typename Bitset::iterator       first = b.begin();
    const typename Bitset::const_iterator cfirst( first );
    first[ last_bit ] = true;

    return has_iterator_operations( b.begin(), b.end() ) && has_iterator_operations( cb.begin(), cb.end() )
        && has_iterator_operations( b.cbegin(), b.cend() ) && cfirst == first && first == cfirst
        && b.end() - cfirst == size && cfirst < b.end();
}

template< typename Bitset >
constexpr bool
iterates_in_reverse()
{
    Bitset         b  = make_bitset< Bitset >( size, { 0, 2 } );
    const Bitset & cb = b;
    *b.rbegin()       = true; // sets last_bit

    return *cb.rbegin() && ! *( cb.rbegin() + 1 ) && *( b.rend() - 1 ) && *b.crbegin() && *( b.crend() - 3 ) // bit 2
        && b.rend() - b.rbegin() == size && cb.rend() - cb.rbegin() == size && b.crend() - b.crbegin() == size;
}

template< typename Bitset >
constexpr bool
converts_to_numbers()
{
    const std::size_t        ullong_width = std::numeric_limits< unsigned long long >::digits;
    const std::size_t        ulong_width  = std::numeric_limits< unsigned long >::digits;
    const std::size_t        uchar_width  = std::numeric_limits< unsigned char >::digits;
    const unsigned long long ullong_value = make_value< unsigned long long >( { 0, 2, ullong_width - 1 } );
    const unsigned long      ulong_value  = make_value< unsigned long >( { 0, 2, ulong_width - 1 } );
    const unsigned char      uchar_value  = make_value< unsigned char >( { 0, 2, uchar_width - 1 } );

    // The first bitset is wider than unsigned long long.
    return Bitset( size, ullong_value ).template to_number< unsigned long long >() == ullong_value
        && Bitset( ulong_width, ulong_value ).to_ulong() == ulong_value
        && Bitset( uchar_width, uchar_value ).template to_number< unsigned char >() == uchar_value
        && Bitset().to_ulong() == 0;
}

template< typename Bitset >
constexpr bool
converts_to_and_from_ranges_of_blocks()
{
    typedef typename Bitset::block_type Block;

    const Bitset                        b = make_bitset< Bitset >( size, { 0, 2, last_bit } );
    Bitset                              c( size );
    Block                               blocks[ blocks_for< Bitset >( size ) ] = {};
    boost::to_block_range( b, blocks );
    boost::from_block_range( std::begin( blocks ), std::end( blocks ), c );

    return c == b && blocks[ 0 ] == make_value< Block >( { 0, 2 } );
}

#    if defined( __cpp_lib_constexpr_string ) && __cpp_lib_constexpr_string >= 201907L

template< typename Bitset >
constexpr bool
converts_to_and_from_basic_strings()
{
    const std::size_t n      = Bitset::bits_per_block;
    const std::string digits = "10110";
    const Bitset      b( "xx" + digits, 2 ); // skips the "xx"
    std::string       s;
    std::string       dump;
    boost::to_string( b, s );
    boost::dump_to_string( b, dump );

    return b == Bitset( digits.size(), 0b10110ull ) && s == digits
        && dump == std::string( n - digits.size(), '0' ) + digits;
}

#    endif

// Instantiating this class evaluates all the checks for Bitset.
template< typename Bitset >
struct constexpr_tests
{
    static_assert( size > Bitset::bits_per_block && size % Bitset::bits_per_block != 0 );
    static_assert( large_size > 3 * Bitset::bits_per_block );

    static_assert( constructs_from_sizes_and_values< Bitset >() );
    static_assert( constructs_from_ranges_of_blocks< Bitset >() );
    static_assert( constructs_from_pointers_and_string_views< Bitset >() );
    static_assert( copies_and_moves< Bitset >() );
    static_assert( swaps< Bitset >() );
    static_assert( manages_capacity< Bitset >() );
    static_assert( pushes_and_pops< Bitset >() );
    static_assert( resizes_and_clears< Bitset >() );
    static_assert( appends< Bitset >() );
    static_assert( sets_resets_and_flips_single_bits< Bitset >() );
    static_assert( sets_resets_and_flips_ranges< Bitset >() );
    static_assert( tests_bits< Bitset >() );
    static_assert( uses_the_reference_proxy< Bitset >() );
    static_assert( assigns_through_the_reference_proxy< Bitset >() );
    static_assert( does_bitwise_operations< Bitset >() );
    static_assert( does_compound_bitwise_assignments< Bitset >() );
    static_assert( shifts< Bitset >() );
    static_assert( extracts< Bitset >() );
    static_assert( compares< Bitset >() );
    static_assert( checks_set_relations< Bitset >() );
    static_assert( finds_set_bits< Bitset >() );
    static_assert( finds_unset_bits< Bitset >() );
    static_assert( iterates< Bitset >() );
    static_assert( iterates_in_reverse< Bitset >() );
    static_assert( converts_to_numbers< Bitset >() );
    static_assert( converts_to_and_from_ranges_of_blocks< Bitset >() );
#    if defined( __cpp_lib_constexpr_string ) && __cpp_lib_constexpr_string >= 201907L
    static_assert( converts_to_and_from_basic_strings< Bitset >() );
#    endif
};

template struct constexpr_tests< boost::dynamic_bitset< unsigned char > >;
template struct constexpr_tests< boost::dynamic_bitset< unsigned short > >;
template struct constexpr_tests< boost::dynamic_bitset< unsigned int > >;
template struct constexpr_tests< boost::dynamic_bitset< unsigned long > >;
template struct constexpr_tests< boost::dynamic_bitset< unsigned long long > >;

}

#endif

int
main()
{
}
