//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright David Abrahams, Vicente Botet, Ion Gaztanaga 2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////
//
// Parts of this file come from Adobe's Move library:
//
// Copyright 2005-2007 Adobe Systems Incorporated
// Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
// or a copy at http://stlab.adobe.com/licenses.html)
//
//////////////////////////////////////////////////////////////////////////////

//! \file

#ifndef BOOST_INTERPROCESS_MOVE_HPP
#define BOOST_INTERPROCESS_MOVE_HPP

#include <boost/config.hpp>
#include <algorithm> //copy, copy_backward
#include <memory>    //uninitialized_copy
#include <iterator>  //std::iterator
#include <boost/mpl/if.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/has_trivial_destructor.hpp>

namespace boost {
namespace interprocess {
namespace move_detail {

template <class T>
struct identity
{
   typedef T type;
};

template <class T, class U>
class is_convertible
{
   typedef char true_t;
   class false_t { char dummy[2]; };
   static true_t dispatch(U);
   static false_t dispatch(...);
   static T trigger();
   public:
   enum { value = sizeof(dispatch(trigger())) == sizeof(true_t) };
};

}  //namespace move_detail {
}  //namespace interprocess {
}  //namespace boost {

#if !defined(BOOST_HAS_RVALUE_REFS) && !defined(BOOST_MOVE_DOXYGEN_INVOKED)

namespace boost {
namespace interprocess {

//////////////////////////////////////////////////////////////////////////////
//
//                            struct rv
//
//////////////////////////////////////////////////////////////////////////////
template <class T>
class rv : public T
{
   rv();
   ~rv();
   rv(rv const&);
   void operator=(rv const&);

   public:
   //T &get() { return *this; }
};

//////////////////////////////////////////////////////////////////////////////
//
//                            move_detail::is_rv
//
//////////////////////////////////////////////////////////////////////////////

namespace move_detail {

template <class T>
struct is_rv
{
   static const bool value = false;
};

template <class T>
struct is_rv< rv<T> >
{
   static const bool value = true;
};

}  //namespace move_detail {

//////////////////////////////////////////////////////////////////////////////
//
//                               is_movable
//
//////////////////////////////////////////////////////////////////////////////
template<class T>
class is_movable
{
   public:
   static const bool value = move_detail::is_convertible<T, rv<T>&>::value;
};

template<class T>
class is_movable< rv<T> >
{
   public:
   static const bool value = false;
};

//////////////////////////////////////////////////////////////////////////////
//
//                            move()
//
//////////////////////////////////////////////////////////////////////////////
template <class T>
typename boost::disable_if<is_movable<T>, T&>::type move(T& x)
{
   return x;
}

template <class T>
typename enable_if<is_movable<T>, rv<T>&>::type move(T& x)
{
   return reinterpret_cast<rv<T>& >(x);
}

template <class T>
typename enable_if<is_movable<T>, rv<T>&>::type move(const rv<T>& x)
{
   return const_cast<rv<T>& >(x);
}

//////////////////////////////////////////////////////////////////////////////
//
//                            forward()
//
//////////////////////////////////////////////////////////////////////////////

template <class T>
typename enable_if<boost::interprocess::move_detail::is_rv<T>, T &>::type
   forward(const typename move_detail::identity<T>::type &x)
{
   return const_cast<T&>(x);
}

/*
template <class T>
typename enable_if<boost::interprocess::move_detail::is_rv<T>, T &>::type
forward(typename move_detail::identity<T>::type &x)
{
   return x;
}

template <class T>
typename disable_if<boost::interprocess::move_detail::is_rv<T>, T &>::type
   forward(typename move_detail::identity<T>::type &x)
{
   return x;
}
*/
template <class T>
typename disable_if<boost::interprocess::move_detail::is_rv<T>, const T &>::type
   forward(const typename move_detail::identity<T>::type &x)
{
   return x;
}

//////////////////////////////////////////////////////////////////////////////
//
//                         BOOST_INTERPROCESS_ENABLE_MOVE_EMULATION
//
//////////////////////////////////////////////////////////////////////////////
#define BOOST_INTERPROCESS_ENABLE_MOVE_EMULATION(TYPE)\
   operator boost::interprocess::rv<TYPE>&() \
   {  return reinterpret_cast<boost::interprocess::rv<TYPE>& >(*this);   }\
//


#define BOOST_INTERPROCESS_RV_REF(TYPE)\
   boost::interprocess::rv< TYPE >& \
//

#define BOOST_INTERPROCESS_RV_REF_2_TEMPL_ARGS(TYPE, ARG1, ARG2)\
   boost::interprocess::rv< TYPE<ARG1, ARG2> >& \
//

#define BOOST_INTERPROCESS_RV_REF_3_TEMPL_ARGS(TYPE, ARG1, ARG2, ARG3)\
   boost::interprocess::rv< TYPE<ARG1, ARG2, ARG3> >& \
//

#define BOOST_INTERPROCESS_FWD_REF(TYPE)\
   const TYPE & \
//
}  //namespace interprocess {
}  //namespace boost

