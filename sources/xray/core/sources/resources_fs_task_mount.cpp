////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_fs_task_mount.h"
#include "resources_manager.h"

namespace xray {
namespace resources {

fs_task_mount::fs_task_mount			(fs_new::virtual_path_string const & 	virtual_path,
										 fs_new::native_path_string const & 	physical_path,
										 pcstr									descriptor,
										 fs_new::watcher_enabled_bool			watcher_enabled,
										 query_mount_callback const &			callback,
										 recursive_bool							recursive,
										 memory::base_allocator *				allocator)
	:	fs_task					(type_mount_physical, allocator),
		m_virtual_path			(virtual_path),
		m_physical_path			(physical_path),
		m_descriptor			(descriptor),
		m_watcher_enabled		(watcher_enabled),
		m_callback				(callback),
		m_recursive				(recursive),
		m_reverse_byte_order	(false),
		m_fat_physical_path		(m_physical_path)
{
	LOGI_TRACE("resources", "adding mount task: '%s' on '%s'", physical_path.c_str(), virtual_path.c_str());
	g_resources_manager->change_count_of_pending_mount_operations	(+1);
}

fs_task_mount::fs_task_mount			(fs_new::virtual_path_string const & 	virtual_path,
										 fs_new::native_path_string const & 	fat_physical_path,
										 fs_new::native_path_string const & 	archive_physical_path,
										 pcstr									descriptor,
										 query_mount_callback const &			callback,
										 memory::base_allocator *				allocator)
	:	fs_task					(type_mount_archive, allocator),
		m_virtual_path			(virtual_path),
		m_archive_physical_path	(archive_physical_path),
		m_descriptor			(descriptor),
		m_watcher_enabled		(fs_new::watcher_enabled_false),
		m_callback				(callback),
		m_recursive				(recursive_true),
		m_fat_physical_path		(m_physical_path),
		m_reverse_byte_order	(false)
{
	LOGI_TRACE("resources", "adding mount task: '%s' on '%s'", fat_physical_path.c_str(), virtual_path.c_str());
	m_fat_physical_path					=	fat_physical_path;

	g_resources_manager->change_count_of_pending_mount_operations	(+1);
}

void   fs_task_mount::execute_may_destroy_this	()
{
	vfs::virtual_file_system * const vfs	=	g_resources_manager->get_vfs();

	#pragma message							(XRAY_TODO("temporary solution. Remember to make it well"))

	vfs::query_mount_arguments				args;
	if ( type() == type_mount_physical )
	{
		args							=	vfs::query_mount_arguments::mount_physical_path(
													 unmanaged_allocator(),
													 m_virtual_path,
													 m_physical_path,
													 m_descriptor.c_str(),
													 g_resources_manager->get_hdd(),
													 NULL,
													 NULL,
													 (vfs::recursive_bool)m_recursive,
													 vfs::lock_operation_try_lock,
													 m_watcher_enabled);
	}
	else
	{
		args							=	vfs::query_mount_arguments::mount_archive(
													 unmanaged_allocator(),
													 m_virtual_path,
													 m_archive_physical_path,
													 m_fat_physical_path,
													 m_descriptor.c_str(),
													 g_resources_manager->get_hdd(),
													 NULL,
													 NULL,
													 vfs::lock_operation_try_lock);
	}

	vfs::mount_result const mount_result	=	vfs::query_mount_and_wait(* vfs, args, 
													boost::bind(& resources_manager::dispatch_callbacks, g_resources_manager.c_ptr(), false));
	if ( mount_result.mount )
	{
		vfs_sub_fat_resource_ptr sub_fat_resource	=	get_sub_fat_resource(mount_result.mount);
		R_ASSERT							(sub_fat_resource);

		fs_task_unmount * const unmount_task	=	XRAY_NEW_IMPL(helper_allocator(), fs_task_unmount)
													(sub_fat_resource);
		m_mount_ptr						=	unmount_task;
	}

	on_task_ready_may_destroy_this			();
}

void   fs_task_mount::call_user_callback	()
{
	if ( m_callback )
		m_callback							(m_mount_ptr);
}

} // namespace resources
} // namespace xray