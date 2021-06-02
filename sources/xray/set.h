////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

template < typename key_type, typename predicate_type = ::std::less< key_type > >
class set : public 
	::std::set<
		key_type,
		predicate_type,
		std_allocator<
			key_type
		>
	>
{
public:
	typedef set< key_type, predicate_type >	self_type;

public:
	inline				set			( );
	inline				set			( predicate_type const& other);
	inline				set			( self_type const& other);
	template < typename input_iterator >
	inline				set			( input_iterator const& first, input_iterator const& last );
	template < typename input_iterator >
	inline				set			( input_iterator const& first, input_iterator const& last, predicate_type const& predicate );
	inline	self_type&	operator=	( self_type const& other );

private:
	typedef ::std::set<
		key_type,
		predicate_type,
		std_allocator<
			key_type
		>
	>								super;
}; // class set

#include <xray/set_inline.h>