#else    //BOOST_HAS_RVALUE_REFS

#include <boost/type_traits/remove_reference.hpp>

namespace boost {
namespace interprocess {

//////////////////////////////////////////////////////////////////////////////
//
//                               is_movable
//
//////////////////////////////////////////////////////////////////////////////

//! For compilers with rvalue references, this traits class returns true
//! if T && is convertible to T.
//!
//! For other compilers returns true if T is convertible to <i>boost::interprocess::rv<T>&</i>
template<class T>
class is_movable
{
   public:
   static const bool value = move_detail::is_convertible<T&&, T>::value;
};

//////////////////////////////////////////////////////////////////////////////
//
//                                  move
//
//////////////////////////////////////////////////////////////////////////////


#if defined(BOOST_MOVE_DOXYGEN_INVOKED)
//! This function provides a way to convert a reference into a rvalue reference
//! in compilers with rvalue reference. For other compilers converts T & into
//! <i>boost::interprocess::rv<T> &</i> so that move emulation is activated.
template <class T> inline 
rvalue_reference move (input_reference);
#else
template <class T> inline 
typename remove_reference<T>::type&& move(T&& t)
{  return t;   }
#endif

//////////////////////////////////////////////////////////////////////////////
//
//                                  forward
//
//////////////////////////////////////////////////////////////////////////////


#if defined(BOOST_MOVE_DOXYGEN_INVOKED)
//! This function provides limited form of forwarding that is usually enough for
//! in-place construction and avoids the exponential overloading necessary for
//! perfect forwarding in C++03.
//!
//! For compilers with rvalue references this function provides perfect forwarding.
//!
//! Otherwise:
//! * If input_reference binds to const boost::interprocess::rv<T> & then it output_reference is
//!   boost::rev<T> &
//!
//! * Else, input_reference is equal to output_reference is equal to input_reference.
template <class T> inline output_reference forward(input_reference);
#else
template <class T> inline
T&& forward (typename move_detail::identity<T>::type&& t)
{  return t;   }
#endif

//////////////////////////////////////////////////////////////////////////////
//
//                         BOOST_INTERPROCESS_ENABLE_MOVE_EMULATION
//
//////////////////////////////////////////////////////////////////////////////

//! This macro expands to nothing for compilers with rvalue references.
//! Otherwise expands to:
//! \code
//! operator boost::interprocess::rv<TYPE>&()
//! {  return static_cast<boost::interprocess::rv<TYPE>& >(*this);   }
//! \endcode
#define BOOST_INTERPROCESS_ENABLE_MOVE_EMULATION(TYPE)\
//

#define BOOST_INTERPROCESS_RV_REF_2_TEMPL_ARGS(TYPE, ARG1, ARG2)\
   TYPE<ARG1, ARG2> && \
//

#define BOOST_INTERPROCESS_RV_REF_3_TEMPL_ARGS(TYPE, ARG1, ARG2, ARG3)\
   TYPE<ARG1, ARG2, ARG3> && \
//

//! This macro expands to <i>T&&</i> for compilers with rvalue references.
//! Otherwise expands to <i>boost::interprocess::rv<T> &</i>.
#define BOOST_INTERPROCESS_RV_REF(TYPE)\
   TYPE && \
//

//! This macro expands to <i>T&&</i> for compilers with rvalue references.
//! Otherwise expands to <i>const T &</i>.
#define BOOST_INTERPROCESS_FWD_REF(TYPE)\
   TYPE && \
//

}  //namespace interprocess {
}  //namespace boost {

