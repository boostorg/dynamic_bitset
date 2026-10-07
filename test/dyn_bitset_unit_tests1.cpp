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
#include <list>
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

// A std::vector whose iterators are made from pointers: by default,
// they are raw pointers. It redefines only the functions which the
// iterators of dynamic_bitset use.
template< typename T, typename Iterator = T *, typename ConstIterator = const T * >
class pointer_vector
    : public std::vector< T >
{
public:
    typedef Iterator      iterator;
    typedef ConstIterator const_iterator;

    using std::vector< T >::vector;

    iterator
    begin()
    {
        return iterator( this->data() );
    }

    iterator
    end()
    {
        return iterator( this->data() + this->size() );
    }

    const_iterator
    cbegin() const
    {
        return const_iterator( this->data() );
    }

    const_iterator
    cend() const
    {
        return const_iterator( this->data() + this->size() );
    }
};

// An iterator whose difference type is narrower than std::ptrdiff_t.
template< typename T >
class short_difference_iterator
{
public:
    typedef std::random_access_iterator_tag       iterator_category;
    typedef typename std::remove_const< T >::type value_type;
    typedef short                                 difference_type;
    typedef T *                                   pointer;
    typedef T &                                   reference;

    short_difference_iterator()
        : m_ptr()
    {
    }

    explicit short_difference_iterator( T * ptr )
        : m_ptr( ptr )
    {
    }

    T &
    operator*() const
    {
        return *m_ptr;
    }

    T &
    operator[]( difference_type n ) const
    {
        return m_ptr[ n ];
    }

    short_difference_iterator &
    operator++()
    {
        ++m_ptr;
        return *this;
    }

    short_difference_iterator
    operator++( int )
    {
        const short_difference_iterator old = *this;
        ++m_ptr;
        return old;
    }

    short_difference_iterator &
    operator--()
    {
        --m_ptr;
        return *this;
    }

    short_difference_iterator
    operator--( int )
    {
        const short_difference_iterator old = *this;
        --m_ptr;
        return old;
    }

    short_difference_iterator &
    operator+=( difference_type n )
    {
        m_ptr += n;
        return *this;
    }

    short_difference_iterator &
    operator-=( difference_type n )
    {
        m_ptr -= n;
        return *this;
    }

    friend short_difference_iterator
    operator+( short_difference_iterator it, difference_type n )
    {
        return it += n;
    }

    friend short_difference_iterator
    operator+( difference_type n, short_difference_iterator it )
    {
        return it += n;
    }

    friend short_difference_iterator
    operator-( short_difference_iterator it, difference_type n )
    {
        return it -= n;
    }

    friend difference_type
    operator-( const short_difference_iterator & lhs, const short_difference_iterator & rhs )
    {
        return static_cast< difference_type >( lhs.m_ptr - rhs.m_ptr );
    }

    friend bool
    operator==( const short_difference_iterator & lhs, const short_difference_iterator & rhs )
    {
        return lhs.m_ptr == rhs.m_ptr;
    }

    friend bool
    operator!=( const short_difference_iterator & lhs, const short_difference_iterator & rhs )
    {
        return ! ( lhs == rhs );
    }

    friend bool
    operator<( const short_difference_iterator & lhs, const short_difference_iterator & rhs )
    {
        return lhs.m_ptr < rhs.m_ptr;
    }

    friend bool
    operator<=( const short_difference_iterator & lhs, const short_difference_iterator & rhs )
    {
        return ! ( rhs < lhs );
    }

    friend bool
    operator>( const short_difference_iterator & lhs, const short_difference_iterator & rhs )
    {
        return rhs < lhs;
    }

    friend bool
    operator>=( const short_difference_iterator & lhs, const short_difference_iterator & rhs )
    {
        return ! ( lhs < rhs );
    }

private:
    T * m_ptr;
};

// A std::list with the members which dynamic_bitset needs from a
// container, so that the iterators of a bitset which uses it are only
// bidirectional.
template< typename T >
class list_with_subscript
    : public std::list< T >
{
public:
    using std::list< T >::list;

    T &
    operator[]( std::size_t i )
    {
        return *std::next( this->begin(), static_cast< std::ptrdiff_t >( i ) );
    }

    const T &
    operator[]( std::size_t i ) const
    {
        return *std::next( this->begin(), static_cast< std::ptrdiff_t >( i ) );
    }

    std::size_t
    capacity() const
    {
        return this->size();
    }
};

// Character traits under which 'T' and 'F' are equal to '1' and '0',
// respectively.
struct true_false_traits : std::char_traits< char >
{
    static char
    canonical( char c )
    {
        if ( c == 'T' ) {
            return '1';
        } else if ( c == 'F' ) {
            return '0';
        }
        return c;
    }

    static bool
    eq( char a, char b )
    {
        return canonical( a ) == canonical( b );
    }
};

