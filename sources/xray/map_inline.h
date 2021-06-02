////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

#define TEMPLATE_SIGNATURE	template < typename key_type, typename value_type, typename predicate_type >
#define MAP					map < key_type, value_type, predicate_type >

TEMPLATE_SIGNATURE
inline MAP::map									( )
{
}

TEMPLATE_SIGNATURE
inline MAP::map									( predicate_type const& predicate) :
	super				( predicate )
{
}

TEMPLATE_SIGNATURE
inline MAP::map									( self_type const& other ) :
	super				( other )
{
}

TEMPLATE_SIGNATURE
template <typename input_iterator>
inline MAP::map									( input_iterator const& first, input_iterator const& last ) :
	super				( first, last )
{
}

TEMPLATE_SIGNATURE
template <typename input_iterator>
inline MAP::map									( input_iterator const& first, input_iterator const& last, predicate_type const& predicate ) :
	super				( first, last, predicate )
{
}

TEMPLATE_SIGNATURE
inline typename MAP::self_type& MAP::operator=	( self_type const& other )
{
	return				super::operator=( other );
}

#undef MAP
#undef TEMPLATE_SIGNATURE