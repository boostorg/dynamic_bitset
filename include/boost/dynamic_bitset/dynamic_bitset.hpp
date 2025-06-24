// -----------------------------------------------------------
//
//   Copyright (c) 2001-2002 Chuck Allison and Jeremy Siek
//        Copyright (c) 2003-2006, 2008 Gennaro Prota
//             Copyright (c) 2014 Ahmed Charles
//
// Copyright (c) 2014 Glen Joseph Fernandes
// (glenjofe@gmail.com)
//
// Copyright (c) 2014 Riccardo Marcangelo
//             Copyright (c) 2018 Evgeny Shulgin
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

#ifndef BOOST_DYNAMIC_BITSET_DYNAMIC_BITSET_HPP
#define BOOST_DYNAMIC_BITSET_DYNAMIC_BITSET_HPP

#include "boost/assert.hpp"
#include "boost/core/bit.hpp"
#include "boost/core/no_exceptions_support.hpp"
#include "boost/dynamic_bitset/config.hpp"
#include "boost/dynamic_bitset/detail/dynamic_bitset.hpp"
#include "boost/dynamic_bitset/detail/lowest_bit.hpp"
#include "boost/dynamic_bitset_fwd.hpp"
#include "boost/functional/hash/hash.hpp"
#include "boost/limits.hpp"
#include "boost/move/move.hpp"
#include "boost/static_assert.hpp"
#include "boost/throw_exception.hpp"

#include <algorithm>
#include <climits>
#include <istream>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef BOOST_NO_STD_LOCALE
#  include <locale>
#endif

namespace boost {

template <typename Block, typename Allocator>
class dynamic_bitset
{
    BOOST_STATIC_ASSERT((bool)detail::dynamic_bitset_impl::allowed_block_type<Block>::value);
    typedef std::vector<Block, Allocator> buffer_type;

public:
    typedef Block block_type;
    typedef Allocator allocator_type;
    typedef std::size_t size_type;
    typedef typename buffer_type::size_type block_width_type;

    BOOST_STATIC_CONSTANT(block_width_type, bits_per_block = (std::numeric_limits<Block>::digits));
    BOOST_STATIC_CONSTANT(size_type, npos = static_cast<size_type>(-1));


    // A proxy class to simulate lvalues of bit type.
    //
    class reference
    {
        friend class dynamic_bitset<Block, Allocator>;


        // the one and only non-copy ctor
        reference(block_type & b, block_width_type pos);

        void operator&(); // left undefined

    public:

        // copy constructor: compiler generated

        operator bool() const;
        bool operator~() const;

        reference& flip();

        reference& operator=(bool x);
        reference& operator=(const reference& rhs);

        reference& operator|=(bool x);
        reference& operator&=(bool x);
        reference& operator^=(bool x);
        reference& operator-=(bool x);

     private:
        block_type & m_block;
        const block_type m_mask;

        void do_set();
        void do_reset();
        void do_flip();
        void do_assign(bool x);
    };

    typedef bool const_reference;

    // constructors, etc.
    dynamic_bitset();

    explicit
    dynamic_bitset(const Allocator& alloc);

    explicit
    dynamic_bitset(size_type num_bits, unsigned long value = 0,
               const Allocator& alloc = Allocator());


    // WARNING: you should avoid using this constructor.
    //
    //  A conversion from string is, in most cases, formatting,
    //  and should be performed by using operator>>.
    //
    // NOTE:
    //  Leave the parentheses around std::basic_string<CharT, Traits, Alloc>::npos.
    //  g++ 3.2 requires them and probably the standard will - see core issue 325
    // NOTE 2:
    //  split into two constructors because of bugs in MSVC 6.0sp5 with STLport

    template <typename CharT, typename Traits, typename Alloc>
    dynamic_bitset(const std::basic_string<CharT, Traits, Alloc>& s,
        typename std::basic_string<CharT, Traits, Alloc>::size_type pos,
        typename std::basic_string<CharT, Traits, Alloc>::size_type n,
        size_type num_bits = npos,
        const Allocator& alloc = Allocator());

