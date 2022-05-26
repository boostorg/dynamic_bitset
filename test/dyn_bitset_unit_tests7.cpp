// -----------------------------------------------------------
//              Copyright (c) 2001 Jeremy Siek
//           Copyright (c) 2003-2006 Gennaro Prota
//             Copyright (c) 2014 Ahmed Charles
//          Copyright (c) 2014 Riccardo Marcangelo
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

#include <assert.h>
#include "bitset_test.hpp"
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <boost/limits.hpp>
#include <boost/config.hpp>

template <typename Block>
void run_test_cases( BOOST_EXPLICIT_TEMPLATE_TYPE(Block) )
{
  // a bunch of typedefs which will be handy later on
  typedef boost::dynamic_bitset<Block> bitset_type;
  typedef bitset_test<bitset_type> Tests;
  // typedef typename bitset_type::size_type size_type; // unusable with Borland 5.5.1

  std::string long_string = get_long_string();

  //=====================================================================
  // Test intersects
  {
    bitset_type a(3); // empty
    bitset_type b(4);
    Tests::intersects(a, b, 0, 0, 3);
    Tests::intersects(a, b, 0, 1, 3);
  }
  {
    bitset_type a(7);
    bitset_type b(5, 8ul);
    Tests::intersects(a, b, 0, 0, 5);
  }
  {
    bitset_type a(8, 0ul);
    bitset_type b(15, 0ul);
    b[9] = 1;
    Tests::intersects(a, b, 0, 3, 8);
  }
  {
    bitset_type a(15, 0ul);
    bitset_type b(22, 0ul);
    a[14] = b[14] = 1;
    Tests::intersects(a, b, 0, 1, 14);
  }
  {
    bitset_type a(long_string), b(long_string);
    b[long_string.size()/2].flip();
    Tests::intersects(a, b, 0, 0, long_string.size());
  }
  {
    bitset_type a(long_string), b(~a);
    for(size_t i = 0; i < a.size(); i+= 5)
    {
        Tests::intersects(a, b, 0, 0, i);
        a[i].flip();
        Tests::intersects(a, b, 0, 0, i);
        a[i].flip();
    }
    
  }
  {
    bitset_type a(long_string), b(a);
    for(size_t i = 0; i < a.size(); i+= 5)
    {
        Tests::intersects(a, b, 0, 0, i);
        a[i].flip();
        Tests::intersects(a, b, 0, 0, i);
        a[i].flip();
    }
    
  }
  
  //=====================================================================
  // Test operator==
  {
    bitset_type a(3), b(4);
    Tests::operator_equal(a, b, 0, 0, 3);
    Tests::operator_equal(a, b, 0, 1, 3);
  }
  {
    bitset_type a(3), b;
    Tests::operator_equal(a, b, 0, 0, 0);
  }
  {
    bitset_type a, b(4);
    Tests::operator_equal(a, b, 0, 0, 0);
  }
  {
    bitset_type a(std::string("00")), b(std::string("0"));
    Tests::operator_equal(a, b, 0, 0, 1);
    Tests::operator_equal(a, b, 1, 0, 1);
  }
  {
    bitset_type a(std::string("11")), b(std::string("1"));
    Tests::operator_equal(a, b, 0, 0, 1);
    Tests::operator_equal(a, b, 1, 0, 1);
  }
  {
    bitset_type a(long_string), b(long_string);
    Tests::operator_equal(a, b, 0, 0, long_string.size());
    Tests::operator_equal(a, b, long_string.size()/2, long_string.size()/2, long_string.size() - (long_string.size()/2));
  }
  {
    bitset_type a(long_string), b(long_string);
    a[long_string.size()/2].flip();
    Tests::operator_equal(a, b, 0, 0, long_string.size());
    Tests::operator_equal(a, b, long_string.size()/2, long_string.size()/2, long_string.size()  - (long_string.size()/2));
  }
  {
    bitset_type a(long_string), b(long_string);
    b[long_string.size()/2].flip();
    Tests::operator_equal(a, b, 0, 0, long_string.size());
    Tests::operator_equal(a, b, long_string.size()/2, long_string.size()/2, long_string.size()  - (long_string.size()/2));
  }
  //=====================================================================
  // Test operator!=
  {
    bitset_type a(3), b(4);
    Tests::operator_not_equal(a, b, 0, 0, 3);
    Tests::operator_not_equal(a, b, 0, 1, 3);
  }
  {
    bitset_type a(std::string("00")), b(std::string("0"));
    Tests::operator_not_equal(a, b, 0, 0, 1);
    Tests::operator_not_equal(a, b, 1, 0, 1);
  }
  {
    bitset_type a(std::string("11")), b(std::string("1"));
    Tests::operator_not_equal(a, b, 0, 0, 1);
    Tests::operator_not_equal(a, b, 1, 0, 1);
  }
  {
    bitset_type a(long_string), b(long_string);
    Tests::operator_not_equal(a, b, 0, 0, long_string.size());
    Tests::operator_not_equal(a, b, long_string.size()/2, long_string.size()/2, long_string.size() - (long_string.size()/2));
  }
  {
    bitset_type a(long_string), b(long_string);
    a[long_string.size()/2].flip();
    Tests::operator_not_equal(a, b, 0, 0, long_string.size());
    Tests::operator_not_equal(a, b, long_string.size()/2, long_string.size()/2, long_string.size() - (long_string.size()/2));
  }
  {
    bitset_type a(long_string), b(long_string);
    b[long_string.size()/2].flip();
    Tests::operator_not_equal(a, b, 0, 0, long_string.size());
    Tests::operator_not_equal(a, b, long_string.size()/2, long_string.size()/2, long_string.size() - (long_string.size()/2));
  }
  //=====================================================================
  // Test a & b
  {
    bitset_type lhs(3), rhs(4);
    Tests::operator_and(lhs, rhs, 0, 0, 3);
    Tests::operator_and(lhs, rhs, 0, 1, 3);
  }
  {
    bitset_type lhs(std::string("11")), rhs(std::string("0"));
    Tests::operator_and(lhs, rhs, 0, 0, 1);
    Tests::operator_and(lhs, rhs, 1, 0, 1);
  }
  {
    bitset_type lhs(long_string.size(), 0), rhs(long_string);
    Tests::operator_and(lhs, rhs, 0, 0, long_string.size()/2);
    Tests::operator_and(lhs, rhs, 1, 2, long_string.size() - 2);
    Tests::operator_and(lhs, rhs, long_string.size()/2, 0, long_string.size() - (long_string.size()/2));
  }
  {
    bitset_type lhs(long_string.size(), 1), rhs(long_string);
    Tests::operator_and(lhs, rhs, 0, 0, long_string.size()/2);
    Tests::operator_and(lhs, rhs, 1, 2, long_string.size() - 2);
    Tests::operator_and(lhs, rhs, long_string.size()/2, 0, long_string.size() - (long_string.size()/2));
  }
  //=====================================================================
  // Test a | b
  {
    bitset_type lhs(3), rhs(4);
    Tests::operator_or(lhs, rhs, 0, 0, 3);
    Tests::operator_or(lhs, rhs, 0, 1, 3);
  }
  {
    bitset_type lhs(std::string("11")), rhs(std::string("0"));
    Tests::operator_or(lhs, rhs, 0, 0, 1);
    Tests::operator_or(lhs, rhs, 1, 0, 1);
  }
  {
    bitset_type lhs(long_string.size(), 0), rhs(long_string);
    Tests::operator_or(lhs, rhs, 0, 0, long_string.size()/2);
    Tests::operator_or(lhs, rhs, 1, 2, long_string.size() - 2);
    Tests::operator_or(lhs, rhs, long_string.size()/2, 0, long_string.size() - (long_string.size()/2));
  }
  {
    bitset_type lhs(long_string.size(), 1), rhs(long_string);
    Tests::operator_or(lhs, rhs, 0, 0, long_string.size()/2);
    Tests::operator_or(lhs, rhs, 1, 2, long_string.size() - 2);
    Tests::operator_or(lhs, rhs, long_string.size()/2, 0, long_string.size() - (long_string.size()/2));
  }
  //=====================================================================
  // Test a^b
  {
    bitset_type lhs(3), rhs(4);
    Tests::operator_xor(lhs, rhs, 0, 0, 3);
    Tests::operator_xor(lhs, rhs, 0, 1, 3);
  }
  {
    bitset_type lhs(std::string("11")), rhs(std::string("0"));
    Tests::operator_xor(lhs, rhs, 0, 0, 1);
    Tests::operator_xor(lhs, rhs, 1, 0, 1);
  }
  {
    bitset_type lhs(long_string.size(), 0), rhs(long_string);
    Tests::operator_xor(lhs, rhs, 0, 0, long_string.size()/2);
    Tests::operator_xor(lhs, rhs, 1, 2, long_string.size() - 2);
    Tests::operator_xor(lhs, rhs, long_string.size()/2, 0, long_string.size() - (long_string.size()/2));
  }
  {
    bitset_type lhs(long_string.size(), 1), rhs(long_string);
    Tests::operator_xor(lhs, rhs, 0, 0, long_string.size()/2);
    Tests::operator_xor(lhs, rhs, 1, 2, long_string.size() - 2);
    Tests::operator_xor(lhs, rhs, long_string.size()/2, 0, long_string.size() - (long_string.size()/2));
  }
  //=====================================================================
  // Test a-b
  {
    bitset_type lhs(3), rhs(4);
    Tests::operator_sub(lhs, rhs, 0, 0, 3);
    Tests::operator_sub(lhs, rhs, 0, 1, 3);
  }
  {
    bitset_type lhs(std::string("11")), rhs(std::string("0"));
    Tests::operator_sub(lhs, rhs, 0, 0, 1);
    Tests::operator_sub(lhs, rhs, 1, 0, 1);
  }
  {
    bitset_type lhs(long_string.size(), 0), rhs(long_string);
    Tests::operator_sub(lhs, rhs, 0, 0, long_string.size()/2);
    Tests::operator_sub(lhs, rhs, 1, 2, long_string.size() - 2);
    Tests::operator_sub(lhs, rhs, long_string.size()/2, 0, long_string.size() - (long_string.size()/2));
  }
  {
    bitset_type lhs(long_string.size(), 1), rhs(long_string);
    Tests::operator_sub(lhs, rhs, 0, 0, long_string.size()/2);
    Tests::operator_sub(lhs, rhs, 1, 2, long_string.size() - 2);
    Tests::operator_sub(lhs, rhs, long_string.size()/2, 0, long_string.size() - (long_string.size()/2));
  }
}


int
main()
{
  run_test_cases<unsigned char>();
  run_test_cases<unsigned short>();
  run_test_cases<unsigned int>();
  run_test_cases<unsigned long>();
# ifdef BOOST_HAS_LONG_LONG
  run_test_cases< ::boost::ulong_long_type>();
# endif

  return boost::report_errors();
}
