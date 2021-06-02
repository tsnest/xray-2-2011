////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_MACROS_H_INCLUDED
#define RESOURCES_MACROS_H_INCLUDED

#include <xray/memory_doug_lea_allocator.h>

namespace xray {
namespace memory {

extern  doug_lea_allocator_type						g_resources_helper_allocator;

} // namespace memory
} // namespace xray

#define RES_NEW( type )			XRAY_NEW_IMPL(		::xray::memory::g_resources_helper_allocator, type )
#define RES_DELETE( pointer )	XRAY_DELETE_IMPL(	::xray::memory::g_resources_helper_allocator, pointer )
#define RES_ALLOC( type, count )XRAY_ALLOC_IMPL(	::xray::memory::g_resources_helper_allocator, type, count )
#define RES_FREE( pointer )		XRAY_FREE_IMPL(		::xray::memory::g_resources_helper_allocator, pointer )

#endif // #ifndef FS_MACROS_H_INCLUDED