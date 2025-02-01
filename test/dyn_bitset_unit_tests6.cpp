// -----------------------------------------------------------
//              Copyright (c) 2001 Jeremy Siek
//           Copyright (c) 2003-2006 Gennaro Prota
//             Copyright (c) 2014 Ahmed Charles
//             Copyright (c) 2018 Evgeny Shulgin
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

#include "bitset_test.hpp"
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <boost/config.hpp>


template <typename Block>
void run_test_cases( BOOST_EXPLICIT_TEMPLATE_TYPE(Block) )
{
  typedef boost::dynamic_bitset<Block> bitset_type;
  typedef bitset_test< bitset_type > Tests;
  //const int bits_per_block = bitset_type::bits_per_block;

  std::string long_string = get_long_string();

  //=====================================================================
  // Test operator&=
  {
    boost::dynamic_bitset<Block> lhs(3), rhs(4);
    Tests::and_assignment(lhs, rhs, 0, 0, 3);
    Tests::and_assignment(lhs, rhs, 0, 1, 3);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("11")), rhs(std::string("0"));
    Tests::and_assignment(lhs, rhs, 0, 0, 1);
    Tests::and_assignment(lhs, rhs, 1, 0, 1);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 0), rhs(long_string);
    Tests::and_assignment(lhs, rhs, 0, 0, long_string.size());
    Tests::and_assignment(lhs, rhs, 1, 2, long_string.size() - 2);
    Tests::and_assignment(lhs, rhs, long_string.size()/2, long_string.size()/2, long_string.size() - (long_string.size()/2));
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 1), rhs(long_string);
    Tests::and_assignment(lhs, rhs, 0, 0, long_string.size());
    Tests::and_assignment(lhs, rhs, 1, 2, long_string.size() - 2);
    Tests::and_assignment(lhs, rhs, long_string.size()/2, long_string.size()/2, long_string.size() - (long_string.size()/2));
  }
  //=====================================================================
  // Test operator |=
  {
    boost::dynamic_bitset<Block> lhs(3), rhs(4);
    Tests::or_assignment(lhs, rhs, 0, 0, 3);
    Tests::or_assignment(lhs, rhs, 0, 1, 3);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("11")), rhs(std::string("0"));
    Tests::or_assignment(lhs, rhs, 0, 0, 1);
    Tests::or_assignment(lhs, rhs, 1, 0, 1);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 0), rhs(long_string);
    Tests::or_assignment(lhs, rhs, 0, 0, long_string.size());
    Tests::or_assignment(lhs, rhs, 1, 2, long_string.size() - 2);
    Tests::or_assignment(lhs, rhs, long_string.size()/2, long_string.size()/2, long_string.size() - (long_string.size()/2));
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string.size(), 1), rhs(long_string);
    Tests::or_assignment(lhs, rhs, 0, 0, long_string.size());
    Tests::or_assignment(lhs, rhs, 1, 2, long_string.size() - 2);
    Tests::or_assignment(lhs, rhs, long_string.size()/2, long_string.size()/2, long_string.size() - (long_string.size()/2));
  }
  //=====================================================================
  // Test operator^=
  {
    boost::dynamic_bitset<Block> lhs(3), rhs(4);
    Tests::xor_assignment(lhs, rhs, 0, 0, 3);
    Tests::xor_assignment(lhs, rhs, 0, 1, 3);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("11")), rhs(std::string("0"));
    Tests::xor_assignment(lhs, rhs, 0, 0, 1);
    Tests::xor_assignment(lhs, rhs, 1, 0, 1);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("00")), rhs(std::string("1"));
    Tests::xor_assignment(lhs, rhs, 0, 0, 1);
    Tests::xor_assignment(lhs, rhs, 1, 0, 1);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string), rhs(long_string);
    Tests::xor_assignment(lhs, rhs, 0, 0, long_string.size());
    Tests::xor_assignment(lhs, rhs, 1, 2, long_string.size() - 2);
    Tests::xor_assignment(lhs, rhs, long_string.size()/2, long_string.size()/2, long_string.size() - (long_string.size()/2));
  }
  //=====================================================================
  // Test operator-=
  {
    boost::dynamic_bitset<Block> lhs(3), rhs(4);
    Tests::sub_assignment(lhs, rhs, 0, 0, 3);
    Tests::sub_assignment(lhs, rhs, 0, 1, 3);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("11")), rhs(std::string("0"));
    Tests::sub_assignment(lhs, rhs, 0, 0, 1);
    Tests::sub_assignment(lhs, rhs, 1, 0, 1);
  }
  {
    boost::dynamic_bitset<Block> lhs(std::string("00")), rhs(std::string("1"));
    Tests::sub_assignment(lhs, rhs, 0, 0, 1);
    Tests::sub_assignment(lhs, rhs, 1, 0, 1);
  }
  {
    boost::dynamic_bitset<Block> lhs(long_string), rhs(long_string);
    Tests::sub_assignment(lhs, rhs, 0, 0, long_string.size());
    Tests::sub_assignment(lhs, rhs, 1, 2, long_string.size() - 2);
    Tests::sub_assignment(lhs, rhs, long_string.size()/2, long_string.size()/2, long_string.size() - (long_string.size()/2));
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
