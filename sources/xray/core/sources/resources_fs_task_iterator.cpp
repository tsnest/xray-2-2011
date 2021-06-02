////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_fs_task_iterator.h"
#include <xray/vfs/virtual_file_system.h>
#include "resources_manager.h"

namespace xray {
namespace resources {

fs_task_iterator::fs_task_iterator		(fs_new::virtual_path_string const &	virtual_path, 
						 				 query_vfs_iterator_callback			callback, 
										 recursive_bool							recursive,
						 				 memory::base_allocator *				allocator,
						 				 query_result_for_cook *				parent)
	:	fs_task						(type_fs_iterator, allocator, parent),
		m_callback					(callback),
		m_virtual_path				(virtual_path),
		m_in_sync_call				(false),
		m_recursive					(recursive),
		m_vfs_result				(vfs::result_fail),
		m_iterator_ready			(false),
		m_parent_query				(NULL)
{
}

void   fs_task_iterator::try_async_query	()
{
	#pragma message							(XRAY_TODO("Change to helper to unmanaged allocator when you can"))
	vfs::virtual_file_system * const vfs	=	g_resources_manager->get_vfs();

	vfs->try_find_async						(m_virtual_path.c_str(),
											 boost::bind(& fs_task_iterator::on_vfs_iterator_ready, this, _1, _2),
											 m_recursive ? vfs::find_recursively : (vfs::find_enum)0, 
											 unmanaged_allocator());
}

void   fs_task_iterator::on_vfs_iterator_ready	(vfs::vfs_locked_iterator const & iterator, vfs::result_enum result)
{
	m_vfs_result						=	result;
	if ( result == vfs::result_cannot_lock )
	{
		if ( !m_in_sync_call )
			try_async_query					();
		return;
	}

	R_ASSERT								(result != vfs::result_out_of_memory, "Not yet implemented. Call Lain");
	m_iterator.grab							(iterator);
	m_iterator_ready					=	true;

	if ( !m_in_sync_call )
		on_task_ready_may_destroy_this		();
}

void   fs_task_iterator::execute_may_destroy_this ()
{
	#pragma message (XRAY_TODO("temporary solution. Remember to make it well with new task queues"))
	m_in_sync_call						=	true;
	do 
	{
		try_async_query						();

	} while ( m_vfs_result == vfs::result_cannot_lock );
	m_in_sync_call						=	false;

	if ( m_iterator_ready )
		on_task_ready_may_destroy_this		();
}

void   fs_task_iterator::call_user_callback	()
{
	R_ASSERT								(m_iterator_ready);
	m_callback								(m_iterator);
}

} // namespace resources
} // namespace xray
