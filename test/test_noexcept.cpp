// -----------------------------------------------------------
//                Copyright 2026 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

// Checks, at compile time, the exception specifications of the
// functions which have a wide contract and can't throw. Those which
// don't depend on the underlying container are noexcept, also in
// container mode; the default constructor and the constructor from an
// allocator are noexcept if and only if the corresponding constructor
// of the underlying container is. (The move operations and swap() are
// checked by test_move.cpp and test_container_mode.cpp.)

#include "boost/config.hpp"
#include "boost/dynamic_bitset.hpp"
#include <cstddef>
#include <deque>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

// An allocator whose default constructor isn't noexcept.
template< typename T >
class throwing_default_allocator
{
public:
    typedef T value_type;

    throwing_default_allocator()
    {
    }

    template< typename U >
    throwing_default_allocator( const throwing_default_allocator< U > & ) noexcept
    {
    }

    T *
    allocate( std::size_t n )
    {
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
operator==( const throwing_default_allocator< T > &, const throwing_default_allocator< U > & )
{
    return true;
}

template< typename T, typename U >
bool
operator!=( const throwing_default_allocator< T > &, const throwing_default_allocator< U > & )
{
    return false;
}

// A container whose constructor from an allocator isn't noexcept, like
// that of boost::container::small_vector.
template< typename T >
class throwing_allocator_ctor_vector
    : public std::vector< T >
{
public:
    throwing_allocator_ctor_vector() noexcept
    {
    }

    explicit throwing_allocator_ctor_vector( const std::allocator< T > & alloc )
        : std::vector< T >( alloc )
    {
    }
};

template< typename Bitset >
void
check_modifiers()
{
    typedef typename Bitset::size_type size_type;

    static_assert( noexcept( std::declval< Bitset & >().clear() ), "" );
    static_assert( noexcept( std::declval< Bitset & >().set() ), "" );
    static_assert( noexcept( std::declval< Bitset & >().reset() ), "" );
    static_assert( noexcept( std::declval< Bitset & >().flip() ), "" );
    static_assert( noexcept( std::declval< Bitset & >() <<= size_type() ), "" );
    static_assert( noexcept( std::declval< Bitset & >() >>= size_type() ), "" );
}

template< typename Bitset >
void
check_observers()
{
    static_assert( noexcept( std::declval< const Bitset & >().get_allocator() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().all() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().any() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().none() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().count() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().size() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().num_blocks() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().max_size() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().empty() ), "" );
    static_assert( noexcept( hash_value( std::declval< const Bitset & >() ) ), "" );
}

template< typename Bitset >
void
check_find_functions()
{
    typedef typename Bitset::size_type size_type;

    static_assert( noexcept( std::declval< const Bitset & >().find_first() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().find_first( size_type() ) ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().find_first_off() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().find_first_off( size_type() ) ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().find_next( size_type() ) ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().find_next_off( size_type() ) ), "" );
}

template< typename Bitset >
void
check_iterator_functions()
{
    static_assert( noexcept( std::declval< Bitset & >().begin() ), "" );
    static_assert( noexcept( std::declval< Bitset & >().end() ), "" );
    static_assert( noexcept( std::declval< Bitset & >().rbegin() ), "" );
    static_assert( noexcept( std::declval< Bitset & >().rend() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().begin() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().end() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().rbegin() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().rend() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().cbegin() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().cend() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().crbegin() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >().crend() ), "" );
}

template< typename Bitset >
void
check_comparisons()
{
    static_assert( noexcept( std::declval< const Bitset & >() == std::declval< const Bitset & >() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >() != std::declval< const Bitset & >() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >() < std::declval< const Bitset & >() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >() <= std::declval< const Bitset & >() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >() > std::declval< const Bitset & >() ), "" );
    static_assert( noexcept( std::declval< const Bitset & >() >= std::declval< const Bitset & >() ), "" );
}

template< typename Bitset >
void
check_constructors()
{
    typedef typename Bitset::buffer_type    buffer_type;
    typedef typename Bitset::allocator_type allocator_type;

    static_assert( std::is_nothrow_default_constructible< Bitset >::value == std::is_nothrow_default_constructible< buffer_type >::value, "" );
    static_assert( std::is_nothrow_constructible< Bitset, const allocator_type & >::value == std::is_nothrow_constructible< buffer_type, const allocator_type & >::value, "" );
}

template< typename Bitset >
void
check_all()
{
    check_modifiers< Bitset >();
    check_observers< Bitset >();
    check_find_functions< Bitset >();
    check_iterator_functions< Bitset >();
    check_comparisons< Bitset >();
    check_constructors< Bitset >();
}

template< typename Block >
void
check_default_container()
{
    typedef boost::dynamic_bitset< Block > bitset_type;

    check_all< bitset_type >();

    // Since C++17, the standard requires these constructors of
    // std::vector< Block > to be noexcept.
#if BOOST_CXX_VERSION >= 201703L
    static_assert( std::is_nothrow_default_constructible< bitset_type >::value, "" );
    static_assert( std::is_nothrow_constructible< bitset_type, const std::allocator< Block > & >::value, "" );
#endif
}

typedef boost::dynamic_bitset< unsigned, throwing_default_allocator< unsigned > >     throwing_default_bitset;
typedef boost::dynamic_bitset< unsigned, throwing_allocator_ctor_vector< unsigned > > throwing_allocator_ctor_bitset;

static_assert( ! std::is_nothrow_default_constructible< throwing_default_bitset >::value, "" );
static_assert( std::is_nothrow_default_constructible< throwing_allocator_ctor_bitset >::value, "" );
static_assert( ! std::is_nothrow_constructible< throwing_allocator_ctor_bitset, const std::allocator< unsigned > & >::value, "" );

}

int
main()
{
    check_default_container< unsigned char >();
    check_default_container< unsigned short >();
    check_default_container< unsigned int >();
    check_default_container< unsigned long >();
    check_default_container< unsigned long long >();

    check_all< boost::dynamic_bitset< unsigned, std::deque< unsigned > > >();
    check_all< throwing_default_bitset >();
    check_all< throwing_allocator_ctor_bitset >();

    return 0;
}