    template <typename CharT, typename Traits, typename Alloc>
    explicit
    dynamic_bitset(const std::basic_string<CharT, Traits, Alloc>& s,
      typename std::basic_string<CharT, Traits, Alloc>::size_type pos = 0);

    // The first bit in *first is the least significant bit, and the
    // last bit in the block just before *last is the most significant bit.
    template <typename BlockInputIterator>
    dynamic_bitset(BlockInputIterator first, BlockInputIterator last,
                   const Allocator& alloc = Allocator());

    // copy constructor
    dynamic_bitset(const dynamic_bitset& b);

    ~dynamic_bitset();

    void swap(dynamic_bitset& b) BOOST_NOEXCEPT;
    dynamic_bitset& operator=(const dynamic_bitset& b);

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    dynamic_bitset(dynamic_bitset&& src);
    dynamic_bitset& operator=(dynamic_bitset&& src);
#endif // BOOST_NO_CXX11_RVALUE_REFERENCES

    allocator_type get_allocator() const;

    // size changing operations
    void resize(size_type num_bits, bool value = false);
    void clear();
    void push_back(bool bit);
    void pop_back();
    void append(Block block);

    template <typename BlockInputIterator>
    void append(BlockInputIterator first, BlockInputIterator last); // strong guarantee

    // bitset operations
    dynamic_bitset& operator&=(const dynamic_bitset& b);
    dynamic_bitset& operator|=(const dynamic_bitset& b);
    dynamic_bitset& operator^=(const dynamic_bitset& b);
    dynamic_bitset& operator-=(const dynamic_bitset& b);
    dynamic_bitset& operator<<=(size_type n);
    dynamic_bitset& operator>>=(size_type n);
    dynamic_bitset operator<<(size_type n) const;
    dynamic_bitset operator>>(size_type n) const;

    // basic bit operations
    dynamic_bitset& set(size_type n, size_type len, bool val /* = true */); // default would make it ambiguous
    dynamic_bitset& set(size_type n, bool val = true);
    dynamic_bitset& set();
    dynamic_bitset& reset(size_type n, size_type len);
    dynamic_bitset& reset(size_type n);
    dynamic_bitset& reset();
    dynamic_bitset& flip(size_type n, size_type len);
    dynamic_bitset& flip(size_type n);
    dynamic_bitset& flip();
    reference at(size_type n);
    bool at(size_type n) const;
    bool test(size_type n) const;
    bool test_set(size_type n, bool val = true);
    bool all() const;
    bool any() const;
    bool none() const;
    dynamic_bitset operator~() const;
    size_type count() const BOOST_NOEXCEPT;

    // subscript
    reference operator[](size_type pos);
    bool operator[](size_type pos) const;

    unsigned long to_ulong() const;

    size_type size() const BOOST_NOEXCEPT;
    size_type num_blocks() const BOOST_NOEXCEPT;
    size_type max_size() const BOOST_NOEXCEPT;
    bool empty() const BOOST_NOEXCEPT;
    size_type capacity() const BOOST_NOEXCEPT;
    void reserve(size_type num_bits);
    void shrink_to_fit();

    bool is_subset_of(const dynamic_bitset& a) const;
    bool is_proper_subset_of(const dynamic_bitset& a) const;
    bool intersects(const dynamic_bitset & a) const;

    // lookup
    size_type find_first() const;
    size_type find_first(size_type pos) const;
    size_type find_next(size_type pos) const;

    // lexicographical comparison
    template <typename B, typename A>
    friend bool operator==(const dynamic_bitset<B, A>& a,
                           const dynamic_bitset<B, A>& b);

    template <typename B, typename A>
    friend bool operator<(const dynamic_bitset<B, A>& a,
                          const dynamic_bitset<B, A>& b);

