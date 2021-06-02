////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

#define TEMPLATE_SIGNATURE	template < typename key_type, typename predicate_type >
#define SET					set < key_type, predicate_type >

TEMPLATE_SIGNATURE
inline SET::set									( )
{
}

TEMPLATE_SIGNATURE
inline SET::set									( predicate_type const& predicate) :
	super				( predicate )
{
}

TEMPLATE_SIGNATURE
inline SET::set									( self_type const& other ) :
	super				( other )
{
}

TEMPLATE_SIGNATURE
template <typename input_iterator>
inline SET::set									( input_iterator const& first, input_iterator const& last ) :
	super				( first, last )
{
}

TEMPLATE_SIGNATURE
template <typename input_iterator>
inline SET::set									( input_iterator const& first, input_iterator const& last, predicate_type const& predicate ) :
	super				( first, last, predicate )
{
}

TEMPLATE_SIGNATURE
inline typename SET::self_type& SET::operator=	( self_type const& other )
{
	return				super::operator=( other );
}

#undef SET
#undef TEMPLATE_SIGNATURE