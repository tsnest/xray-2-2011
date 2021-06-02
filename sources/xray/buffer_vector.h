////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.10.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_BUFFER_VECTOR_H_INCLUDED
#define XRAY_BUFFER_VECTOR_H_INCLUDED

#include <iterator>
#include <boost/type_traits/remove_cv.hpp>

namespace xray {

template < typename T >
class buffer_vector {
public:
	typedef T const*								const_iterator;
	typedef T const*								const_pointer;
	typedef T const&								const_reference;
	typedef typename boost::remove_cv<T>::type const & non_volatile_const_reference;
	typedef std::reverse_iterator< const_iterator >	const_reverse_iterator;

	typedef T*										iterator;
	typedef T*										pointer;
	typedef typename boost::remove_cv<T>::type *	non_volatile_pointer;
	typedef T&										reference;
	typedef std::reverse_iterator< iterator >		reverse_iterator;

	typedef ptrdiff_t								difference_type;
	typedef u32										size_type;
	typedef T										value_type;

private:
	typedef buffer_vector< T >						self_type;

public:
	inline							buffer_vector	( pvoid buffer, size_type max_count, size_type live_count = 0 );
	inline							buffer_vector	( pvoid buffer, size_type max_count, size_type count, value_type const& value );
	inline							buffer_vector	( pvoid buffer, size_type max_count, self_type const& other );
	template < typename input_iterator >
	inline							buffer_vector	( pvoid buffer, size_type max_count, input_iterator const& begin, input_iterator const& end );
	inline							~buffer_vector	( );

	inline	self_type				&operator=		( self_type const& other );

//	inline	bool					operator==		( self_type const& other ) const;

	template < typename input_iterator >
	inline	void					assign			( input_iterator begin, input_iterator const& end );
	inline	void					assign			( size_type count, non_volatile_const_reference value );

	inline	void					swap			( self_type& other );
	inline	void					clear			( );
	inline	void					resize			( size_type size );
	inline	void					resize			( size_type size, non_volatile_const_reference value );
	inline	void					reserve			( size_type size );

	template < typename input_iterator >
	inline	void					insert			( iterator const& where, input_iterator begin, input_iterator const& last );
	inline	void					insert			( iterator const& where, size_type count, non_volatile_const_reference value );
	inline	void					insert			( iterator const& where, non_volatile_const_reference value );

	inline	void					erase			( iterator const& begin, iterator const& end );
	inline	void					erase			( iterator const& where );

	inline	void					pop_back		( );
	inline	void					push_back		( non_volatile_const_reference value );

	inline	reference				at				( size_type index );
	inline	const_reference			at				( size_type index ) const;

	inline	reference				operator [ ]	( size_type index );
	inline	const_reference			operator [ ]	( size_type index ) const;

	inline	reference				back			( );
	inline	const_reference			back			( ) const;

	inline	reference				front			( );
	inline	const_reference			front			( ) const;

	inline	iterator				begin			( );
	inline	const_iterator			begin			( ) const;

	inline	iterator				end				( );
	inline	const_iterator			end				( ) const;

	inline	reverse_iterator		rbegin			( );
	inline	const_reverse_iterator	rbegin			( ) const;

	inline	reverse_iterator		rend			( );
	inline	const_reverse_iterator	rend			( ) const;

	inline	bool					empty			( ) const;
	inline	size_type				size			( ) const;

#ifdef DEBUG
	inline	size_type				capacity		( ) const;
	inline	size_type				max_size		( ) const;
#endif // #ifdef DEBUG

private:
	static inline void				construct		( pointer p );
	static inline void				construct		( pointer p, non_volatile_const_reference value );
	static inline void				construct		( iterator begin, iterator const& end );
	static inline void				construct		( iterator begin, iterator const& end, non_volatile_const_reference value );

private:
	static inline void				destroy			( pointer p );
	static inline void				destroy			( iterator begin, iterator const& end );

private:
	inline							buffer_vector	( self_type const& other );

private:
	pointer							m_begin;
	pointer							m_end;
#ifdef DEBUG
	pointer							m_max_end;
#endif // #ifdef DEBUG
};

} // namespace xray

	template < typename T >
	inline	void					swap			( ::xray::buffer_vector< T >& left, ::xray::buffer_vector< T >& right );

#include <xray/buffer_vector_inline.h>

#endif // #ifndef XRAY_BUFFER_VECTOR_H_INCLUDED