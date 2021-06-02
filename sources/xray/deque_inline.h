////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

#define TEMPLATE_SIGNATURE	template < typename T >
#define DEQUE				deque < T >

TEMPLATE_SIGNATURE
inline DEQUE::deque											( )
{
}

TEMPLATE_SIGNATURE
inline DEQUE::deque											( size_type const count, value_type const& value ) :
	super				( count, value )
{
}

TEMPLATE_SIGNATURE
inline DEQUE::deque											( self_type const& other ) :
	super				( other )
{
}

TEMPLATE_SIGNATURE
template <typename input_iterator>
inline DEQUE::deque											( input_iterator const& first, input_iterator const& last ) :
	super				( first, last )
{
}

TEMPLATE_SIGNATURE
inline typename DEQUE::self_type& DEQUE::operator=			( self_type const& other )
{
	super::operator=	( other );
	return					( *this );
}

TEMPLATE_SIGNATURE
inline u32 DEQUE::size											( ) const
{
	return					( ( u32 ) super::size( ) );
}

#undef DEQUE
#undef TEMPLATE_SIGNATURE