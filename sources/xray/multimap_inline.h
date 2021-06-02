////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

#define TEMPLATE_SIGNATURE	template < typename key_type, typename value_type, typename predicate_type >
#define MULTIMAP			multimap < key_type, value_type, predicate_type >

TEMPLATE_SIGNATURE
inline MULTIMAP::multimap									( )
{
}

TEMPLATE_SIGNATURE
inline MULTIMAP::multimap									( predicate_type const& predicate) :
	super				( predicate )
{
}

TEMPLATE_SIGNATURE
inline MULTIMAP::multimap									( self_type const& other ) :
	super				( other )
{
}

TEMPLATE_SIGNATURE
template <typename input_iterator>
inline MULTIMAP::multimap									( input_iterator const& first, input_iterator const& last ) :
	super				( first, last )
{
}

TEMPLATE_SIGNATURE
template <typename input_iterator>
inline MULTIMAP::multimap									( input_iterator const& first, input_iterator const& last, predicate_type const& predicate ) :
	super				( first, last, predicate )
{
}

TEMPLATE_SIGNATURE
inline typename MULTIMAP::self_type& MULTIMAP::operator=	( self_type const& other )
{
	return				super::operator=( other );
}

#undef MULTIMAP
#undef TEMPLATE_SIGNATURE