    template <typename B, typename A, typename BlockOutputIterator>
    friend void to_block_range(const dynamic_bitset<B, A>& b,
                               BlockOutputIterator result);

    template <typename BlockIterator, typename B, typename A>
    friend void from_block_range(BlockIterator first, BlockIterator last,
                                 dynamic_bitset<B, A>& result);


    template <typename CharT, typename Traits, typename B, typename A>
    friend std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is,
                                                         dynamic_bitset<B, A>& b);

    template <typename B, typename A, typename stringT>
    friend void to_string_helper(const dynamic_bitset<B, A> & b, stringT & s, bool dump_all);

    template <typename B, typename A>
    friend std::size_t hash_value(const dynamic_bitset<B, A>& a);


    // forward declaration for optional zero-copy serialization support
    class serialize_impl;
    friend class serialize_impl;

private:
    BOOST_STATIC_CONSTANT(block_width_type, ulong_width = std::numeric_limits<unsigned long>::digits);

    dynamic_bitset& range_operation(size_type pos, size_type len,
        Block (*partial_block_operation)(Block, size_type, size_type),
        Block (*full_block_operation)(Block));
    void m_zero_unused_bits();
    bool m_check_invariants() const;

    static bool m_not_empty(Block x);
    size_type m_do_find_from(size_type first_block) const;

    block_width_type count_extra_bits() const BOOST_NOEXCEPT;
    static size_type block_index(size_type pos) BOOST_NOEXCEPT;
    static block_width_type bit_index(size_type pos) BOOST_NOEXCEPT;
    static Block bit_mask(size_type pos) BOOST_NOEXCEPT;
    static Block bit_mask(size_type first, size_type last) BOOST_NOEXCEPT;
    static Block set_block_bits(Block block, size_type first,
        size_type last, bool val) BOOST_NOEXCEPT;

    // Functions for operations on ranges
    inline static Block set_block_partial(Block block, size_type first,
        size_type last) BOOST_NOEXCEPT;
    inline static Block set_block_full(Block) BOOST_NOEXCEPT;
    inline static Block reset_block_partial(Block block, size_type first,
        size_type last) BOOST_NOEXCEPT;
    inline static Block reset_block_full(Block) BOOST_NOEXCEPT;
    inline static Block flip_block_partial(Block block, size_type first,
        size_type last) BOOST_NOEXCEPT;
    inline static Block flip_block_full(Block block) BOOST_NOEXCEPT;

    template <typename T>
    void dispatch_init(T num_bits, unsigned long value,
                       detail::dynamic_bitset_impl::value_to_type<true>);

    template <typename T>
    void dispatch_init(T first, T last,
                       detail::dynamic_bitset_impl::value_to_type<false>);

    template <typename BlockIter>
    void init_from_block_range(BlockIter first, BlockIter last);

    template <typename CharT, typename Traits, typename Alloc>
    void init_from_string(const std::basic_string<CharT, Traits, Alloc>& s,
        typename std::basic_string<CharT, Traits, Alloc>::size_type pos,
        typename std::basic_string<CharT, Traits, Alloc>::size_type n,
        size_type num_bits);

    void init_from_unsigned_long(size_type num_bits,
                                 unsigned long value/*,
                                 const Allocator& alloc*/);

    template <typename BlockInputIterator>
    void m_append(BlockInputIterator first, BlockInputIterator last, std::input_iterator_tag);

    template <typename BlockInputIterator>
    void m_append(BlockInputIterator first, BlockInputIterator last, std::forward_iterator_tag);

    bool m_unchecked_test(size_type pos) const;
    static size_type calc_num_blocks(size_type num_bits);

    Block&        m_highest_block();
    const Block&  m_highest_block() const;

    buffer_type m_bits;
    size_type   m_num_bits;


