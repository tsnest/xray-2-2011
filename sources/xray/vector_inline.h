////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

#define TEMPLATE_SIGNATURE	template < typename T >
#define VECTOR				vector < T >

TEMPLATE_SIGNATURE
inline VECTOR::vector											( )
{
}

TEMPLATE_SIGNATURE
inline VECTOR::vector											( size_type const count, value_type const& value ) :
	super				( count, value )
{
}

TEMPLATE_SIGNATURE
inline VECTOR::vector											( self_type const& other ) :
	super				( other )
{
}

TEMPLATE_SIGNATURE
inline VECTOR::vector											( size_type count) :
	super				( count )
{
}

TEMPLATE_SIGNATURE
template <typename input_iterator>
inline VECTOR::vector											( input_iterator const& first, input_iterator const& last ) :
	super				( first, last )
{
}

TEMPLATE_SIGNATURE
inline typename VECTOR::self_type& VECTOR::operator=			( self_type const& other )
{
	super::operator=	( other );
	return				( *this );
}

TEMPLATE_SIGNATURE
inline typename VECTOR::reference	VECTOR::operator[]			( u32 index )
{
	R_ASSERT			( index < size(), "index is out of range: %d, size %d", index, size() );
	return				super::operator[] ( index );
}

TEMPLATE_SIGNATURE
inline typename VECTOR::const_reference VECTOR::operator[]		( u32 index ) const
{
	R_ASSERT			( index < size(), "index is out of range: %d, size %d", index, size() );
	return				super::operator[] ( index );
}

TEMPLATE_SIGNATURE
inline u32 VECTOR::size											( ) const
{
	return				( u32 )super::size();
}

#undef VECTOR
#undef TEMPLATE_SIGNATURE