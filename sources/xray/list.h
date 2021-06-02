////////////////////////////////////////////////////////////////////////////
//	Created		: 15.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

// no header guards, since header may be included into the
// different namespaces to support several allocators

template < typename T >
class list : public ::std::list< T, std_allocator< T > > {
public:
	typedef list< T >	self_type;

private:
}; // class list

#include <xray/list_inline.h>