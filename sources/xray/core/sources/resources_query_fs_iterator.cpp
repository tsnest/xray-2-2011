////////////////////////////////////////////////////////////////////////////
//	Created		: 25.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_fs.h>
#include <xray/vfs/virtual_file_system.h>
#include "resources_fs_task_iterator.h"
#include "resources_manager.h"
#include "resources_fs_task_erase.h"

namespace xray {
namespace resources {

void   query_vfs_iterator 				(fs_new::virtual_path_string const &	path, 
										 query_vfs_iterator_callback const &	callback,
										 memory::base_allocator * const			allocator,
										 recursive_bool	const					recursive,
										 query_result_for_cook * const			parent)
{
	ASSERT									(callback);

	bool const try_async				=	!g_resources_manager->mount_operations_are_pending();
					
	if ( try_async )
	{
		// TRYING SYNC WAY
		vfs::virtual_file_system * const vfs	=	g_resources_manager->get_vfs();

		vfs::vfs_locked_iterator			iterator;
		vfs::result_enum const found_sync	=	vfs->try_find_sync	(
			path, 
			& iterator, 
			recursive ? vfs::find_recursively : (vfs::find_enum)0, 
			allocator	
		);

		if ( found_sync == vfs::result_success )
		{
			callback						(iterator);
			return;
		}
	}

	// ASYNC WAY
	
	fs_task_iterator * const new_task	=	XRAY_NEW_IMPL(* allocator, fs_task_iterator)
											(path, callback, recursive, allocator, parent);

	g_resources_manager->add_fs_task		(new_task);
}

struct callback_helper
{
	callback_helper						(query_vfs_iterator_callback const & callback) 
		: m_callback(callback), m_receieved_callback(false) {}

	void callback						(vfs::vfs_locked_iterator const & result)
	{
		m_callback							(result);
		m_receieved_callback			=	true;
	}

	bool received_callback				() const { return m_receieved_callback; }

private:
	bool								m_receieved_callback;
	query_vfs_iterator_callback			m_callback;
};

void   query_vfs_iterator_and_wait		(fs_new::virtual_path_string const &	path, 
										 query_vfs_iterator_callback const &	callback,
							 			 memory::base_allocator * const			allocator,
										 recursive_bool const					recursive,
										 query_result_for_cook * const			parent)
{
	callback_helper		helper				(callback);
	query_vfs_iterator						(path, 
											 boost::bind(& callback_helper::callback, & helper, _1), 
											 allocator, 
											 recursive, 
											 parent);

	while ( !helper.received_callback() )
	{
		if ( threading::g_debug_single_thread )
			resources::tick					();
		else if ( threading::current_thread_id() == g_resources_manager->resources_thread_id()	)
			g_resources_manager->resources_thread_tick	();

		dispatch_callbacks					();
	}
}

bool   convert_physical_to_virtual_path	(fs_new::virtual_path_string *			out_path, 
										 fs_new::native_path_string	const &		path,
										 pcstr									mount_descriptor)
{
	vfs::virtual_file_system * const	vfs	=	g_resources_manager->get_vfs();
	return									vfs->convert_physical_to_virtual_path(out_path, path, mount_descriptor);	
}

bool   convert_virtual_to_physical_path	(fs_new::native_path_string * const		out_path, 
										 fs_new::virtual_path_string const &	path,
										 pcstr									mount_descriptor)
{
	if ( path.length() > 1 && path[0] == physical_path_char )
	{
		* out_path						=	fs_new::native_path_string::convert(path.c_str() + 1);
		return								true;
	}

	vfs::virtual_file_system * const	vfs	=	g_resources_manager->get_vfs();
	return									vfs->convert_virtual_to_physical_path(out_path, path, mount_descriptor);	
}

void   query_erase_file 				(fs_new::native_path_string const &		opt_physical_path, 
										 fs_new::virtual_path_string const &	opt_virtual_path, 
										 query_erase_callback const &			callback, 
										 memory::base_allocator * 				allocator)
{
	fs_task_erase *	const new_task		=	XRAY_NEW_IMPL(* allocator, fs_task_erase)
		(opt_physical_path, opt_virtual_path, callback, allocator);

	g_resources_manager->add_fs_task		(new_task);
}

} // namespace resources
} // namespace xray 