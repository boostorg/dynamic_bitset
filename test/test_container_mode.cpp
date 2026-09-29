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
}

}

namespace impl = boost::detail::dynamic_bitset_impl;

// std::list has no operator[] and is not an allocator, so
// dynamic_bitset< unsigned, std::list< unsigned > > fails a
// static_assert.
static_assert( ! impl::is_container< std::list< unsigned >, unsigned >::value, "" );
static_assert( ! impl::is_allocator< std::list< unsigned > >::value, "" );

int
main()
{
    test_container< std::deque< unsigned > >();
    test_container< indexable_list< unsigned > >();

    return boost::report_errors();
}
