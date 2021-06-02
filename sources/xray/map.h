////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

template < typename key_type, typename value_type, typename predicate_type = ::std::less< key_type > >
class map : public 
	::std::map<
		key_type,
		value_type,
		predicate_type,
		std_allocator<
			::std::pair<
				key_type,
				value_type
			>
		>
	>
{
public:
	typedef map< key_type, value_type, predicate_type >	self_type;

public:
	inline				map			( );
	inline				map			( predicate_type const& other);
	inline				map			( self_type const& other);
	template < typename input_iterator >
	inline				map			( input_iterator const& first, input_iterator const& last );
	template < typename input_iterator >
	inline				map			( input_iterator const& first, input_iterator const& last, predicate_type const& predicate );
	inline	self_type&	operator=	( self_type const& other );

private:
	typedef ::std::map<
		key_type,
		value_type,
		predicate_type,
		std_allocator<
			::std::pair<
				key_type,
				value_type
			>
		>
	>								super;
}; // class map

#include <xray/map_inline.h>