#endif   //BOOST_HAS_RVALUE_REFS

namespace boost {
namespace interprocess {

//////////////////////////////////////////////////////////////////////////////
//
//                            move_iterator
//
//////////////////////////////////////////////////////////////////////////////

//! Class template move_iterator is an iterator adaptor with the same behavior
//! as the underlying iterator except that its dereference operator implicitly
//! converts the value returned by the underlying iterator's dereference operator
//! to an rvalue reference. Some generic algorithms can be called with move
//! iterators to replace copying with moving.
template <class It>
class move_iterator
{
   public:
   typedef It                                                              iterator_type;
   typedef typename std::iterator_traits<iterator_type>::value_type        value_type;
   #if defined(BOOST_HAS_RVALUE_REFS) || defined(BOOST_MOVE_DOXYGEN_INVOKED)
   typedef value_type &&                                                   reference;
   #else
   typedef typename boost::mpl::if_
      < boost::interprocess::is_movable<value_type>
      , boost::interprocess::rv<value_type>&
      , value_type & >::type                                               reference;
   #endif
   typedef typename std::iterator_traits<iterator_type>::pointer           pointer;
   typedef typename std::iterator_traits<iterator_type>::difference_type   difference_type;
   typedef typename std::iterator_traits<iterator_type>::iterator_category iterator_category;

   move_iterator()
   {}

   explicit move_iterator(It i)
      :  m_it(i)
   {}

   template <class U>
   move_iterator(const move_iterator<U>& u)
      :  m_it(u.base())
   {}

   iterator_type base() const
   {  return m_it;   }

   reference operator*() const
   {
      #if defined(BOOST_HAS_RVALUE_REFS)
      return *m_it;
      #else
      return boost::interprocess::move(*m_it);
      #endif
   }

   pointer   operator->() const
   {  return m_it;   }

   move_iterator& operator++()
   {  ++m_it; return *this;   }

   move_iterator<iterator_type>  operator++(int)
   {  move_iterator<iterator_type> tmp(*this); ++(*this); return tmp;   }

   move_iterator& operator--()
   {  --m_it; return *this;   }

   move_iterator<iterator_type>  operator--(int)
   {  move_iterator<iterator_type> tmp(*this); --(*this); return tmp;   }

   move_iterator<iterator_type>  operator+ (difference_type n) const
   {  return move_iterator<iterator_type>(m_it + n);  }

   move_iterator& operator+=(difference_type n)
   {  m_it += n; return *this;   }

   move_iterator<iterator_type>  operator- (difference_type n) const
   {  return move_iterator<iterator_type>(m_it - n);  }

   move_iterator& operator-=(difference_type n)
   {  m_it -= n; return *this;   }

   reference operator[](difference_type n) const
   {
      #if defined(BOOST_HAS_RVALUE_REFS)
      return m_it[n];
      #else
      return boost::interprocess::move(m_it[n]);
      #endif
   }

   friend bool operator==(const move_iterator& x, const move_iterator& y)
   {  return x.base() == y.base();  }

   friend bool operator!=(const move_iterator& x, const move_iterator& y)
   {  return x.base() != y.base();  }

   friend bool operator< (const move_iterator& x, const move_iterator& y)
   {  return x.base() < y.base();   }

   friend bool operator<=(const move_iterator& x, const move_iterator& y)
   {  return x.base() <= y.base();  }

   friend bool operator> (const move_iterator& x, const move_iterator& y)
   {  return x.base() > y.base();  }

   friend bool operator>=(const move_iterator& x, const move_iterator& y)
   {  return x.base() >= y.base();  }

   friend difference_type operator-(const move_iterator& x, const move_iterator& y)
   {  return x.base() - y.base();   }

   friend move_iterator operator+(difference_type n, const move_iterator& x)
   {  return move_iterator(x.base() + n);   }

