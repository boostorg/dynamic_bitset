// -----------------------------------------------------------
//              Copyright (c) 2001 Jeremy Siek
//         Copyright (c) 2003-2006, 2025 Gennaro Prota
//             Copyright (c) 2014 Ahmed Charles
//            Copyright (c) 2014 Riccardo Marcangelo
//
// Copyright (c) 2014 Glen Joseph Fernandes
// (glenjofe@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

#include "bitset_test.hpp"
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#include <cstdlib>
#include <limits>
#include <new>
#if ! defined( BOOST_NO_CXX17_HDR_MEMORY_RESOURCE )
#    include <memory_resource>
#endif

template< typename T >
class minimal_allocator
{
public:
    typedef T value_type;

    minimal_allocator()
    {
    }

    template< typename U >
    minimal_allocator( const minimal_allocator< U > & )
    {
    }

    T *
    allocate( std::size_t n )
    {
        void * p = std::malloc( sizeof( T ) * n );
        if ( ! p ) {
            throw std::bad_alloc();
        }
        return static_cast< T * >( p );
    }

    void
    deallocate( T * p, std::size_t )
    {
        std::free( p );
    }
};

// A std::vector whose iterators are raw pointers. It redefines only the
// functions which the iterators of dynamic_bitset use.
template< typename T >
class pointer_vector
    : public std::vector< T >
{
public:
    typedef T *       iterator;
    typedef const T * const_iterator;

    using std::vector< T >::vector;

    iterator
    begin()
    {
        return this->data();
    }

    iterator
    end()
    {
        return this->data() + this->size();
    }

    const_iterator
    cbegin() const
    {
        return this->data();
    }

    const_iterator
    cend() const
    {
        return this->data() + this->size();
    }
};

#define BOOST_BITSET_TEST_COUNT( x ) ( sizeof( x ) / sizeof( x[ 0 ] ) )

template< typename Tests, typename String >
void
run_string_tests( const String & s )
{
    const std::size_t len  = s.length();
    const std::size_t step = len / 4 ? len / 4 : 1;

    // bitset length determined by the string-related arguments
    std::size_t       i;
    for ( i = 0; i <= len / 2; i += step ) {
        Tests::from_string( s, i, len / 2 );     // len/2 - i bits
        Tests::from_string( s, i, len );         // len - i   bits
        Tests::from_string( s, i, 1 + len * 2 ); // len - i   bits
    }

    // bitset length explicitly specified
    for ( i = 0; i <= len / 2; i += step ) {
        for ( std::size_t sz = 0; sz <= len * 4; sz += step * 2 ) {
            Tests::from_string( s, i, len / 2, sz );
            Tests::from_string( s, i, len, sz );
            Tests::from_string( s, i, 1 + len * 2, sz );
        }
    }
}

// tests the do-the-right-thing constructor dispatch
template< typename Tests, typename T >
void
run_numeric_ctor_tests()
{
    const int bits_per_block = Tests::bits_per_block;
    const int width          = std::numeric_limits< T >::digits;
    const T   ma             = ( std::numeric_limits< T >::max )();
    const T   mi             = ( std::numeric_limits< T >::min )();

    int       sizes[]        = {
        0, 7 * width / 10, width, 13 * width / 10, 3 * width, 7 * bits_per_block / 10, bits_per_block, 13 * bits_per_block / 10, 3 * bits_per_block
    };

    const T numbers[] = {
        T( -1 ), T( -3 ), T( -8 ), T( -15 ), T( mi / 2 ), T( mi ), T( 0 ), T( 1 ), T( 3 ), T( 8 ), T( 15 ), T( ma / 2 ), T( ma )
    };

    for ( std::size_t s = 0; s < BOOST_BITSET_TEST_COUNT( sizes ); ++s ) {
        for ( std::size_t n = 0; n < BOOST_BITSET_TEST_COUNT( numbers ); ++n ) {
            // can match ctor from ulong or templated one
            Tests::from_unsigned_long( sizes[ s ], numbers[ n ] );

            typedef std::size_t compare_type;
            const compare_type  sz = sizes[ s ];
            // this condition is to be sure that size is representable in T, so
            // that for signed T's we avoid implementation-defined behavior [if ma
            // is larger than what std::size_t can hold then this is ok for our
            // purposes: our sizes are anyhow < max(size_t)], which in turn could
            // make the first argument of from_unsigned_long() a small negative,
            // later converted to a very large unsigned. Example: signed 8-bit
            // char (CHAR_MAX=127), bits_per_block=64, sz = 192 > 127.
            const bool          fits =
                sz <= static_cast< compare_type >( ma );

            if ( fits ) {
                // can match templated ctor only (so we test dispatching)
                Tests::from_unsigned_long( static_cast< T >( sizes[ s ] ), numbers[ n ] );
            }
        }
    }
}

