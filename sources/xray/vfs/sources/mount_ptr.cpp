////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs/mount_ptr.h>
#include <xray/vfs/virtual_file_system.h>
#include "nodes.h"
#include "branch_locks.h"
#include "unmount.h"

namespace xray {
namespace vfs {

bool   current_thread_can_unmount		()
{
	return									true;
}

pcstr	vfs_mount::get_physical_path	() const
{
	return									m_mount_root->physical_path;
}

pcstr	vfs_mount::get_virtual_path		() const
{
	return									m_mount_root->virtual_path;
}

pcstr	vfs_mount::get_descriptor		() const
{
	return									m_mount_root->descriptor;
}

u32   vfs_mount::get_mount_size			() const
{
	return									m_mount_root->mount_size;
}

vfs_mount_ptr   vfs_mount::make_pointer_possibly_increase_destroy_count ()
{
	if ( change_reference_count_change_may_destroy_this(+1) == 1 )
		increment_destroy_count				();

	vfs_mount_ptr	out_result				(this);

	change_reference_count_change_may_destroy_this	(-1);

	return									out_result;
}

void	vfs_mount::unlink_from_parent	()
{
	if ( !parent )
		return;
	parent->children.erase					(this);
	parent								=	NULL;
}

void	vfs_intrusive_mount_base::unlink_children	(vfs_mount * object, virtual_file_system & file_system) const
{
	while ( !object->children.empty() )
	{
		vfs_mount_ptr	child			=	object->children.pop_back();
		
		R_ASSERT_CMP						(child->get_reference_count(), ==, 1, "reference count is still held by someone other then parent!");
		child							=	NULL;
	}
}

void	vfs_intrusive_mount_base::destroy	(vfs_mount * object) const
{
	mount_root_node_base<> * const mount_root	=	object->get_mount_root();
	memory::base_allocator * const allocator	=	object->get_allocator();
	if ( !mount_root )
	{
		u32 const ref_count				=	object->decrement_destroy_count();
		R_ASSERT							(!ref_count);
		XRAY_DELETE_IMPL					(allocator, object);
		return;
	}

	virtual_file_system & file_system	=	* mount_root->file_system;

	unlink_children							(object, file_system);


	mount_history_container & mount_history	=	file_system.mount_history;
	mount_history_container::mutex_raii	raii	(mount_history.policy());

	if ( !mount_history.contains_object(object) )
	{
		// object was unmounted, look if we're last to destroy it...
		if ( object->decrement_destroy_count() == 0 )
			XRAY_DELETE_IMPL				(allocator, object);

		return; 
	}
	
	if ( file_system.unmount_thread_id != u32(-1) && 
		 file_system.unmount_thread_id != threading::current_thread_id() )
	{
		// only dedicated thread can do it... schedule it
		LOG_INFO	("rescheduling unmount (wrong thread) '%s' on '%s'", (pcstr)mount_root->physical_path, (pcstr)mount_root->virtual_path);
		file_system.scheduled_to_unmount.push_back	(object);
		return;
	}

	query_mount_arguments					args;
	base_node<> * root_write_lock		=	NULL;
				
	if ( !file_system.hashset.find_and_lock_branch	(root_write_lock, "", lock_type_write, lock_operation_try_lock) )
	{
		LOG_INFO	("rescheduling unmount (cannot lock) '%s' on '%s'", (pcstr)mount_root->physical_path, (pcstr)mount_root->virtual_path);
		// we cant do it now, lets try later...
		file_system.scheduled_to_unmount.push_back	(object);
		return;
	}

	R_ASSERT								(root_write_lock);

	if ( object->get_reference_count() != 0 )
	{
		LOG_INFO	("unmount canceled (mount-reclaimed) '%s' on '%s'", mount_root->physical_path, mount_root->virtual_path);
		unlock_branch						(args.root_write_lock, lock_type_write);
		// mounters have stolen this object from the laps of god of death
		return;
	}

	erase_from_mount_history				(object, file_system);
	raii.clear								();

	fs_new::asynchronous_device_interface	device_interface	(mount_root->device, fs_new::watcher_enabled_true);

	if ( mount_root->mount_type == mount_type_archive )
	{
		archive_folder_mount_root_node<> *	archive_mount_root	=	node_cast<archive_folder_mount_root_node>(mount_root);
		args							=	query_mount_arguments::mount_archive
											   (mount_root->allocator, 
												mount_root->virtual_path,
												archive_mount_root->archive_path_holder, 
												archive_mount_root->fat_path_holder,
												archive_mount_root->descriptor,
												& device_interface,
												NULL,
												NULL,
												lock_operation_lock);
	}
	else
	{
		args							=	query_mount_arguments::mount_physical_path
											   (mount_root->allocator, 
												mount_root->virtual_path, 
												mount_root->physical_path, 
												mount_root->descriptor,
												& device_interface, 
												NULL, 
												NULL, 
												recursive_true, 
												lock_operation_lock,
												mount_root->watcher_enabled);
	}

	args.mount_ptr						=	object;
	args.root_write_lock				=	root_write_lock;

	unmount									(args, file_system);

	// look if we're last to destroy it
	if ( object->decrement_destroy_count() == 0 )
		XRAY_DELETE_IMPL					(allocator, object);
}

u32   vfs_mount::change_reference_count_change_may_destroy_this (int change)
{
	if ( change > 0 )
	{
		return								threading::interlocked_exchange_add(m_reference_count, change) + change;
	}
	else if ( change == -1 )
	{
		R_ASSERT							(m_reference_count != 0);
		vfs_mount_ptr const holder			(this);
		u32 const out_result			=	threading::interlocked_decrement(m_reference_count) - 1;
		return								out_result;
	}
	else
		UNREACHABLE_CODE					(return 0);
}

} // namespace vfs
} // namespace xray