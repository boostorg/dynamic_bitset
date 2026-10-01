// -----------------------------------------------------------
//                Copyright 2026 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

// Tests of the move constructor and of the move assignment operator,
// including with allocators and containers which make the move of the
// underlying buffer leave the source non-empty, or throw.

#include "boost/config.hpp"
#include "boost/core/lightweight_test.hpp"
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

#if ! defined( BOOST_NO_CXX17_HDR_MEMORY_RESOURCE )
#    include <memory_resource>
#endif

// An allocator which doesn't propagate on move assignment and whose
// instances compare equal only if they have the same id. While
// `*fail_flag` is true, allocations throw std::bad_alloc.
template< typename T >
class unequal_allocator
{
public:
    typedef T value_type;

    unequal_allocator()
        : m_id( 0 ), m_fail_flag( nullptr )
    {
    }

    explicit unequal_allocator( int identifier, const bool * fail_flag = nullptr )
        : m_id( identifier ), m_fail_flag( fail_flag )
    {
    }

    template< typename U >
    unequal_allocator( const unequal_allocator< U > & other )
        : m_id( other.id() ), m_fail_flag( other.fail_flag() )
    {
    }

    T *
    allocate( std::size_t n )
    {
        if ( m_fail_flag != nullptr && *m_fail_flag ) {
            throw std::bad_alloc();
        }
        return std::allocator< T >().allocate( n );
    }

    void
    deallocate( T * p, std::size_t n )
    {
        std::allocator< T >().deallocate( p, n );
    }

    int
    id() const
    {
        return m_id;
    }

    const bool *
    fail_flag() const
    {
        return m_fail_flag;
    }

private:
    int          m_id;
    const bool * m_fail_flag;
};

template< typename T, typename U >
bool
operator==( const unequal_allocator< T > & a, const unequal_allocator< U > & b )
{
    return a.id() == b.id();
}

template< typename T, typename U >
bool
operator!=( const unequal_allocator< T > & a, const unequal_allocator< U > & b )
{
    return ! ( a == b );
}

// A stateless allocator which counts its allocations.
template< typename T >
class counting_allocator
{
public:
    typedef T value_type;

    counting_allocator()
    {
    }

    template< typename U >
    counting_allocator( const counting_allocator< U > & )
    {
    }

    static std::size_t &
    allocation_count()
    {
        static std::size_t count = 0;
        return count;
    }

    T *
    allocate( std::size_t n )
    {
        ++allocation_count();
        return std::allocator< T >().allocate( n );
    }

    void
    deallocate( T * p, std::size_t n )
    {
        std::allocator< T >().deallocate( p, n );
    }
};

template< typename T, typename U >
bool
operator==( const counting_allocator< T > &, const counting_allocator< U > & )
{
    return true;
}

template< typename T, typename U >
bool
operator!=( const counting_allocator< T > &, const counting_allocator< U > & )
{
    return false;
}

// A container whose move operations copy, and thus leave the source
// unchanged. That's allowed, as the state of a moved-from object is
// unspecified.
template< typename T >
class copying_vector : public std::vector< T >
{
public:
    copying_vector()                                     = default;
    copying_vector( const copying_vector & )             = default;
    copying_vector & operator=( const copying_vector & ) = default;

    explicit copying_vector( const std::allocator< T > & alloc )
        : std::vector< T >( alloc )
    {
    }
};

template< typename Bitset >
void
check_noexcept_specifications()
{
    typedef typename Bitset::buffer_type buffer_type;

    static_assert( std::is_nothrow_move_constructible< Bitset >::value == std::is_nothrow_move_constructible< buffer_type >::value, "" );
    static_assert( std::is_nothrow_move_assignable< Bitset >::value == std::is_nothrow_move_assignable< buffer_type >::value, "" );
}

// A moved-from bitset must be empty, and usable as such.
template< typename Bitset >
void
check_moved_from( Bitset & b )
{
    BOOST_TEST( b.size() == 0 );
    BOOST_TEST( b.num_blocks() == 0 );
    BOOST_TEST( b.count() == 0 );
    BOOST_TEST( b == Bitset( b.get_allocator() ) );

    b &= Bitset( b.get_allocator() );
    BOOST_TEST( b.size() == 0 );

    b.push_back( true );
    BOOST_TEST( b.size() == 1 );
    BOOST_TEST( b.num_blocks() == 1 );
    BOOST_TEST( b.count() == 1 );

    b.resize( 3 * Bitset::bits_per_block );
    BOOST_TEST( b.num_blocks() == 3 );
    BOOST_TEST( b.count() == 1 );
}

template< typename Bitset >
Bitset
make_full_bitset( const typename Bitset::allocator_type & alloc = typename Bitset::allocator_type() )
{
    const typename Bitset::size_type size = 4 * Bitset::bits_per_block + 1;
    Bitset                           b( size, 0ul, alloc );
    b.set();
    return b;
}

