////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_fs_task_erase.h"
#include "resources_manager.h"
#include <xray/linkage_helper.h>

XRAY_DECLARE_LINKAGE_ID(resources_fs_task_erase)

namespace xray {
namespace resources {

fs_task_erase::fs_task_erase			(fs_new::native_path_string const &		physical_path,
										 fs_new::virtual_path_string const &	virtual_path,
										 query_erase_callback const &			callback, 
										 memory::base_allocator * const			allocator)
	:	fs_task				(fs_task::type_erase_file, allocator),
		m_physical_path		(physical_path),
		m_virtual_path		(virtual_path),
		m_callback			(callback)
{
	LOGI_TRACE("resources", "adding erase task: '%s' '%s'", physical_path.c_str(), virtual_path.c_str());
	g_resources_manager->change_count_of_pending_mount_operations	(+1);
}

void   fs_task_erase::on_hot_unmounted	(vfs::mount_result result)
{
	unlink									(m_physical_path.c_str());
	m_result							=	true;
	on_task_ready_may_destroy_this			();
}

void   fs_task_erase::execute_may_destroy_this ()
{
	vfs::virtual_file_system * const vfs	=	g_resources_manager->get_vfs();

	if ( !m_physical_path )
	{
		R_ASSERT							(m_virtual_path);
		bool const convertion_result	=	vfs->convert_virtual_to_physical_path	
											(& m_physical_path, m_virtual_path);
		R_ASSERT							(convertion_result);

// 		fs_iterator							iterator;
// 		bool const found				=	g_fat->try_find_sync_no_pin(& iterator, & m_find_results, virtual_path.c_str());
// 		R_ASSERT_U							(found);
// 		if ( iterator )
// 			iterator.get_disk_path			(physical_path);
	}

	LOGI_INFO								("resources", "erasing file(%s)", m_physical_path.c_str());
	vfs->query_hot_unmount					(m_physical_path, 
											 boost::bind(& fs_task_erase::on_hot_unmounted, this, _1),
											 vfs::lock_operation_lock,
											 assert_on_fail_false);
}

void   fs_task_erase::call_user_callback	()
{
	if ( m_callback )
		m_callback							(m_result);
}

} // namespace resources
} // namespace xray