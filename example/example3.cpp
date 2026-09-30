// Copyright (c) 2001 Jeremy Siek
// Copyright (c) 2008 Gennaro Prota
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Reads a bitset from the file named on the command line or, if no file
// name is given, from the standard input.
//
// Sample run:
//
// mask     = 101010101010
// x.size() = 0
// Enter a bitset in binary: x = 100100010
//
// Input number:             100100010
// x.size() is now:          9
// As unsigned long:         290
// Mask (possibly resized):  010101010
// And with mask:            000100010
// Or with mask:             110101010
// Shifted left by 1:        001000100
// Shifted right by 1:       010010001

#include "boost/dynamic_bitset.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>

int
main( int argc, char * argv[] )
{
    std::ifstream file;
    if ( argc > 1 ) {
        file.open( argv[ 1 ] );
    }
    std::istream &          in = argc > 1 ? file : std::cin;

    boost::dynamic_bitset<> mask( 12, 2730ul );
    std::cout << "mask     = " << mask << std::endl;

    boost::dynamic_bitset<> x;
    std::cout << "x.size() = " << x.size() << std::endl;

    std::cout << "Enter a bitset in binary: x = " << std::flush;
    if ( in >> x ) {
        const std::size_t sz = x.size();
        std::cout << std::endl;
        std::cout << "Input number:             " << x << std::endl;
        std::cout << "x.size() is now:          " << sz << std::endl;

        bool          fits_in_ulong = true;
        unsigned long ul            = 0;
        try {
            ul = x.to_ulong();
        } catch ( std::overflow_error & ) {
            fits_in_ulong = false;
        }

        std::cout << "As unsigned long:         ";
        if ( fits_in_ulong ) {
            std::cout << ul;
        } else {
            std::cout << "(overflow exception)";
        }

        std::cout << std::endl;

        mask.resize( sz );

        std::cout << "Mask (possibly resized):  " << mask << std::endl;

        std::cout << "And with mask:            " << ( x & mask ) << std::endl;
        std::cout << "Or with mask:             " << ( x | mask ) << std::endl;
        std::cout << "Shifted left by 1:        " << ( x << 1 ) << std::endl;
        std::cout << "Shifted right by 1:       " << ( x >> 1 ) << std::endl;
    } else {
        std::cerr << "Couldn't read a bitset" << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