   private:
   It m_it;
};


//is_move_iterator
namespace move_detail {

template <class I>
struct is_move_iterator
{
   static const bool value = false;
};

template <class I>
struct is_move_iterator< ::boost::interprocess::move_iterator<I> >
{
   static const bool value = true;
};

}  //namespace move_detail {

//////////////////////////////////////////////////////////////////////////////
//
//                            move_iterator
//
//////////////////////////////////////////////////////////////////////////////

//!
//! <b>Returns</b>: move_iterator<It>(i).
template<class It>
move_iterator<It> make_move_iterator(const It &it)
{  return move_iterator<It>(it); }

//////////////////////////////////////////////////////////////////////////////
//
//                         back_move_insert_iterator
//
//////////////////////////////////////////////////////////////////////////////


//! A move insert iterator that move constructs elements at the
//! back of a container
template <typename C> // C models Container
class back_move_insert_iterator
   : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
   C* container_m;

   public:
   typedef C container_type;

   explicit back_move_insert_iterator(C& x) : container_m(&x) { }

   back_move_insert_iterator& operator=(typename C::reference x)
   { container_m->push_back(boost::interprocess::move(x)); return *this; }

   back_move_insert_iterator& operator*()     { return *this; }
   back_move_insert_iterator& operator++()    { return *this; }
   back_move_insert_iterator& operator++(int) { return *this; }
};

//!
//! <b>Returns</b>: back_move_insert_iterator<C>(x).
template <typename C> // C models Container
inline back_move_insert_iterator<C> back_move_inserter(C& x)
{
   return back_move_insert_iterator<C>(x);
}

//////////////////////////////////////////////////////////////////////////////
//
//                         front_move_insert_iterator
//
//////////////////////////////////////////////////////////////////////////////

//! A move insert iterator that move constructs elements int the
//! front of a container
template <typename C> // C models Container
class front_move_insert_iterator
   : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
   C* container_m;

public:
   typedef C container_type;

   explicit front_move_insert_iterator(C& x) : container_m(&x) { }

   front_move_insert_iterator& operator=(typename C::reference x)
   { container_m->push_front(boost::interprocess::move(x)); return *this; }

   front_move_insert_iterator& operator*()     { return *this; }
   front_move_insert_iterator& operator++()    { return *this; }
   front_move_insert_iterator& operator++(int) { return *this; }
};

//!
//! <b>Returns</b>: front_move_insert_iterator<C>(x).
template <typename C> // C models Container
inline front_move_insert_iterator<C> front_move_inserter(C& x)
{
   return front_move_insert_iterator<C>(x);
}

//////////////////////////////////////////////////////////////////////////////
//
//                         insert_move_iterator
//
//////////////////////////////////////////////////////////////////////////////
template <typename C> // C models Container
class move_insert_iterator
   : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
   C* container_m;
   typename C::iterator pos_;

   public:
   typedef C container_type;

   explicit move_insert_iterator(C& x, typename C::iterator pos)
      : container_m(&x), pos_(pos)
   {}

   move_insert_iterator& operator=(typename C::reference x)
   {
      pos_ = container_m->insert(pos_, boost::interprocess::move(x));
      ++pos_;
      return *this;
   }

   move_insert_iterator& operator*()     { return *this; }
   move_insert_iterator& operator++()    { return *this; }
   move_insert_iterator& operator++(int) { return *this; }
};

//!
//! <b>Returns</b>: move_insert_iterator<C>(x, it).
template <typename C> // C models Container
inline move_insert_iterator<C> move_inserter(C& x, typename C::iterator it)
{
   return move_insert_iterator<C>(x, it);
}

//////////////////////////////////////////////////////////////////////////////
//
//                               move
//
//////////////////////////////////////////////////////////////////////////////


//! <b>Effects</b>: Moves elements in the range [first,last) into the range [result,result + (last -
//!   first)) starting from first and proceeding to last. For each non-negative integer n < (last-first),
//!   performs *(result + n) = boost::interprocess::move (*(first + n)).
//!
//! <b>Effects</b>: result + (last - first).
//!
//! <b>Requires</b>: result shall not be in the range [first,last).
//!
//! <b>Complexity</b>: Exactly last - first move assignments.
template <typename I, // I models InputIterator
          typename O> // O models OutputIterator
O move(I f, I l, O result)
{
   while (f != l) {
      *result = boost::interprocess::move(*f);
      ++f; ++result;
   }
   return result;
}

//////////////////////////////////////////////////////////////////////////////
//
//                               move_backward
//
//////////////////////////////////////////////////////////////////////////////

