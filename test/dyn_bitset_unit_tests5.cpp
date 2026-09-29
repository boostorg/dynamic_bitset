// -----------------------------------------------------------
//              Copyright (c) 2001 Jeremy Siek
//         Copyright (c) 2003-2006, 2025 Gennaro Prota
//
// Copyright (c) 2015 Seth Heeren
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

#include "bitset_test.hpp"
#include "boost/archive/binary_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"
#include "boost/archive/xml_iarchive.hpp"
#include "boost/archive/xml_oarchive.hpp"
#include "boost/dynamic_bitset/serialization.hpp"
#include "boost/serialization/vector.hpp"
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace {
template< typename Block >
struct SerializableType
{
    boost::dynamic_bitset< Block > x;

private:
    friend class boost::serialization::access;
    template< class Archive >
    void
    serialize( Archive & ar, unsigned int )
    {
        ar & BOOST_SERIALIZATION_NVP( x );
    }
};

template< typename Block, typename IArchive, typename OArchive >
void
test_serialization()
{
    SerializableType< Block > a;

    for ( int i = 0; i < 128; ++i )
        a.x.resize( 11 * i, i % 2 );

    std::stringstream ss;

    // test serialization
    {
        OArchive oa( ss );
        oa << BOOST_SERIALIZATION_NVP( a );
    }

    // test de-serialization
    {
        IArchive                  ia( ss );
        SerializableType< Block > b;
        ia >> BOOST_SERIALIZATION_NVP( b );

        BOOST_TEST( a.x == b.x );
    }
}

template< typename Block >
void
test_binary_archive()
{
    test_serialization< Block, boost::archive::binary_iarchive, boost::archive::binary_oarchive >();
}

template< typename Block >
void
test_xml_archive()
{
    test_serialization< Block, boost::archive::xml_iarchive, boost::archive::xml_oarchive >();
}

// Serialized like a dynamic_bitset< Block >, but holds arbitrary data,
// to make archives of invalid bitsets.
template< typename Block >
struct raw_bitset
{
    std::size_t          m_num_bits;
    std::vector< Block > m_bits;

    template< class Archive >
    void
    serialize( Archive & ar, unsigned int )
    {
        ar & BOOST_SERIALIZATION_NVP( m_num_bits ) & BOOST_SERIALIZATION_NVP( m_bits );
    }
};

// Checks that loading an invalid bitset throws std::invalid_argument and
// leaves the target unchanged.
template< typename Block, typename IArchive, typename OArchive >
void
test_invalid_archive( std::size_t num_bits, std::size_t num_blocks, Block block )
{
    const raw_bitset< Block > raw = { num_bits, std::vector< Block >( num_blocks, block ) };
    std::stringstream         ss;
    {
        OArchive oa( ss );
        oa << boost::serialization::make_nvp( "x", raw );
    }

    const boost::dynamic_bitset< Block > original( 10, 5ul );
    boost::dynamic_bitset< Block >       b( original );
    IArchive                             ia( ss );
    bool                                 thrown = false;
    try {
        ia >> boost::serialization::make_nvp( "x", b );
    } catch ( const std::invalid_argument & ) {
        thrown = true;
    }
    BOOST_TEST( thrown );
    BOOST_TEST( b == original );
}

template< typename Block, typename IArchive, typename OArchive >
void
test_invalid_archives()
{
    const std::size_t bits_per_block = boost::dynamic_bitset< Block >::bits_per_block;

    test_invalid_archive< Block, IArchive, OArchive >( bits_per_block + 1, 1, 1 ); // too few blocks
    test_invalid_archive< Block, IArchive, OArchive >( 3, 2, 1 );                  // too many blocks
    test_invalid_archive< Block, IArchive, OArchive >( 3, 1, 8 );                  // a bit set past size()
}
}

template< typename Block >
void
run_test_cases()
{
    test_binary_archive< Block >();
    test_xml_archive< Block >();
    test_invalid_archives< Block, boost::archive::binary_iarchive, boost::archive::binary_oarchive >();
    test_invalid_archives< Block, boost::archive::xml_iarchive, boost::archive::xml_oarchive >();
}

int
main()
{
    run_test_cases< unsigned char >();
    run_test_cases< unsigned short >();
    run_test_cases< unsigned int >();
    run_test_cases< unsigned long >();
    run_test_cases< unsigned long long >();

    return boost::report_errors();
}