template< typename Block, typename AllocatorOrContainer = std::allocator< Block > >
void
run_test_cases()
{
    typedef boost::dynamic_bitset< Block, AllocatorOrContainer >
                                           bitset_type;
    typedef bitset_test< bitset_type >     Tests;
    const int                              bits_per_block = bitset_type::bits_per_block;

    const std::string                      long_string    = get_long_string();
    const Block                            all_1s         = static_cast< Block >( -1 );

    //=====================================================================
    // Test construction from unsigned long
    {
        // NOTE:
        //
        // 1. keep this in sync with the numeric types supported
        //    for constructor dispatch (of course)
        // 2. bool is tested separately; ugly and inelegant, but
        //    we don't have much time to think of a better solution
        //    which is likely to work on broken compilers
        //
        const int sizes[] = {
            0, 1, 3, 7 * bits_per_block / 10, bits_per_block, 13 * bits_per_block / 10, 3 * bits_per_block
        };

        const bool values[] = { false, true };

        for ( std::size_t s = 0; s < BOOST_BITSET_TEST_COUNT( sizes ); ++s ) {
            for ( std::size_t v = 0; v < BOOST_BITSET_TEST_COUNT( values ); ++v ) {
                Tests::from_unsigned_long( sizes[ s ], values[ v ] );
                Tests::from_unsigned_long( sizes[ s ] != 0, values[ v ] );
            }
        }

        run_numeric_ctor_tests< Tests, char >();
        run_numeric_ctor_tests< Tests, wchar_t >();

        run_numeric_ctor_tests< Tests, signed char >();
        run_numeric_ctor_tests< Tests, short int >();
        run_numeric_ctor_tests< Tests, int >();
        run_numeric_ctor_tests< Tests, long int >();
        run_numeric_ctor_tests< Tests, long long >();

        run_numeric_ctor_tests< Tests, unsigned char >();
        run_numeric_ctor_tests< Tests, unsigned short >();
        run_numeric_ctor_tests< Tests, unsigned int >();
        run_numeric_ctor_tests< Tests, unsigned long >();
        run_numeric_ctor_tests< Tests, unsigned long long >();
    }
    //=====================================================================
    // Test construction from a string
    {
        run_string_tests< Tests >( std::string( "" ) ); // empty string
        run_string_tests< Tests >( std::string( "1" ) );

        run_string_tests< Tests >( long_string );

        // I need to decide what to do for non "C" locales here. On
        // one hand I should have better tests. On the other one
        // I don't want tests for dynamic_bitset to cope with locales,
        // ctype::widen, etc. (but that's what you deserve when you
        // don't separate concerns at the library level)
        //
        run_string_tests< Tests >(
            std::wstring( L"11111000000111111111010101010101010101010111111" ) );

        // Note that these are _valid_ arguments
        Tests::from_string( std::string( "x11y" ), 1, 2 );
        Tests::from_string( std::string( "x11" ), 1, 10 );
        Tests::from_string( std::string( "x11" ), 1, 10, 10 );
    }
    //=====================================================================
    // test from_block_range
    {
        std::vector< Block > blocks;
        Tests::from_block_range( blocks );
    }
    {
        std::vector< Block > blocks( 3 );
        blocks[ 0 ] = static_cast< Block >( 0 );
        blocks[ 1 ] = static_cast< Block >( 1 );
        blocks[ 2 ] = all_1s;
        Tests::from_block_range( blocks );
    }
    {
        const unsigned int   n = ( std::numeric_limits< unsigned char >::max )();
        std::vector< Block > blocks( n );
        for ( typename std::vector< Block >::size_type i = 0; i < n; ++i )
            blocks[ i ] = static_cast< Block >( i );
        Tests::from_block_range( blocks );
    }
    {
        // Iterators which throw after the last block.
        const std::vector< Block > blocks( 3, all_1s );
        Tests::from_block_range_throwing( blocks );
    }

    //=====================================================================
    // test iterators
    Tests::value_initialized_iterators();
    Tests::iterator_concepts();
    Tests::mutating_iterator_concepts();
    {
        bitset_type b;
        Tests::iterate_forward( b );
        Tests::iterate_backward( b );
        Tests::iterator_operations( b );
        Tests::const_iterators_of_non_const( b );
        Tests::iterate_with_cbegin_and_crbegin( b );
        Tests::write_through_iterators( b );
        Tests::mutating_std_algorithms( b );
        Tests::swap_references( b );
        Tests::swapping_std_algorithms( b );
        Tests::assigning_ranges_algorithms( b );
        Tests::swapping_ranges_algorithms( b );
        Tests::iterators_with_ranges( b );
    }
    {
        bitset_type b( 1, 1ul );
        Tests::iterate_forward( b );
        Tests::iterate_backward( b );
        Tests::iterator_operations( b );
        Tests::const_iterators_of_non_const( b );
        Tests::iterate_with_cbegin_and_crbegin( b );
        Tests::write_through_iterators( b );
        Tests::mutating_std_algorithms( b );
        Tests::swap_references( b );
        Tests::swapping_std_algorithms( b );
        Tests::assigning_ranges_algorithms( b );
        Tests::swapping_ranges_algorithms( b );
        Tests::iterators_with_ranges( b );
    }
    {
        bitset_type b( bitset_type::bits_per_block, 100ul );
        Tests::iterate_forward( b );
        Tests::iterate_backward( b );
        Tests::iterator_operations( b );
        Tests::const_iterators_of_non_const( b );
        Tests::iterate_with_cbegin_and_crbegin( b );
        Tests::write_through_iterators( b );
        Tests::mutating_std_algorithms( b );
        Tests::swap_references( b );
        Tests::swapping_std_algorithms( b );
        Tests::assigning_ranges_algorithms( b );
        Tests::swapping_ranges_algorithms( b );
        Tests::iterators_with_ranges( b );
    }
    {
        bitset_type b( long_string );
        Tests::iterate_forward( b );
        Tests::iterate_backward( b );
        Tests::iterator_operations( b );
        Tests::iterator_backward_offsets( b );
        Tests::const_iterators_of_non_const( b );
        Tests::iterate_with_cbegin_and_crbegin( b );
        Tests::write_through_iterators( b );
        Tests::mutating_std_algorithms( b );
        Tests::swap_references( b );
        Tests::swapping_std_algorithms( b );
        Tests::assigning_ranges_algorithms( b );
        Tests::swapping_ranges_algorithms( b );
        Tests::iterators_with_ranges( b );
        Tests::mixed_iterator_operations( b );
    }
    {
        typedef boost::dynamic_bitset< Block, pointer_vector< Block > > Bitset;
        bitset_test< Bitset >::iterator_backward_offsets( Bitset( long_string ) );
    }

    //=====================================================================
    // test to_block_range
    {
        bitset_type b;
        Tests::to_block_range( b );
    }
    {
        bitset_type b( 1, 1ul );
        Tests::to_block_range( b );
    }
    {
        bitset_type b( long_string );
        Tests::to_block_range( b );
    }

    //=====================================================================
    // Test copy constructor
    {
        bitset_type b;
        Tests::copy_constructor( b );
    }
    {
        bitset_type b( "0" );
        Tests::copy_constructor( b );
    }
    {
        bitset_type b ( long_string );
        Tests::copy_constructor( b );
    }
    //=====================================================================
    // Test copy assignment operator
    {
        bitset_type a, b;
        Tests::copy_assignment_operator( a, b );
    }
    {
        bitset_type a( "1" ), b( "0" );
        Tests::copy_assignment_operator( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        Tests::copy_assignment_operator( a, b );
    }
    {
        bitset_type a;
        bitset_type b( long_string ); // b greater than a, a empty
        Tests::copy_assignment_operator( a, b );
    }
    {
        bitset_type a( "0" );
        bitset_type b( long_string ); // b greater than a
        Tests::copy_assignment_operator( a, b );
    }

    //=====================================================================
    // Test move constructor
    {
        bitset_type b;
        Tests::move_constructor( b );
    }
    {
        bitset_type b( "0" );
        Tests::move_constructor( b );
    }
    {
        bitset_type b( long_string );
        Tests::move_constructor( b );
    }
    //=====================================================================
    // Test move assignment operator
    {
        bitset_type a, b;
        Tests::move_assignment_operator( a, b );
    }
    {
        bitset_type a( "1" ), b( "0" );
        Tests::move_assignment_operator( a, b );
    }
    {
        bitset_type a( long_string ), b( long_string );
        Tests::move_assignment_operator( a, b );
    }
    {
        bitset_type a;
        bitset_type b( long_string ); // b greater than a, a empty
        Tests::move_assignment_operator( a, b );
    }
    {
        bitset_type a( "0" );
        bitset_type b( long_string ); // b greater than a
        Tests::move_assignment_operator( a, b );
    }
    //=====================================================================
    // Test swap
    {
        bitset_type a;
        bitset_type b( "1" );
        Tests::swap( a, b );
        Tests::swap( b, a );
        Tests::swap( a, a );
    }
    {
        bitset_type a;
        bitset_type b( long_string );
        Tests::swap( a, b );
        Tests::swap( b, a );
    }
    {
        bitset_type a( "0" );
        bitset_type b( long_string );
        Tests::swap( a, b );
        Tests::swap( b, a );
        Tests::swap( a, a );
        Tests::swap( b, b );
    }
    //=====================================================================
    // Test resize
    {
        bitset_type a;
        Tests::resize( a );
    }
    {
        bitset_type a( "0" );
        Tests::resize( a );
    }
    {
        bitset_type a( "1" );
        Tests::resize( a );
    }
    {
        bitset_type a( long_string );
        Tests::resize( a );
    }
    //=====================================================================
    // Test clear
    {
        bitset_type a;
        Tests::clear( a );
    }
    {
        bitset_type a( long_string );
        Tests::clear( a );
    }
    //=====================================================================
    // Test pop back
    {
        bitset_type a( "01" );
        Tests::pop_back( a );
    }
    {
        bitset_type a( "10" );
        Tests::pop_back( a );
    }
    {
        const int   size_to_fill_all_blocks = 4 * bits_per_block;
        bitset_type a( size_to_fill_all_blocks, 255ul );
        Tests::pop_back( a );
    }
    {
        bitset_type a( long_string.c_str() );
        Tests::pop_back( a );
    }
    //=====================================================================
    // Test pop_front
    {
        bitset_type a( "01" );
        Tests::pop_front( a );
    }
    {
        bitset_type a( "10" );
        Tests::pop_front( a );
    }
    {
        const int   size_to_fill_all_blocks = 4 * bits_per_block;
        bitset_type a( size_to_fill_all_blocks, 255ul );
        Tests::pop_front( a );
    }
    {
        bitset_type a( long_string.c_str() );
        Tests::pop_front( a );
    }
    //=====================================================================
    // Test append bit
    {
        bitset_type a;
        Tests::append_bit( a );
    }
    {
        bitset_type a( "0" );
        Tests::append_bit( a );
    }
    {
        bitset_type a( "1" );
        Tests::append_bit( a );
    }
    {
        const int   size_to_fill_all_blocks = 4 * bits_per_block;
        bitset_type a( size_to_fill_all_blocks, 255ul );
        Tests::append_bit( a );
    }
    {
        bitset_type a( long_string );
        Tests::append_bit( a );
    }
    //=====================================================================
    // Test push_front
    {
        bitset_type a;
        Tests::prepend_bit( a );
    }
    {
        bitset_type a( "0" );
        Tests::prepend_bit( a );
    }
    {
        bitset_type a( "1" );
        Tests::prepend_bit( a );
    }
    {
        const int   size_to_fill_all_blocks = 4 * bits_per_block;
        bitset_type a( size_to_fill_all_blocks, 255ul );
        Tests::prepend_bit( a );
    }
    {
        bitset_type a( long_string );
        Tests::prepend_bit( a );
    }
    //=====================================================================
    // Test append block
    {
        bitset_type a;
        Tests::append_block( a );
    }
    {
        bitset_type a( "0" );
        Tests::append_block( a );
    }
    {
        bitset_type a( "1" );
        Tests::append_block( a );
    }
    {
        const int   size_to_fill_all_blocks = 4 * bits_per_block;
        bitset_type a( size_to_fill_all_blocks, 15ul );
        Tests::append_block( a );
    }
    {
        bitset_type a( long_string.c_str() );
        Tests::append_block( a );
    }
    //=====================================================================
    // Test append block range
    {
        bitset_type          a;
        std::vector< Block > blocks;
        Tests::append_block_range( a, blocks );
    }
    {
        bitset_type          a( "0" );
        std::vector< Block > blocks( 3 );
        blocks[ 0 ] = static_cast< Block >( 0 );
        blocks[ 1 ] = static_cast< Block >( 1 );
        blocks[ 2 ] = all_1s;
        Tests::append_block_range( a, blocks );
    }
    {
        bitset_type          a( "1" );
        const unsigned int   n = ( std::numeric_limits< unsigned char >::max )();
        std::vector< Block > blocks( n );
        for ( typename std::vector< Block >::size_type i = 0; i < n; ++i )
            blocks[ i ] = static_cast< Block >( i );
        Tests::append_block_range( a, blocks );
    }
    {
        bitset_type a;
        a.append( Block( 1 ) );
        a.append( Block( 2 ) );
        Block                x[] = { 3, 4, 5 };
        std::size_t          sz  = sizeof( x ) / sizeof( x[ 0 ] );
        std::vector< Block > blocks( x, x + sz );
        Tests::append_block_range( a, blocks );
    }
    {
        bitset_type          a( long_string.c_str() );
        std::vector< Block > blocks( 3 );
        blocks[ 0 ] = static_cast< Block >( 0 );
        blocks[ 1 ] = static_cast< Block >( 1 );
        blocks[ 2 ] = all_1s;
        Tests::append_block_range( a, blocks );
    }
    {
        // A range of unsigned char, which is narrower than Block unless
        // Block is unsigned char.
        const std::vector< unsigned char > chars( 3, ( std::numeric_limits< unsigned char >::max )() );
        Tests::append_block_range( bitset_type( long_string ), chars );
        Tests::append_block_range( bitset_type( bitset_type::bits_per_block - 1 ), chars );
    }
    {
        // Repeated appends of small ranges must grow the buffer
        // geometrically, as push_back() does, rather than reallocate it
        // at each call. With a growth factor of 1.5, the 2000 blocks
        // need about 20 allocations.
        const Block blocks[] = { 1, 2 };
        for ( std::size_t initial_size = 0; initial_size < 2; ++initial_size ) {
            bitset_type b( initial_size );
            int         allocation_count = 0;
            for ( int i = 0; i < 1000; ++i ) {
                const std::size_t capacity = b.capacity();
                b.append( blocks, blocks + 2 );
                if ( b.capacity() != capacity ) {
                    ++allocation_count;
                }
            }
            BOOST_TEST_LT( allocation_count, 40 );
        }
    }
    // Test with input iterators
    {
        bitset_type b;
        Tests::append_block_range_input_iter( b );
    }
    {
        bitset_type b( "0" );
        Tests::append_block_range_input_iter( b );
    }
    {
        bitset_type b( long_string.c_str() );
        Tests::append_block_range_input_iter( b );
    }
    //=====================================================================
    // Test bracket operator
    Tests::reference_traits();
    {
        bitset_type         b1;
        std::vector< bool > bitvec1;
        Tests::operator_bracket( b1, bitvec1 );
    }
    {
        bitset_type         b( "1" );
        std::vector< bool > bit_vec( 1, true );
        Tests::operator_bracket( b, bit_vec );
    }
    {
        bitset_type         b( long_string.c_str() );
        std::size_t         n = long_string.size();
        std::vector< bool > bit_vec( n );
        for ( std::size_t i = 0; i < n; ++i )
            bit_vec[ i ] = long_string[ n - 1 - i ] == '0' ? 0 : 1;
        Tests::operator_bracket( b, bit_vec );
    }
    //=====================================================================
    // Test at
    {
        bitset_type         b1;
        std::vector< bool > bitvec1;
        Tests::at( b1, bitvec1 );
    }
    {
        bitset_type         b( "1" );
        std::vector< bool > bit_vec( 1, true );
        Tests::at( b, bit_vec );
    }
    {
        bitset_type         b( long_string.c_str() );
        std::size_t         n = long_string.size();
        std::vector< bool > bit_vec( n );
        for ( std::size_t i = 0; i < n; ++i )
            bit_vec[ i ] = long_string[ n - 1 - i ] == '0' ? 0 : 1;
        Tests::at( b, bit_vec );
    }
    //=====================================================================
    // Test max_size
    {
        typedef boost::dynamic_bitset< Block, minimal_allocator< Block > > Bitset;
        Bitset                                                             b;
        bitset_test< Bitset >::max_size( b );
    }
    {
        typedef boost::dynamic_bitset< Block, small_vector< Block > > Bitset;
        Bitset                                                        b;
        bitset_test< Bitset >::max_size( b );
    }
    // Test copy-initialize with default constructor
    {
        bitset_type b[ 1 ] = {};
        (void)b;
    }
#if ! defined( BOOST_NO_CXX17_HDR_MEMORY_RESOURCE )
    //=====================================================================
    // A std::pmr container passes its allocator to the bitsets it
    // constructs, which requires the allocator-extended constructors.
    {
        typedef boost::dynamic_bitset< Block, std::pmr::polymorphic_allocator< Block > > Bitset;

        std::pmr::monotonic_buffer_resource                                              resource;
        std::pmr::vector< Bitset >                                                       v( &resource );
        const Bitset                                                                     b( 70, 5ul );
        v.push_back( b );
        v.emplace_back( 70 );
        v.reserve( 2 * v.capacity() );
        BOOST_TEST( v[ 0 ] == b );
        BOOST_TEST( v[ 1 ] == Bitset( 70 ) );
        BOOST_TEST( v[ 0 ].get_allocator().resource() == &resource );
        BOOST_TEST( v[ 1 ].get_allocator().resource() == &resource );

        // A polymorphic_allocator is implicitly constructible from 0, a null
        // pointer constant, but ( size, 0 ) must still mean ( size, value ).
        BOOST_TEST( Bitset( b.size(), 0 ).get_allocator().resource() == std::pmr::get_default_resource() );

        // A copy, and the result of an operator, get the allocator which
        // select_on_container_copy_construction() gives, i.e. the default
        // memory resource. A copy assignment keeps the allocator of the
        // target, since a polymorphic_allocator doesn't propagate.
        BOOST_TEST( Bitset( v[ 0 ] ).get_allocator().resource() == std::pmr::get_default_resource() );
        BOOST_TEST( ( ~v[ 0 ] ).get_allocator().resource() == std::pmr::get_default_resource() );
        v[ 1 ] = b;
        BOOST_TEST( v[ 1 ].get_allocator().resource() == &resource );
    }
#endif
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
