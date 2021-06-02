////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

template < typename key_type, typename value_type, typename predicate_type = ::std::less< key_type > >
class multimap : public 
	::std::multimap<
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
	typedef multimap< key_type, value_type, predicate_type >	self_type;

public:
	inline				multimap	( );
	inline				multimap	( predicate_type const& other);
	inline				multimap	( self_type const& other);
	template < typename input_iterator >
	inline				multimap	( input_iterator const& first, input_iterator const& last );
	template < typename input_iterator >
	inline				multimap	( input_iterator const& first, input_iterator const& last, predicate_type const& predicate );
	inline	self_type&	operator=	( self_type const& other );

private:
	typedef ::std::multimap<
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
}; // class multimap

#include <xray/multimap_inline.h>