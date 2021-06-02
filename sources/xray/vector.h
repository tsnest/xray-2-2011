////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

template < typename T >
class vector : public ::std::vector< T, std_allocator< T > > {
public:
	typedef vector<T>								self_type;

private:
	typedef ::std::vector< T, std_allocator< T > >	super;

public:
	typedef typename super::size_type				size_type;
	typedef typename super::value_type				value_type;
	typedef typename super::reference				reference;
	typedef typename super::const_reference			const_reference;

public:
	inline						vector		( );
	inline						vector		( size_type count, value_type const& value );
	inline						vector		( self_type const& other);
	inline	explicit			vector		( size_type count);
	template < typename input_iterator >
	inline						vector		( input_iterator const& first, input_iterator const& last );
	inline	self_type&			operator=	( self_type const& other );
	inline	reference			operator[]	( u32 index );
	inline	const_reference		operator[]	( u32 index ) const;
	inline	u32					size		( ) const;
}; // class vector

#include <xray/vector_inline.h>