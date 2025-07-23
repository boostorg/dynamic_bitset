// -----------------------------------------------------------
//
//   Copyright (c) 2001-2002 Chuck Allison and Jeremy Siek
//   Copyright (c) 2003-2006, 2008 Gennaro Prota
//   Copyright (c) 2014 Glen Joseph Fernandes
//       (glenjofe@gmail.com)
//   Copyright (c) 2018 Evgeny Shulgin
//   Copyright (c) 2019 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

#ifndef BOOST_DETAIL_DYNAMIC_BITSET_HPP
#define BOOST_DETAIL_DYNAMIC_BITSET_HPP

#include "boost/config.hpp"
#include "boost/core/allocator_access.hpp"
#include <cstddef>
#include <memory>

namespace boost {

namespace detail {
namespace dynamic_bitset_impl {

template< typename T, int amount, int width /* = default */ >
struct shifter
{
    static void
    left_shift( T & v )
    {
        amount >= width ? ( v = 0 )
                        : ( v >>= BOOST_DYNAMIC_BITSET_WRAP_CONSTANT( amount ) );
    }
};

template< bool value >
struct value_to_type
{
};

// Some library implementations simply return a dummy
// value such as
//
//   size_type(-1) / sizeof(T)
//
// from vector<>::max_size. This tries to get more
// meaningful info.
//
template< typename T >
typename T::size_type
vector_max_size_workaround( const T & v )
    BOOST_NOEXCEPT
{
    typedef typename T::allocator_type                          allocator_type;

    const allocator_type &                                      alloc = v.get_allocator();

    typename boost::allocator_size_type< allocator_type >::type alloc_max =
        boost::allocator_max_size( alloc );

    const typename T::size_type container_max = v.max_size();
    return alloc_max < container_max ? alloc_max : container_max;
}

// for static_asserts
template< typename T >
struct allowed_block_type
{
    enum
    {
        value = T( -1 ) > 0 // ensure T has no sign
    };
};

template<>
struct allowed_block_type< bool >
{
    enum
    {
        value = false
    };
};

template< typename T >
struct is_numeric
{
    enum
    {
        value = false
    };
};

#define BOOST_dynamic_bitset_is_numeric( x ) \
    template<>                               \
    struct is_numeric< x >                   \
    {                                        \
        enum                                 \
        {                                    \
            value = true                     \
        };                                   \
    } /**/

BOOST_dynamic_bitset_is_numeric( bool );
BOOST_dynamic_bitset_is_numeric( char );

#if ! defined( BOOST_NO_INTRINSIC_WCHAR_T )
BOOST_dynamic_bitset_is_numeric( wchar_t );
#endif

BOOST_dynamic_bitset_is_numeric( signed char );
BOOST_dynamic_bitset_is_numeric( short int );
BOOST_dynamic_bitset_is_numeric( int );
BOOST_dynamic_bitset_is_numeric( long int );

BOOST_dynamic_bitset_is_numeric( unsigned char );
BOOST_dynamic_bitset_is_numeric( unsigned short );
BOOST_dynamic_bitset_is_numeric( unsigned int );
BOOST_dynamic_bitset_is_numeric( unsigned long );

#if defined( BOOST_HAS_LONG_LONG )
BOOST_dynamic_bitset_is_numeric( ::boost::long_long_type );
BOOST_dynamic_bitset_is_numeric( ::boost::ulong_long_type );
#endif

// intentionally omitted
// BOOST_dynamic_bitset_is_numeric(float);
// BOOST_dynamic_bitset_is_numeric(double);
// BOOST_dynamic_bitset_is_numeric(long double);

#undef BOOST_dynamic_bitset_is_numeric

} // dynamic_bitset_impl
} // namespace detail

} // namespace boost

#endif // include guard
