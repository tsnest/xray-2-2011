////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

template < typename key_type, typename predicate_type = ::std::less< key_type > >
class multiset : public 
	::std::multiset<
		key_type,
		predicate_type,
		std_allocator<
			key_type
		>
	>
{
public:
	typedef multiset< key_type, predicate_type >	self_type;

public:
	inline				multiset			( );
	inline				multiset			( predicate_type const& other);
	inline				multiset			( self_type const& other);
	template < typename input_iterator >
	inline				multiset			( input_iterator const& first, input_iterator const& last );
	template < typename input_iterator >
	inline				multiset			( input_iterator const& first, input_iterator const& last, predicate_type const& predicate );
	inline	self_type&	operator=	( self_type const& other );

private:
	typedef ::std::multiset<
		key_type,
		predicate_type,
		std_allocator<
			key_type
		>
	>								super;
}; // class multiset

#include <xray/multiset_inline.h>