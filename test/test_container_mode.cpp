//
// Copyright 2026 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//


// Tests of what is specific to AllocatorOrContainer being a container.

#include "boost/core/lightweight_test.hpp"
#include "boost/dynamic_bitset.hpp"
#include <cstddef>
#include <deque>
#include <iterator>
#include <list>
#include <memory>
#include <utility>
#include <vector>

namespace {

// A container with operator[] but only bidirectional iterators, and
// without capacity() and reserve().
template< typename T >
class indexable_list
    : public std::list< T >
{
public:
    indexable_list() = default;

    explicit indexable_list( const std::allocator< T > & alloc )
        : std::list< T >( alloc )
    {
    }

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
};

// Uses as the underlying container either a std::deque, which has no
// capacity(), or an indexable_list, whose iterators aren't
// random-access.
template< typename Container >
void
test_container()
{
    typedef boost::dynamic_bitset< unsigned, Container > bitset_type;

    bitset_type b( 70, 9ul );
    b.set( 69 );
    b >>= 3;
    BOOST_TEST_EQ( b.find_first(), 0u );
    BOOST_TEST_EQ( b.find_next( 0 ), 66u );
    BOOST_TEST( b.find_next( 66 ) == bitset_type::npos );
    BOOST_TEST_EQ( b.find_first_off(), 1 );

    const std::vector< unsigned > blocks( 2, 5u );
    b.append( blocks.begin(), blocks.end() );
    BOOST_TEST_EQ( b.size(), 70u + 2 * bitset_type::bits_per_block );
    BOOST_TEST( b.test( 70 ) && ! b.test( 71 ) && b.test( 72 ) );
}

}

namespace user {

int swap_calls = 0;

// A container which provides its own swap(), to be found by ADL, and
// doesn't declare it noexcept.
template< typename T >
class vector_with_swap
    : public std::vector< T >
{
public:
    vector_with_swap() = default;

    explicit vector_with_swap( const std::allocator< T > & alloc )
        : std::vector< T >( alloc )
    {
    }

    friend void
    swap( vector_with_swap & a, vector_with_swap & b )
    {
        ++swap_calls;
        static_cast< std::vector< T > & >( a ).swap( b );
    }
};

}

namespace impl = boost::detail::dynamic_bitset_impl;

static_assert( impl::is_container< user::vector_with_swap< unsigned >, unsigned >::value, "" );

// std::list has no operator[] and is not an allocator, so
// dynamic_bitset< unsigned, std::list< unsigned > > fails a
// static_assert.
static_assert( ! impl::is_container< std::list< unsigned >, unsigned >::value, "" );
static_assert( ! impl::is_allocator< std::list< unsigned > >::value, "" );

typedef boost::dynamic_bitset< unsigned, std::deque< unsigned > >               deque_bitset;
typedef boost::dynamic_bitset< unsigned, user::vector_with_swap< unsigned > >   user_bitset;

template< typename Bitset >
constexpr bool
has_nothrow_member_swap()
{
    return noexcept( std::declval< Bitset & >().swap( std::declval< Bitset & >() ) );
}

template< typename T >
constexpr bool
has_nothrow_free_swap()
{
    return noexcept( swap( std::declval< T & >(), std::declval< T & >() ) );
}

// swap() is noexcept if and only if swapping the underlying containers
// is, which, before C++17, the standard doesn't require for std::vector
// and std::deque.
static_assert( has_nothrow_member_swap< boost::dynamic_bitset<> >() == has_nothrow_free_swap< std::vector< unsigned long > >(), "" );
static_assert( has_nothrow_free_swap< boost::dynamic_bitset<> >() == has_nothrow_free_swap< std::vector< unsigned long > >(), "" );
static_assert( has_nothrow_member_swap< deque_bitset >() == has_nothrow_free_swap< std::deque< unsigned > >(), "" );
static_assert( ! has_nothrow_member_swap< user_bitset >(), "" );
static_assert( ! has_nothrow_free_swap< user_bitset >(), "" );

void
test_swap_uses_the_container_swap()
{
    user_bitset a( 10, 5ul );
    user_bitset b( 70, 1ul );

    user::swap_calls = 0;
    a.swap( b );
    BOOST_TEST_EQ( user::swap_calls, 1 );
    BOOST_TEST_EQ( a.size(), 70u );
    BOOST_TEST_EQ( a.to_ulong(), 1ul );
    BOOST_TEST_EQ( b.size(), 10u );
    BOOST_TEST_EQ( b.to_ulong(), 5ul );

    swap( a, b );
    BOOST_TEST_EQ( user::swap_calls, 2 );
    BOOST_TEST_EQ( a.size(), 10u );
    BOOST_TEST_EQ( a.to_ulong(), 5ul );
}

int
main()
{
    test_swap_uses_the_container_swap();
    test_container< std::deque< unsigned > >();
    test_container< indexable_list< unsigned > >();

    return boost::report_errors();
}