template< typename Block >
void
test_unequal_allocators()
{
    typedef unequal_allocator< Block >                     allocator_type;
    typedef boost::dynamic_bitset< Block, allocator_type > bitset_type;

    const bitset_type                                      full = make_full_bitset< bitset_type >( allocator_type( 1 ) );

    // The blocks are moved one by one, and the source keeps its
    // allocator.
    bitset_type                                            src( full );
    bitset_type                                            dst( allocator_type( 2 ) );
    dst = std::move( src );
    BOOST_TEST( dst == full );
    BOOST_TEST( dst.get_allocator().id() == 2 );
    BOOST_TEST( src.get_allocator().id() == 1 );
    check_moved_from( src );

    bitset_type moved_to( std::move( dst ) );
    BOOST_TEST( moved_to == full );
    BOOST_TEST( moved_to.get_allocator().id() == 2 );
    check_moved_from( dst );
}

template< typename Block >
void
test_throwing_move_assignment()
{
    typedef unequal_allocator< Block >                     allocator_type;
    typedef boost::dynamic_bitset< Block, allocator_type > bitset_type;

    bool                                                   fail = false;
    const bitset_type                                      full = make_full_bitset< bitset_type >( allocator_type( 1 ) );
    bitset_type                                            src( full );
    bitset_type                                            dst( allocator_type( 2, &fail ) );
    dst.push_back( true );

    // Moving the blocks requires an allocation from the allocator of
    // dst, which throws.
    bool threw = false;
    fail       = true;
    try {
        dst = std::move( src );
    } catch ( const std::bad_alloc & ) {
        threw = true;
    }
    fail = false;

    BOOST_TEST( threw );
    BOOST_TEST( dst.size() == 0 );
    BOOST_TEST( dst.num_blocks() == 0 );
    dst.push_back( true );
    BOOST_TEST( dst.size() == 1 );
    BOOST_TEST( dst.count() == 1 );

    // The standard doesn't require that, but the std::vector
    // implementations we know of leave the source unchanged when the
    // move assignment throws, and dynamic_bitset doesn't touch it
    // either.
    BOOST_TEST( src == full );
}

template< typename Block >
void
test_copying_container()
{
    typedef boost::dynamic_bitset< Block, copying_vector< Block > > bitset_type;

    const bitset_type                                               full = make_full_bitset< bitset_type >();

    bitset_type                                                     src( full );
    bitset_type                                                     dst( std::move( src ) );
    BOOST_TEST( dst == full );
    check_moved_from( src );

    bitset_type other;
    other = std::move( dst );
    BOOST_TEST( other == full );
    check_moved_from( dst );
}

// When a std::vector of bitsets grows, it must move the existing
// bitsets, not copy them.
template< typename Block >
void
test_vector_reallocation()
{
    typedef counting_allocator< Block >                    allocator_type;
    typedef boost::dynamic_bitset< Block, allocator_type > bitset_type;

    const std::size_t                                      initial_count = allocator_type::allocation_count();
    const std::size_t                                      n             = 20;
    std::vector< bitset_type >                             v;
    for ( std::size_t i = 0; i < n; ++i ) {
        v.push_back( bitset_type( 100 ) );
    }

    // One allocation per push_back() argument, and none for the
    // reallocations of v.
    BOOST_TEST_EQ( allocator_type::allocation_count() - initial_count, n );
}

#if ! defined( BOOST_NO_CXX17_HDR_MEMORY_RESOURCE )
void
test_pmr_move_assignment()
{
    typedef std::pmr::polymorphic_allocator< unsigned long >       allocator_type;
    typedef boost::dynamic_bitset< unsigned long, allocator_type > bitset_type;

    std::pmr::monotonic_buffer_resource                            r1;
    std::pmr::monotonic_buffer_resource                            r2;
    const allocator_type                                           alloc1( &r1 );
    const allocator_type                                           alloc2( &r2 );

    const bitset_type                                              full = make_full_bitset< bitset_type >();
    bitset_type                                                    src  = make_full_bitset< bitset_type >( alloc1 );
    bitset_type                                                    dst( alloc2 );
    dst = std::move( src );
    BOOST_TEST( dst == full );
    check_moved_from( src );
}
#endif

template< typename Block >
void
run_tests()
{
    check_noexcept_specifications< boost::dynamic_bitset< Block > >();
    check_noexcept_specifications< boost::dynamic_bitset< Block, unequal_allocator< Block > > >();
    check_noexcept_specifications< boost::dynamic_bitset< Block, copying_vector< Block > > >();
    static_assert( std::is_nothrow_move_constructible< boost::dynamic_bitset< Block > >::value, "" );
    static_assert( std::is_nothrow_move_assignable< boost::dynamic_bitset< Block > >::value, "" );

    test_unequal_allocators< Block >();
    test_throwing_move_assignment< Block >();
    test_copying_container< Block >();
    test_vector_reallocation< Block >();
}

int
main()
{
    run_tests< unsigned char >();
    run_tests< unsigned short >();
    run_tests< unsigned int >();
    run_tests< unsigned long >();
    run_tests< unsigned long long >();
#if ! defined( BOOST_NO_CXX17_HDR_MEMORY_RESOURCE )
    test_pmr_move_assignment();
#endif

    return boost::report_errors();
}
