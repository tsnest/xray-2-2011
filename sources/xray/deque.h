////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

template < typename T >
class deque : public ::std::deque< T, std_allocator< T > > {
public:
	typedef deque<T>								self_type;

private:
	typedef ::std::deque< T, std_allocator< T > >	super;

public:
	typedef typename super::size_type				size_type;
	typedef typename super::value_type				value_type;

public:
	inline						deque		( );
	inline						deque		( size_type count, value_type const& value );
	inline						deque		( self_type const& other);
	template < typename input_iterator >
	inline						deque		( input_iterator const& first, input_iterator const& last );
	inline	self_type&			operator=	( self_type const& other );
	inline	u32					size		( ) const;
}; // class deque

#include <xray/deque_inline.h>