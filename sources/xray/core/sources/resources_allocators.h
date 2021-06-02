////////////////////////////////////////////////////////////////////////////
//	Created		: 16.06.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_ALLOCATORS_H_INCLUDED
#define RESOURCES_ALLOCATORS_H_INCLUDED

#include <xray/memory_fixed_size_allocator.h>
#include <xray/resources_resource_link.h>
#include <xray/managed_allocator.h>

namespace xray {
namespace memory {

extern	managed_allocator							g_resources_managed_allocator;
extern	doug_lea_allocator_type						g_resources_unmanaged_allocator;
extern	doug_lea_allocator_type						g_cook_allocator;
extern	fixed_size_allocator<
			xray::resources::resource_link,
			threading::mutex
		>											g_resources_links_allocator;

} // namespace memory
} // namespace xray

#endif // #ifndef RESOURCES_ALLOCATORS_H_INCLUDED