    class bit_appender;
    friend class bit_appender;
    class bit_appender {
      // helper for stream >>
      // Supplies to the lack of an efficient append at the less
      // significant end: bits are actually appended "at left" but
      // rearranged in the destructor. From the perspective of
      // client code everything works *as if* dynamic_bitset<> had
      // an append_at_right() function (eventually throwing the same
      // exceptions as push_back) except that the function is in fact
      // called bit_appender::do_append().
      //
      dynamic_bitset & bs;
      size_type n;
      Block mask;
      Block * current;

      // not implemented
      bit_appender(const bit_appender &);
      bit_appender & operator=(const bit_appender &);

    public:
        bit_appender(dynamic_bitset & r);
        ~bit_appender();
        inline void do_append(bool value);
        size_type get_count() const;
    };
};

#if !defined BOOST_NO_INCLASS_MEMBER_INITIALIZATION

template <typename Block, typename Allocator>
const typename dynamic_bitset<Block, Allocator>::block_width_type
dynamic_bitset<Block, Allocator>::bits_per_block;

template <typename Block, typename Allocator>
const typename dynamic_bitset<Block, Allocator>::size_type
dynamic_bitset<Block, Allocator>::npos;

template <typename Block, typename Allocator>
const typename dynamic_bitset<Block, Allocator>::block_width_type
dynamic_bitset<Block, Allocator>::ulong_width;

#endif

// Global Functions:

// comparison
template <typename Block, typename Allocator>
bool operator!=(const dynamic_bitset<Block, Allocator>& a,
                const dynamic_bitset<Block, Allocator>& b);

template <typename Block, typename Allocator>
bool operator<=(const dynamic_bitset<Block, Allocator>& a,
                const dynamic_bitset<Block, Allocator>& b);

template <typename Block, typename Allocator>
bool operator>(const dynamic_bitset<Block, Allocator>& a,
               const dynamic_bitset<Block, Allocator>& b);

template <typename Block, typename Allocator>
bool operator>=(const dynamic_bitset<Block, Allocator>& a,
                const dynamic_bitset<Block, Allocator>& b);

// stream operators
template <typename CharT, typename Traits, typename Block, typename Allocator>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os,
           const dynamic_bitset<Block, Allocator>& b);

template <typename CharT, typename Traits, typename Block, typename Allocator>
std::basic_istream<CharT, Traits>&
operator>>(std::basic_istream<CharT, Traits>& is,
           dynamic_bitset<Block, Allocator>& b);

// bitset operations
template <typename Block, typename Allocator>
dynamic_bitset<Block, Allocator>
operator&(const dynamic_bitset<Block, Allocator>& b1,
          const dynamic_bitset<Block, Allocator>& b2);

template <typename Block, typename Allocator>
dynamic_bitset<Block, Allocator>
operator|(const dynamic_bitset<Block, Allocator>& b1,
          const dynamic_bitset<Block, Allocator>& b2);

template <typename Block, typename Allocator>
dynamic_bitset<Block, Allocator>
operator^(const dynamic_bitset<Block, Allocator>& b1,
          const dynamic_bitset<Block, Allocator>& b2);

template <typename Block, typename Allocator>
dynamic_bitset<Block, Allocator>
operator-(const dynamic_bitset<Block, Allocator>& b1,
          const dynamic_bitset<Block, Allocator>& b2);

// namespace scope swap
template<typename Block, typename Allocator>
void swap(dynamic_bitset<Block, Allocator>& b1,
          dynamic_bitset<Block, Allocator>& b2) BOOST_NOEXCEPT;


template <typename Block, typename Allocator, typename stringT>
void
to_string(const dynamic_bitset<Block, Allocator>& b, stringT & s);

template <typename Block, typename Allocator, typename BlockOutputIterator>
void
to_block_range(const dynamic_bitset<Block, Allocator>& b,
               BlockOutputIterator result);


template <typename BlockIterator, typename B, typename A>
inline void
from_block_range(BlockIterator first, BlockIterator last,
                 dynamic_bitset<B, A>& result);

}

#include "impl/dynamic_bitset.tpp"
#endif // include guard

