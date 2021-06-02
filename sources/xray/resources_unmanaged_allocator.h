////////////////////////////////////////////////////////////////////////////
//	Created		: 20.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_UNMANAGED_ALLOCATOR_H_INCLUDED
#define XRAY_RESOURCES_UNMANAGED_ALLOCATOR_H_INCLUDED

namespace xray {
namespace resources {

XRAY_CORE_API managed_resource *		allocate_managed_resource	(u32 size, class_id_enum class_id);
XRAY_CORE_API void						free_managed_resource		(managed_resource *);

#define UNMANAGED_NEW( out_pointer, type )	(((out_pointer) = (type*)xray::resources::allocate_unmanaged_memory(sizeof(type), typeid(type).name())) == 0) ? 0 : new (out_pointer) type
#define UNMANAGED_DELETE( pointer )			XRAY_DELETE_IMPL( xray::resources::unmanaged_allocator(), pointer )
#define UNMANAGED_ALLOC( type, count )		(type*)xray::resources::allocate_unmanaged_memory(sizeof(type)*count, typeid(type).name())
#define UNMANAGED_FREE( pointer )			XRAY_FREE_IMPL( xray::resources::unmanaged_allocator(), pointer )

// use UNMANAGED_* macro instead
XRAY_CORE_API memory::base_allocator *	unmanaged_allocator			();
XRAY_CORE_API memory::base_allocator *	helper_allocator			();
XRAY_CORE_API pvoid						allocate_unmanaged_memory	(u32 size, pcstr type_name);

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_UNMANAGED_ALLOCATOR_H_INCLUDED