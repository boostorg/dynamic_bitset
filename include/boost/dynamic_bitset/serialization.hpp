// -----------------------------------------------------------
//
// Copyright (c) 2015 Seth Heeren
// Copyright 2026 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

#ifndef BOOST_DYNAMIC_BITSET_SERIALIZATION_HPP
#define BOOST_DYNAMIC_BITSET_SERIALIZATION_HPP

// Boost.Serialization support for dynamic_bitset.
//
// This header deliberately includes no Boost.Serialization header, so
// that DynamicBitset doesn't depend on Boost.Serialization. To serialize
// a dynamic_bitset, you must also include the Boost.Serialization
// support for its underlying buffer (and link with Boost.Serialization):
// <boost/serialization/vector.hpp> for the default buffer (a
// std::vector), or the support for the container passed as
// AllocatorOrContainer. Boost.Serialization doesn't support every
// container (e.g., it doesn't support boost::container::small_vector):
// in that case, you have to write that support yourself.

#include "boost/core/nvp.hpp"
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#include "boost/throw_exception.hpp"
#include <stdexcept>

namespace boost {

template< typename Block, typename AllocatorOrContainer >
class dynamic_bitset< Block, AllocatorOrContainer >::serialize_impl
{
public:
    template< typename Ar >
    static void
    serialize( Ar & ar, dynamic_bitset & bs, unsigned )
    {
        do_serialize( ar, bs, detail::dynamic_bitset_impl::value_to_type< Ar::is_loading::value >() );
    }

private:
    // Saving.
    template< typename Ar >
    static void
    do_serialize( Ar & ar, dynamic_bitset & bs, detail::dynamic_bitset_impl::value_to_type< false > )
    {
        ar & boost::make_nvp( "m_num_bits", bs.m_num_bits )
            & boost::make_nvp( "m_bits", bs.m_bits );
    }

    // Loading. We load into local variables and modify bs only if they
    // make up a valid bitset, so that a truncated, corrupt or mismatched
    // archive (e.g. one saved with a different Block type) can't break
    // the invariant of bs.
    template< typename Ar >
    static void
    do_serialize( Ar & ar, dynamic_bitset & bs, detail::dynamic_bitset_impl::value_to_type< true > )
    {
        size_type   num_bits = 0;
        buffer_type bits( bs.get_allocator() );
        ar >> boost::make_nvp( "m_num_bits", num_bits )
            >> boost::make_nvp( "m_bits", bits );
        if ( ! is_consistent( num_bits, bits ) ) {
            BOOST_THROW_EXCEPTION( std::invalid_argument( "boost::dynamic_bitset: invalid bitset in archive" ) );
        }

        bs.m_bits.swap( bits );
        bs.m_num_bits = num_bits;

        // In case the archive tracks the buffer, tell it where the
        // loaded one is now.
        ar.reset_object_address( &bs.m_bits, &bits );
    }

    static bool
    is_consistent( size_type num_bits, const buffer_type & bits )
    {
        if ( bits.size() != calc_num_blocks( num_bits ) ) {
            return false;
        }
        const int extra_bits = bit_index( num_bits );
        return extra_bits == 0 || ( bits.back() >> extra_bits ) == 0;
    }
};

}

// ADL hook to Boost Serialization library
namespace boost {
namespace serialization {

template< typename Ar, typename Block, typename AllocatorOrContainer >
void
serialize( Ar & ar, dynamic_bitset< Block, AllocatorOrContainer > & bs, unsigned version )
{
    dynamic_bitset< Block, AllocatorOrContainer >::serialize_impl::serialize( ar, bs, version );
}

} // namespace serialization
} // namespace boost

#endif // include guard
