// Copyright 2026 Gennaro Prota
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// An example of growing a bitset with push_back(), of searching it
// with find_first() and find_next(), and of traversing it with
// iterators. Bit i of the bitset tells whether i is a prime number,
// and each new bit is computed by trial division by the primes found
// so far.
//
// Iterating from begin() to end() goes from the least significant bit
// (the first one pushed back) to the most significant one (the last
// one pushed back). The operator<< for dynamic_bitset prints the bits
// in the opposite order, like iterating from rbegin() to rend().
//
//  The output is:
//
//  Primes below 30:  2 3 5 7 11 13 17 19 23 29
//  Number of primes: 10
//  Forward:          001101010001010001010001000001
//  Backward:         100000100010100010100010101100
//  operator<<:       100000100010100010100010101100
// ---------------------------------------------------------------------

#include "boost/dynamic_bitset.hpp"
#include <algorithm>
#include <iostream>

namespace {

typedef boost::dynamic_bitset<> bitset_type;
typedef bitset_type::size_type  size_type;

// Tells whether n is prime, given a bitset whose bit i is set if and
// only if i is prime, for every i < n.
bool
is_prime( size_type n, const bitset_type & primes )
{
    if ( n < 2 ) {
        return false;
    }
    for ( size_type p = primes.find_first(); p != bitset_type::npos && p * p <= n; p = primes.find_next( p ) ) {
        if ( n % p == 0 ) {
            return false;
        }
    }
    return true;
}

// Returns a bitset of size limit whose bit i is set if and only if i
// is prime.
bitset_type
primes_below( size_type limit )
{
    bitset_type primes;
    for ( size_type n = 0; n < limit; ++n ) {
        primes.push_back( is_prime( n, primes ) );
    }
    return primes;
}

// Prints the positions of the set bits of b, from the lowest one up.
void
print_set_positions( const bitset_type & b )
{
    for ( size_type pos = b.find_first(); pos != bitset_type::npos; pos = b.find_next( pos ) ) {
        std::cout << ' ' << pos;
    }
}

// Prints the bits of b, from the least significant one up.
void
print_forward( const bitset_type & b )
{
    for ( bool bit : b ) {
        std::cout << bit;
    }
}

// Prints the bits of b, from the most significant one down.
void
print_backward( const bitset_type & b )
{
    for ( bitset_type::const_reverse_iterator it = b.rbegin(); it != b.rend(); ++it ) {
        std::cout << *it;
    }
}

}

int
main()
{
    const size_type   limit  = 30;
    const bitset_type primes = primes_below( limit );

    std::cout << "Primes below " << limit << ": ";
    print_set_positions( primes );

    // std::count() needs only input iterators, so it works with the
    // iterators of a bitset: this is the same as primes.count().
    std::cout << "\nNumber of primes: " << std::count( primes.begin(), primes.end(), true );

    std::cout << "\nForward:          ";
    print_forward( primes );
    std::cout << "\nBackward:         ";
    print_backward( primes );
    std::cout << "\noperator<<:       " << primes << "\n";

    return 0;
}