// The constructor from a pointer to a string takes only pointers to
// character types.
template< typename T >
using is_constructible_from_pointer_to = std::is_constructible< boost::dynamic_bitset<>, const T * >;

#if defined( __cpp_lib_constexpr_string ) && __cpp_lib_constexpr_string >= 201907L \
    && defined( __cpp_lib_constexpr_vector ) && __cpp_lib_constexpr_vector >= 201907L

// The string constructors and to_string() don't depend on any locale,
// so they can be used in constant expressions.
constexpr bool
converts_in_constant_expressions()
{
    std::string    s;
    std::u16string u;
    boost::to_string( boost::dynamic_bitset<>( std::string( "1101" ) ), s );
    boost::to_string( boost::dynamic_bitset<>( u"0110" ), u );

    return s == "1101" && u == u"0110"
        && boost::dynamic_bitset<>( std::string_view( "0111" ) ).count() == 3;
}

#endif

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

        run_string_tests< Tests >(
            std::wstring( L"11111000000111111111010101010101010101010111111" ) );
        run_string_tests< Tests >( std::u16string( long_string.begin(), long_string.end() ) );
        run_string_tests< Tests >( std::u32string( long_string.begin(), long_string.end() ) );
#if defined( __cpp_lib_char8_t ) && __cpp_lib_char8_t >= 201811L
        run_string_tests< Tests >( std::u8string( long_string.begin(), long_string.end() ) );
#endif

        // Note that these are _valid_ arguments
        Tests::from_string( std::string( "x11y" ), 1, 2 );
        Tests::from_string( std::string( "x11" ), 1, 10 );
        Tests::from_string( std::string( "x11" ), 1, 10, 10 );
    }
    {
        // The string constructors compare the characters with the
        // Traits::eq() of the string.
        typedef std::basic_string< char, true_false_traits > tf_string;
        BOOST_TEST( bitset_type( tf_string( "TFTF" ) ) == bitset_type( 4, 10ul ) );
#if ! defined( BOOST_NO_CXX17_HDR_STRING_VIEW )
        BOOST_TEST( bitset_type( std::basic_string_view< char, true_false_traits >( "TFTF" ) ) == bitset_type( 4, 10ul ) );
#endif
    }
    static_assert( is_constructible_from_pointer_to< char >::value, "" );
    static_assert( is_constructible_from_pointer_to< wchar_t >::value, "" );
#if defined( __cpp_char8_t ) && __cpp_char8_t >= 201811L
    static_assert( is_constructible_from_pointer_to< char8_t >::value, "" );
#endif
    static_assert( is_constructible_from_pointer_to< char16_t >::value, "" );
    static_assert( is_constructible_from_pointer_to< char32_t >::value, "" );
    static_assert( std::is_constructible< boost::dynamic_bitset<>, char * >::value, "" );
    static_assert( ! is_constructible_from_pointer_to< signed char >::value, "" );
    static_assert( ! is_constructible_from_pointer_to< unsigned char >::value, "" );
    static_assert( ! is_constructible_from_pointer_to< unsigned long >::value, "" );
#if defined( __cpp_lib_memory_resource ) && __cpp_lib_memory_resource >= 201603L
    {
        // So, a pointer to a memory resource selects the constructor from
        // an allocator.
        typedef boost::dynamic_bitset< Block, std::pmr::polymorphic_allocator< Block > > Bitset;

        std::pmr::monotonic_buffer_resource                                              resource;
        const Bitset                                                                     b( &resource );
        BOOST_TEST( b.size() == 0 );
        BOOST_TEST( b.get_allocator().resource() == &resource );

        std::pmr::memory_resource * const mr = std::pmr::new_delete_resource();
        const Bitset                      c( mr );
        BOOST_TEST( c.get_allocator().resource() == mr );
    }
#endif
#if defined( __cpp_lib_constexpr_string ) && __cpp_lib_constexpr_string >= 201907L \
    && defined( __cpp_lib_constexpr_vector ) && __cpp_lib_constexpr_vector >= 201907L
    static_assert( converts_in_constant_expressions(), "" );
#endif
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
    {
        // More bits than the maximum value of the difference type of the
        // iterators of the underlying container.
        typedef boost::dynamic_bitset< Block, pointer_vector< Block, short_difference_iterator< Block >, short_difference_iterator< const Block > > > Bitset;
        bitset_test< Bitset >::iterator_backward_offsets( Bitset( 40000 ) );
    }
    {
        typedef boost::dynamic_bitset< Block, list_with_subscript< Block > > Bitset;
        bitset_test< Bitset >::bidirectional_iterators( Bitset() );
        bitset_test< Bitset >::bidirectional_iterators( Bitset( 1, 1ul ) );
        bitset_test< Bitset >::bidirectional_iterators( Bitset( 3 * Bitset::bits_per_block + 5, 0x5A5Aul ) );
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
