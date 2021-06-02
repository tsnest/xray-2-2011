////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <xray/network/api.h>

#include "std_string.h"

namespace xray {
namespace network {

extern allocator_type*						g_allocator;

#define USER_ALLOCATOR						*::xray::network::g_allocator
#include <xray/std_containers.h>
#include <xray/unique_ptr.h>
#undef USER_ALLOCATOR

template < typename T >
class vector_size_t : public vector< T > {
private:
	typedef vector< T >						super;

public:
	typedef vector_size_t< T >				self_type;
	typedef typename super::size_type		size_type;
	typedef typename super::value_type		value_type;
	typedef typename super::reference		reference;
	typedef typename super::const_reference	const_reference;

public:
	inline				vector_size_t		( ) {}
	inline				vector_size_t		( size_type count, value_type const& value ) : super(count,value) {}
	inline				vector_size_t		( self_type const& other) : super(other) {}
	inline	explicit	vector_size_t		( size_type count) : super(count) {}
	template < typename input_iterator >
	inline				vector_size_t		( input_iterator const& first, input_iterator const& last ) : super(first, last) {}
	inline	reference		operator[]		( size_t index ) { return super::operator[]( (u32)index ); }
	inline	const_reference	operator[]		( size_t index ) const { return super::operator[]( (u32)index ); }
}; // class vector

} // namespace network
} // namespace xray

#define XN_NEW( type )								XRAY_NEW_IMPL(		*::xray::network::g_allocator, type )
#define XN_DELETE( pointer )						XRAY_DELETE_IMPL(	*::xray::network::g_allocator, pointer )
#define XN_MALLOC( size, description )				XRAY_MALLOC_IMPL(	*::xray::network::g_allocator, size, description )
#define XN_REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::network::g_allocator, pointer, size, description )
#define XN_FREE( pointer )							XRAY_FREE_IMPL(		*::xray::network::g_allocator, pointer )
#define XN_ALLOC( type, count )						XRAY_ALLOC_IMPL(	*::xray::network::g_allocator, type, count )

#define XRAY_BOOST_NEW								XN_NEW
#define XRAY_BOOST_DELETE							XN_DELETE

#define XRAY_BOOST_MALLOC							XN_MALLOC
#define XRAY_BOOST_FREE								XN_FREE

#endif // #ifndef MEMORY_H_INCLUDED