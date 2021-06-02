////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.11.2008
//	Author		: Dmitriy Iassenev
//	Description : memory
////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include <xray/physics/api.h>
//
namespace xray {
namespace physics {
//
	extern memory::base_allocator*					g_ph_allocator;
//
//#define USER_ALLOCATOR							*::xray::physics::g_allocator
//#include <xray/std_containers.h>
//#include <xray/unique_ptr.h>
//#undef USER_ALLOCATOR
//
} // namespace physics
} // namespace xray

//
//#define NEW( type )								XRAY_NEW_IMPL(		*::xray::physics::g_allocator, type )
//#define DELETE( pointer )						XRAY_DELETE_IMPL(	*::xray::physics::g_allocator, pointer )
//#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	*::xray::physics::g_allocator, size, description )
//#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::physics::g_allocator, pointer, size, description )
//#define FREE( pointer )							XRAY_FREE_IMPL(		*::xray::physics::g_allocator, pointer )
//#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::xray::physics::g_allocator, type, count )

#endif // #ifndef MEMORY_H_INCLUDED