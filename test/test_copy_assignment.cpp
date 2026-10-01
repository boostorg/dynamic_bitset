// -----------------------------------------------------------
//                Copyright 2026 Gennaro Prota
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

// Tests of the copy assignment operator when the copy of the
// underlying buffer throws. Depending on the allocator and on the
// standard library implementation, std::vector can free the old
// blocks before allocating the new ones, and the bitset must be left
// in a consistent state anyway.

#include "boost/core/lightweight_test.hpp"
#include "boost/dynamic_bitset/dynamic_bitset.hpp"
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>

// An allocator whose instances compare equal only if they have the
// same id, and which propagates on copy assignment if and only if
// `Propagate` is std::true_type. While `*fail_flag` is true,
// allocations throw std::bad_alloc.
template< typename T, typename Propagate >
class failing_allocator
{
public:
    typedef T         value_type;
    typedef Propagate propagate_on_container_copy_assignment;

    explicit failing_allocator( int identifier, const bool * fail_flag = nullptr )
        : m_id( identifier ), m_fail_flag( fail_flag )
    {
    }

    template< typename U >
    failing_allocator( const failing_allocator< U, Propagate > & other )
        : m_id( other.id() ), m_fail_flag( other.fail_flag() )
    {
    }

    T *
    allocate( std::size_t n )
    {
        if ( m_fail_flag != nullptr && *m_fail_flag ) {
            throw std::bad_alloc();
        }
        return std::allocator< T >().allocate( n );
    }

    void
    deallocate( T * p, std::size_t n )
    {
        std::allocator< T >().deallocate( p, n );
    }

    int
    id() const
    {
        return m_id;
    }

    const bool *
    fail_flag() const
    {
        return m_fail_flag;
    }

private:
    int          m_id;
    const bool * m_fail_flag;
};

template< typename T, typename U, typename Propagate >
bool
operator==( const failing_allocator< T, Propagate > & a, const failing_allocator< U, Propagate > & b )
{
    return a.id() == b.id();
}

template< typename T, typename U, typename Propagate >
bool
operator!=( const failing_allocator< T, Propagate > & a, const failing_allocator< U, Propagate > & b )
{
    return ! ( a == b );
}

template< typename Bitset >
Bitset
make_full_bitset( typename Bitset::size_type size, const typename Bitset::allocator_type & alloc )
{
    Bitset b( size, 0ul, alloc );
    b.set();
    return b;
}

// Assigns src to dst while `fail` is true, and returns whether that
// threw std::bad_alloc.
template< typename Bitset >
bool
assignment_throws( Bitset & dst, const Bitset & src, bool & fail )
{
    bool threw = false;
    fail       = true;
    try {
        dst = src;
    } catch ( const std::bad_alloc & ) {
        threw = true;
    }
    fail = false;
    return threw;
}

// Checks that a failed assignment of src to dst leaves dst empty and
// usable, and src unchanged.
template< typename Bitset >
void
check_failed_assignment( Bitset & dst, const Bitset & src, bool & fail )
{
    const Bitset original_src( src );
    BOOST_TEST( assignment_throws( dst, src, fail ) );
    BOOST_TEST( src == original_src );
    BOOST_TEST( dst.size() == 0 );
    BOOST_TEST( dst.num_blocks() == 0 );

    // Use dst only if it is empty: otherwise, it might be inconsistent.
    if ( dst.size() == 0 && dst.num_blocks() == 0 ) {
        dst.push_back( true );
        BOOST_TEST( dst.size() == 1 );
        BOOST_TEST( dst.count() == 1 );

        dst = src;
        BOOST_TEST( dst == src );
    }
}

// With an allocator which propagates on copy assignment, and instances
// which compare unequal, std::vector frees the old blocks of the target
// (with its own allocator) before allocating the new ones (with the
// allocator of the source). Here, the latter allocation throws.
template< typename Block >
void
test_propagating_allocator()
{
    typedef failing_allocator< Block, std::true_type >     allocator_type;
    typedef boost::dynamic_bitset< Block, allocator_type > bitset_type;

    bool                                                   fail = false;
    const bitset_type                                      src  = make_full_bitset< bitset_type >( 100, allocator_type( 1, &fail ) );
    bitset_type                                            dst  = make_full_bitset< bitset_type >( 50, allocator_type( 2 ) );
    check_failed_assignment( dst, src, fail );
}

// With an allocator which doesn't propagate on copy assignment, the
// target allocates from its own allocator, if it has to grow (and
// some std::vector implementations free the old blocks first). Here,
// that allocation throws.
template< typename Block >
void
test_non_propagating_allocator()
{
    typedef failing_allocator< Block, std::false_type >    allocator_type;
    typedef boost::dynamic_bitset< Block, allocator_type > bitset_type;

    bool                                                   fail = false;
    const bitset_type                                      src  = make_full_bitset< bitset_type >( 1000, allocator_type( 1 ) );
    bitset_type                                            dst  = make_full_bitset< bitset_type >( 50, allocator_type( 2, &fail ) );
    check_failed_assignment( dst, src, fail );
}

template< typename Block >
void
run_tests()
{
    test_propagating_allocator< Block >();
    test_non_propagating_allocator< Block >();
}

int
main()
{
    run_tests< unsigned char >();
    run_tests< unsigned short >();
    run_tests< unsigned int >();
    run_tests< unsigned long >();
    run_tests< unsigned long long >();

    return boost::report_errors();
}
