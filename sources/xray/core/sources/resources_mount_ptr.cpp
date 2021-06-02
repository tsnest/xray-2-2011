////////////////////////////////////////////////////////////////////////////
//	Created		: 09.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_mount_ptr.h>
#include "resources_manager.h"
#include "game_resman.h"

namespace xray {
	namespace vfs {
		u32 get_global_unmounts_count	(); // for debug
	} // namespace vfs

namespace resources {

void   intrusive_fs_task_unmount_base::destroy	(fs_task_unmount * object) const
{
	LOGI_TRACE								("resources", "adding unmount task: '%s' on '%s'", 
											 object->get_vfs_mount_ptr()->get_physical_path(), 
											 object->get_vfs_mount_ptr()->get_virtual_path());

	g_resources_manager->change_count_of_pending_mount_operations	(+1);
	g_resources_manager->add_fs_task		(object);
}

fs_task_unmount::fs_task_unmount		(vfs_sub_fat_resource_ptr const & sub_fat_ptr) 
	:	fs_task			(type_unmount, NULL),
		m_sub_fat_ptr	(sub_fat_ptr) 
{
}

void   fs_task_unmount::unmount_children	(vfs::vfs_mount * sub_fat)
{
	u32 const children_count			=	sub_fat->children.size();
	if ( children_count )
	{
		buffer_vector<vfs::vfs_mount *>	children	(ALLOCA(children_count * sizeof(vfs::vfs_mount *)), children_count);
		vfs::vfs_mount::children_list::mutex_raii	raii	(sub_fat->children.synchronization_primitive());
		for ( vfs::vfs_mount_ptr	it	=	sub_fat->children.front();
									it;
									it	=	sub_fat->children.get_next_of_object(it) )
		{
			children.push_back				(it.c_ptr());
		}

		for ( u32 i=0; i<children_count; ++i )
		{
			vfs::vfs_mount * child		=	children[i];
			unmount_children				(child);
		}
	}

	if ( m_sub_fat_ptr->mount_ptr.c_ptr() == sub_fat )
		return; // skip root sub_fat
	
	R_ASSERT								(sub_fat->user_data);
	vfs_sub_fat_resource * sub_fat_resource	=	(vfs_sub_fat_resource *)sub_fat->user_data;
	g_game_resources_manager->release_sub_fat	(sub_fat_resource);
}

void   fs_task_unmount::execute_may_destroy_this	()
{
	static u32 s_iter = 0;
++s_iter;
	u32 const umount_count1				=	vfs::get_global_unmounts_count();
	LOGI_INFO("resources:test", "running unmount_task");

	unmount_children						(m_sub_fat_ptr->mount_ptr.c_ptr());

	vfs_sub_fat_resource * sub_fat		=	m_sub_fat_ptr.c_ptr();
	bool const is_captured				=	sub_fat->is_captured();
	m_sub_fat_ptr						=	NULL; // no user links now

	if ( is_captured )
		g_game_resources_manager->release_sub_fat	(sub_fat);

	u32 const umount_count2				=	vfs::get_global_unmounts_count();
	R_ASSERT								(umount_count2 != umount_count1, "no unmount occured! Call Lain");

	fs_task_unmount * this_ptr			=	this;
	XRAY_DELETE_IMPL						(helper_allocator(), this_ptr);
}

} // namespace resources
} // namespace xray
