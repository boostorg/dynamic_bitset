#ifndef BOOST_DYNAMIC_BITSET_ITER_HPP
#define BOOST_DYNAMIC_BITSET_ITER_HPP

#include "boost/throw_exception.hpp"
#include <cstddef>
#include <iterator>
#include <stdexcept>

namespace boost {

template <typename Container_t> class dbs_iterator {
private:
  typedef typename Container_t::size_type size_type;

  Container_t& m_bs;
  size_type m_len;
  size_type m_pos;

  void swap(dbs_iterator& other);

public:
  typedef void pointer;
  typedef typename Container_t::reference reference;
  typedef typename Container_t::const_reference value_type;
  typedef std::ptrdiff_t difference_type;
  typedef std::random_access_iterator_tag iterator_category;

  dbs_iterator(Container_t& dbs, size_type pos = 0)
      : m_bs(dbs), m_len(dbs.size()), m_pos(pos) {}

  dbs_iterator(const dbs_iterator& other)
      : m_bs(other.m_bs), m_len(other.m_len), m_pos(other.m_pos) {}

  dbs_iterator& operator=(const dbs_iterator& rhs);

  dbs_iterator& operator++();
  dbs_iterator operator++(int);

  dbs_iterator& operator--();
  dbs_iterator operator--(int);

  dbs_iterator operator+(difference_type n) const;
  dbs_iterator& operator+=(difference_type n);

  dbs_iterator operator-(difference_type n) const;
  dbs_iterator& operator-=(difference_type n);

  reference operator*() const;

#if !defined BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS
  template <typename C>
  friend bool operator==(const dbs_iterator<C>& lhs,
                         const dbs_iterator<C>& rhs);

  template <typename C>
  friend bool operator!=(const dbs_iterator<C>& lhs,
                         const dbs_iterator<C>& rhs);

  template <typename C>
  friend bool operator<(const dbs_iterator<C>& lhs,
                        const dbs_iterator<C>& rhs);

  template <typename C>
  friend bool operator>(const dbs_iterator<C>& lhs,
                        const dbs_iterator<C>& rhs);

  template <typename C>
  friend bool operator<=(const dbs_iterator<C>& lhs,
                         const dbs_iterator<C>& rhs);

  template <typename C>
  friend bool operator>=(const dbs_iterator<C>& lhs,
                        const dbs_iterator<C>& rhs);

  template <typename C>
  friend dbs_iterator<C>
  operator+(const typename dbs_iterator<C>::difference_type n,
            const dbs_iterator<C>& rhs);

  template <typename C>
  friend typename dbs_iterator<C>::difference_type
  operator-(const dbs_iterator<C>& lhs,
            const dbs_iterator<C>& rhs);

#endif
};


template <typename Container_t> class const_dbs_iterator : public dbs_iterator<Container_t> {
private:
  typedef typename Container_t::size_type size_type;

public:
  typedef void pointer;
  typedef typename Container_t::reference reference;
  typedef typename Container_t::const_reference value_type;
  typedef std::ptrdiff_t difference_type;
  typedef std::random_access_iterator_tag iterator_category;

  const_dbs_iterator(Container_t& dbs, size_type pos = 0)
      : dbs_iterator<Container_t>(dbs, pos) {}

  const_dbs_iterator(const dbs_iterator<Container_t>& other)
      : dbs_iterator<Container_t>(other) {}

  value_type operator*() const;
};

template <typename Container_t>
typename const_dbs_iterator<Container_t>::value_type
const_dbs_iterator<Container_t>::operator*() const {
  typename const_dbs_iterator<Container_t>::value_type var =
      dbs_iterator<Container_t>::operator*();
  return var;
}


template <typename Container_t>
void dbs_iterator<Container_t>::swap(dbs_iterator<Container_t>& other)
{
  std::swap(m_bs, other.m_bs);
  std::swap(m_len, other.m_len);
  std::swap(m_pos, other.m_pos);
}

template <typename Container_t>
dbs_iterator<Container_t>&
dbs_iterator<Container_t>::operator=(const dbs_iterator<Container_t>& rhs) 
{
  dbs_iterator<Container_t> temp(rhs);
  swap(temp);
  return *this;
}

template <typename Container_t>
dbs_iterator<Container_t>& dbs_iterator<Container_t>::operator++() 
{
  m_pos++;
  return *this;
}

template <typename Container_t>
dbs_iterator<Container_t> dbs_iterator<Container_t>::operator++(int) 
{
  dbs_iterator<Container_t> temp(*this);
  operator++();
  return temp;
}

template <typename Container_t>
dbs_iterator<Container_t>& dbs_iterator<Container_t>::operator--() 
{
  m_pos--;
  return *this;
}

template <typename Container_t>
dbs_iterator<Container_t> dbs_iterator<Container_t>::operator--(int)
{
  dbs_iterator<Container_t> temp(*this);
  operator--();
  return temp;
}

template <typename Container_t>
dbs_iterator<Container_t> dbs_iterator<Container_t>::operator+(
    typename dbs_iterator<Container_t>::difference_type n) const
{
  dbs_iterator<Container_t> temp(*this);
  temp += n;
  return temp;
}

template <typename Container_t>
dbs_iterator<Container_t>& dbs_iterator<Container_t>::operator+=(
    typename dbs_iterator<Container_t>::difference_type n) 
{
  m_pos += n;
  return *this;
}

template <typename Container_t>
dbs_iterator<Container_t> dbs_iterator<Container_t>::operator-(
    typename dbs_iterator<Container_t>::difference_type n) const
{
  dbs_iterator<Container_t> temp(*this);
  temp -= n;
  return temp;
}

template <typename Container_t>
dbs_iterator<Container_t>& dbs_iterator<Container_t>::operator-=(
    typename dbs_iterator<Container_t>::difference_type n)
{
  *this += -n;
  return *this;
}

template <typename Container_t>
typename Container_t::reference dbs_iterator<Container_t>::operator*() const
{
  if (m_pos >= m_len)
  {
    BOOST_THROW_EXCEPTION(std::out_of_range("boost::dbs_iterator::operator*() out_of_range"));
  }
  return m_bs[m_pos];
}

template <typename Container_t>
bool operator==(const dbs_iterator<Container_t>& lhs,
                const dbs_iterator<Container_t>& rhs) 
{
  return (lhs.m_pos == rhs.m_pos) && (lhs.m_len == rhs.m_len) &&
         (&(lhs.m_bs) == &(rhs.m_bs));
}

template <typename Container_t>
bool operator!=(const dbs_iterator<Container_t>& lhs,
                const dbs_iterator<Container_t>& rhs)
{
  return !(lhs == rhs);
}

template <typename Container_t>
bool operator<(const dbs_iterator<Container_t>& lhs,
               const dbs_iterator<Container_t>& rhs)
{
  if (&(lhs.m_bs) != &(rhs.m_bs)) {
    BOOST_THROW_EXCEPTION(std::logic_error("boost::operator<() not from same dynamic_bitset bitset"));
  }
  return lhs.m_pos < rhs.m_pos;
}

template <typename Container_t>
bool operator>(const dbs_iterator<Container_t>& lhs,
               const dbs_iterator<Container_t>& rhs)
{
  return rhs < lhs;
}

template <typename Container_t>
bool operator<=(const dbs_iterator<Container_t>& lhs,
               const dbs_iterator<Container_t>& rhs)
{
  return !(lhs > rhs);
}

template <typename Container_t>
bool operator>=(const dbs_iterator<Container_t>& lhs,
               const dbs_iterator<Container_t>& rhs)
{
  return !(lhs < rhs);
}

template <typename Container_t>
dbs_iterator<Container_t>
operator+(typename dbs_iterator<Container_t>::difference_type n,
          const dbs_iterator<Container_t>& rhs)
{
  return rhs + n;
}

template <typename Container_t>
typename dbs_iterator<Container_t>::difference_type
operator-(const dbs_iterator<Container_t>& lhs,
          const dbs_iterator<Container_t>& rhs)
{
    if (&(lhs.m_bs) != &(rhs.m_bs)) {
    BOOST_THROW_EXCEPTION(std::logic_error("boost::operator-() not from same dynamic_bitset bitset"));
  }
  return lhs.m_pos - rhs.m_pos;
}

} // namespace boost
#endif // include guard
