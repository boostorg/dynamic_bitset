// -----------------------------------------------------------
//
//   Copyright (c) 2001-2002 Chuck Allison and Jeremy Siek
//   Copyright (c) 2003-2006, 2008, 2025 Gennaro Prota
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

#include "boost/dynamic_bitset/config.hpp"
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

namespace boost {

namespace detail {
namespace dynamic_bitset_impl {

// A type is taken as a container of Block, to be used as the underlying
// buffer, if its value_type is Block and it has resize( n ) and
// operator[]( n ).
template< typename AllocatorOrContainer, typename Block >
class is_container
{
private:
    template< typename U, typename = typename std::enable_if< std::is_same< typename U::value_type, Block >::value >::type >
    static decltype( std::declval< U & >().resize( std::size_t{} ), std::declval< U & >()[ std::size_t{} ], std::true_type{} ) test( int );

    template< typename >
    static std::false_type test( ... );

public:
    static constexpr bool value = decltype( test< AllocatorOrContainer >( 0 ) )::value;
};

// Detects the allocate() and deallocate() members that every allocator
// has. Used only to reject, with a clear message, an AllocatorOrContainer
// which is neither an allocator nor a suitable container of Block (e.g. a
// std::list).
template< typename T >
class is_allocator
{
private:
    template< typename U >
    static decltype( std::declval< U & >().deallocate( std::declval< U & >().allocate( std::size_t{} ), std::size_t{} ), std::true_type{} ) test( int );

    template< typename >
    static std::false_type test( ... );

public:
    static constexpr bool value = decltype( test< T >( 0 ) )::value;
};

// Swaps with an unqualified call, with std::swap() visible, so that a
// swap() found by ADL (for instance, one provided for a user container)
// takes part in overload resolution.
namespace adl_swap_impl {

using std::swap;

template< typename T >
struct is_nothrow_swappable
{
    static constexpr bool value = noexcept( swap( std::declval< T & >(), std::declval< T & >() ) );
};

template< typename T >
BOOST_DYNAMIC_BITSET_CONSTEXPR20 void
adl_swap( T & a, T & b ) noexcept( is_nothrow_swappable< T >::value )
{
    swap( a, b );
}

} // adl_swap_impl

using adl_swap_impl::adl_swap;
using adl_swap_impl::is_nothrow_swappable;

template< typename AllocatorOrContainer, bool IsContainer >
class allocator_type_extractor_impl;

template< typename AllocatorOrContainer >
class allocator_type_extractor_impl< AllocatorOrContainer, false >
{
public:
    typedef AllocatorOrContainer type;
};

template< typename AllocatorOrContainer >
class allocator_type_extractor_impl< AllocatorOrContainer, true >
{
public:
    typedef typename AllocatorOrContainer::allocator_type type;
};

template< typename AllocatorOrContainer, typename Block >
class allocator_type_extractor
{
public:
    typedef typename allocator_type_extractor_impl<
        AllocatorOrContainer,
        is_container< AllocatorOrContainer, Block >::value >::type type;
};

template< typename T, int amount, int width /* = default */ >
struct shifter
{
    static BOOST_DYNAMIC_BITSET_CONSTEXPR20 void
    right_shift( T & v )
    {
        amount >= width ? ( v = 0 )
                        : ( v >>= BOOST_DYNAMIC_BITSET_WRAP_CONSTANT( amount ) );
    }
};

template< bool value >
struct value_to_type
{
};

// for static_asserts
template< typename T >
struct allowed_block_type
{
    enum
    {
        value = std::numeric_limits< T >::is_integer && ! std::numeric_limits< T >::is_signed
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
using is_numeric = std::is_integral< T >; // floating points intentionally excluded

// The characters that stand for a zero bit and a one bit in the strings
// that a dynamic_bitset is constructed from or converted to. Unlike the
// characters used by the stream operators, they don't depend on any
// locale. The primary template serves char and any character type that
// is not specialized below. The specializations use the literals of the
// corresponding type, which give the right digits even if the encoding
// of that type doesn't agree with the narrow one on their values.
template< typename CharT >
struct binary_digits
{
    static constexpr CharT
    zero() noexcept
    {
        return CharT( '0' );
    }

    static constexpr CharT
    one() noexcept
    {
        return CharT( '1' );
    }
};

template<>
struct binary_digits< wchar_t >
{
    static constexpr wchar_t
    zero() noexcept
    {
        return L'0';
    }

    static constexpr wchar_t
    one() noexcept
    {
        return L'1';
    }
};

#if defined( __cpp_char8_t ) && __cpp_char8_t >= 201811L
// We don't use u8'0' and u8'1' here, because u8 character literals only
// exist since C++17, whereas GCC and Clang also support char8_t in C++11
// and C++14 (with -fchar8_t). u8 string literals exist since C++11.
template<>
struct binary_digits< char8_t >
{
    static constexpr char8_t
    zero() noexcept
    {
        return u8"0"[ 0 ];
    }

    static constexpr char8_t
    one() noexcept
    {
        return u8"1"[ 0 ];
    }
};
#endif

template<>
struct binary_digits< char16_t >
{
    static constexpr char16_t
    zero() noexcept
    {
        return u'0';
    }

    static constexpr char16_t
    one() noexcept
    {
        return u'1';
    }
};

template<>
struct binary_digits< char32_t >
{
    static constexpr char32_t
    zero() noexcept
    {
        return U'0';
    }

    static constexpr char32_t
    one() noexcept
    {
        return U'1';
    }
};

} // dynamic_bitset_impl
} // namespace detail

} // namespace boost

#endif // include guard