//! <b>Effects</b>: Moves elements in the range [first,last) into the range
//!   [result - (last-first),result) starting from last - 1 and proceeding to
//!   first. For each positive integer n <= (last - first),
//!   performs *(result - n) = boost::interprocess::move(*(last - n)).
//!
//! <b>Requires</b>: result shall not be in the range [first,last).
//!
//! <b>Returns</b>: result - (last - first).
//!
//! <b>Complexity</b>: Exactly last - first assignments.
template <typename I, // I models BidirectionalIterator
typename O> // O models BidirectionalIterator
O move_backward(I f, I l, O result)
{
   while (f != l) {
      --l; --result;
      *result = boost::interprocess::move(*l);
   }
   return result;
}

//////////////////////////////////////////////////////////////////////////////
//
//                               uninitialized_move
//
//////////////////////////////////////////////////////////////////////////////

//! <b>Effects</b>:
//!   \code
//!   for (; first != last; ++result, ++first)
//!      new (static_cast<void*>(&*result))
//!         typename iterator_traits<ForwardIterator>::value_type(boost::interprocess::move(*first));
//!   \endcode
//!
//! <b>Returns</b>: result
template
   <typename I, // I models InputIterator
    typename F> // F models ForwardIterator
F uninitialized_move(I f, I l, F r
   /// @cond
   ,typename enable_if<is_movable<typename std::iterator_traits<I>::value_type> >::type* = 0
   /// @endcond
   )
{
   typedef typename std::iterator_traits<I>::value_type input_value_type;
   while (f != l) {
      ::new(static_cast<void*>(&*r)) input_value_type(boost::interprocess::move(*f));
      ++f; ++r;
   }
   return r;
}

/// @cond

template
   <typename I,   // I models InputIterator
    typename F>   // F models ForwardIterator
F uninitialized_move(I f, I l, F r,
   typename disable_if<is_movable<typename std::iterator_traits<I>::value_type> >::type* = 0)
{
   return std::uninitialized_copy(f, l, r);
}

//////////////////////////////////////////////////////////////////////////////
//
//                            uninitialized_copy_or_move
//
//////////////////////////////////////////////////////////////////////////////

namespace move_detail {

template
<typename I,   // I models InputIterator
typename F>   // F models ForwardIterator
F uninitialized_move_move_iterator(I f, I l, F r,
                             typename enable_if< is_movable<typename I::value_type> >::type* = 0)
{
   return boost::interprocess::uninitialized_move(f, l, r);
}

template
<typename I,   // I models InputIterator
typename F>   // F models ForwardIterator
F uninitialized_move_move_iterator(I f, I l, F r,
                                   typename disable_if< is_movable<typename I::value_type> >::type* = 0)
{
   return std::uninitialized_copy(f.base(), l.base(), r);
}

}  //namespace move_detail {

template
<typename I,   // I models InputIterator
typename F>   // F models ForwardIterator
F uninitialized_copy_or_move(I f, I l, F r,
                             typename enable_if< move_detail::is_move_iterator<I> >::type* = 0)
{
   return boost::interprocess::move_detail::uninitialized_move_move_iterator(f, l, r);
}

/// @endcond

//! <b>Effects</b>:
//!   \code
//!   for (; first != last; ++result, ++first)
//!      new (static_cast<void*>(&*result))
//!         typename iterator_traits<ForwardIterator>::value_type(*first);
//!   \endcode
//!
//! <b>Returns</b>: result
//!
//! <b>Note</b>: This function is provided because
//!   <i>std::uninitialized_copy</i> from some STL implementations
//!    is not compatible with <i>move_iterator</i>
template
<typename I,   // I models InputIterator
typename F>   // F models ForwardIterator
F uninitialized_copy_or_move(I f, I l, F r
   /// @cond
   ,typename disable_if< move_detail::is_move_iterator<I> >::type* = 0
   /// @endcond
   )
{
   return std::uninitialized_copy(f, l, r);
}

///has_trivial_destructor_after_move<> == true_type
///specialization for optimizations
template <class T>
struct has_trivial_destructor_after_move
   : public boost::has_trivial_destructor<T>
{};

}  //namespace interprocess {
}  //namespace boost {

#endif  //#ifndef BOOST_INTERPROCESS_MOVE_HPP
