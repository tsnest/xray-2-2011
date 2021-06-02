////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs_sub_fat_resource.h>
#include "vfs_sub_fat_cook.h"

namespace xray {
namespace resources {

vfs_sub_fat_cook::vfs_sub_fat_cook		()
	:	unmanaged_cook	(vfs_sub_fat_class,
						 reuse_true,
						 use_resource_manager_thread_id,
						 thread_id_unset, 
						 flag_create_allocates_destroy_deallocates)
{
}

void   vfs_sub_fat_cook::destroy_resource	(unmanaged_resource * resource)
{
	vfs_sub_fat_resource * sub_fat		=	static_cast_checked<vfs_sub_fat_resource *>(resource);
	R_ASSERT								(sub_fat->mount_ptr);
	threading::interlocked_exchange_pointer	(sub_fat->mount_ptr->user_data, NULL);
	
	vfs::vfs_mount_ptr parent			=	sub_fat->mount_ptr->parent;

	if ( parent )
	{
		sub_fat->mount_ptr->unlink_from_parent	();
		R_ASSERT_CMP						(sub_fat->mount_ptr->get_reference_count(), ==, 1); // we should hold last ref...
	}
	
	sub_fat->mount_ptr					=	NULL;

	parent								=	NULL;

	XRAY_DELETE_IMPL						(helper_allocator(), resource);
}

} // namespace resources
} // namespace xray