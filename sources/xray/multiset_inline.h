////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

#define TEMPLATE_SIGNATURE	template < typename key_type, typename predicate_type >
#define MULTISET			multiset < key_type, predicate_type >

TEMPLATE_SIGNATURE
inline MULTISET::multiset									( )
{
}

TEMPLATE_SIGNATURE
inline MULTISET::multiset									( predicate_type const& predicate) :
	super				( predicate )
{
}

TEMPLATE_SIGNATURE
inline MULTISET::multiset									( self_type const& other ) :
	super				( other )
{
}

TEMPLATE_SIGNATURE
template <typename input_iterator>
inline MULTISET::multiset									( input_iterator const& first, input_iterator const& last ) :
	super				( first, last )
{
}

TEMPLATE_SIGNATURE
template <typename input_iterator>
inline MULTISET::multiset									( input_iterator const& first, input_iterator const& last, predicate_type const& predicate ) :
	super				( first, last, predicate )
{
}

TEMPLATE_SIGNATURE
inline typename MULTISET::self_type& MULTISET::operator=	( self_type const& other )
{
	return				super::operator=( other );
}

#undef MULTISET
#undef TEMPLATE_SIGNATURE