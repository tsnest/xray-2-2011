////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.02.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_fs_impl.h"
#include "resources_manager.h"
#include <xray/resources_fs.h>

namespace xray {
namespace resources {

using namespace fs;

fs_task::fs_task (type_enum const type) :	fs_iterator_path(logical_path),
											fat_file_path(disk_dir_path),
											physical_path(disk_dir_path),
											old_physical_path(disk_dir_path),
											new_physical_path(db_file_path),
											type(type), 
											need_replication(false),
											watch_folder(true),
											reverse_byte_order(false),
											mount_callback(NULL),
											fs_iterator_callback(NULL),
											allocator(NULL),
											thread_id(threading::current_thread_id()),
											result(false),
											continue_mount_disk(false),
											fs_iterator_recursive(false),
											fs_iterator_ready(false),
											parent(NULL),
											m_find_results(find_results_struct::type_pin),
											call_from_get_path_info(false)
{
	if ( is_mount_task() )
		g_resources_manager->change_count_of_pending_mount_operations(+1);
}


void   query_update_disk_node_name (fs::file_change_info const &	file_change_info, 
									query_mount_callback const 		callback, 
									memory::base_allocator * const 	allocator)
{
	fs_task *	new_task			=	XRAY_NEW_IMPL(* allocator, fs_task)(fs_task::type_update_disk_node_name);
#ifndef MASTER_GOLD
	new_task->file_change_info		=	file_change_info;
#endif // #ifndef MASTER_GOLD

	new_task->old_physical_path		=	file_change_info.physical_renamed_old_file_path;
	new_task->new_physical_path		=	file_change_info.physical_renamed_new_file_path;
	new_task->allocator				=	allocator;
	new_task->mount_callback		=	callback;

	g_resources_manager->add_fs_task		(new_task);
}

void   query_mount_disk_node (fs::file_change_info const & file_change_info, query_mount_callback const callback, memory::base_allocator * const allocator)
{
	fs_task *	new_task			=	XRAY_NEW_IMPL(* allocator, fs_task)(fs_task::type_mount_disk_node);

#ifndef MASTER_GOLD
	new_task->file_change_info		=	file_change_info;
#endif // #ifndef MASTER_GOLD

	new_task->physical_path			=	file_change_info.physical_file_path;
	new_task->allocator				=	allocator;
	new_task->mount_callback		=	callback;

	g_resources_manager->add_fs_task		(new_task);
}

void   query_unmount_disk_node (fs::file_change_info const & file_change_info, query_mount_callback const callback, memory::base_allocator * const allocator)
{
	fs_task *	new_task			=	XRAY_NEW_IMPL(* allocator, fs_task)(fs_task::type_unmount_disk_node);

#ifndef MASTER_GOLD
	new_task->file_change_info		=	file_change_info;
#endif // #ifndef MASTER_GOLD

	new_task->physical_path			=	file_change_info.physical_file_path;
	new_task->allocator				=	allocator;
	new_task->mount_callback		=	callback;

	g_resources_manager->add_fs_task		(new_task);
}

void   query_update_file_size_in_fat (fs::file_change_info const & file_change_info, query_mount_callback const callback, memory::base_allocator * const allocator)
{
	fs_task *	new_task			=	XRAY_NEW_IMPL(* allocator, fs_task)(fs_task::type_update_file_size_in_fat);
#ifndef MASTER_GOLD
	new_task->file_change_info		=	file_change_info;
#endif // #ifndef MASTER_GOLD

	new_task->physical_path			=	file_change_info.physical_file_path;
	new_task->allocator				=	allocator;
	new_task->mount_callback		=	callback;

	g_resources_manager->add_fs_task		(new_task);
}

void   query_erase_file (pcstr const	opt_physical_path, 
						 pcstr const	opt_logical_path, 
						 query_mount_callback const callback, 
						 memory::base_allocator * const allocator)
{
	fs_task *	new_task			=	XRAY_NEW_IMPL(* allocator, fs_task)(fs_task::type_erase_file);

	new_task->logical_path			=	opt_logical_path;
	new_task->physical_path			=	opt_physical_path;
	new_task->allocator				=	allocator;
	new_task->mount_callback		=	callback;

	g_resources_manager->add_fs_task		(new_task);
}

void   fs_task::on_fs_iterator_ready ()
{
	thread_local_data * const local_data	=	g_resources_manager->get_thread_local_data(thread_id, true);
	fs_iterator_ready				=	true;
	local_data->ready_fs_tasks.push_back	(this);
	g_resources_manager->wakeup_thread_by_id_if_needed	(thread_id);
}

bool   fs_task::execute_may_destroy_this ()
{
	bool result						=	true;
	if ( type == type_fs_iterator_task )
	{
		m_find_results.callback		=	boost::bind(& fs_task::on_fs_iterator_ready, this);
		g_fat->find_async				(& m_find_results,
										 fs_iterator_path.c_str(),
										 fs_iterator_recursive ? find_recursively : 0);
		return							true;
	}
	else if ( type == type_mount_disk )
	{
		if ( continue_mount_disk )
			g_fat->continue_mount_disk	(logical_path.c_str(), (file_system::is_recursive_bool)fs_iterator_recursive);
		else
			g_fat->mount_disk			(logical_path.c_str(), 
										 disk_dir_path.c_str(), 
										 (file_system::watch_directory_bool)watch_folder,
										 (file_system::is_recursive_bool)fs_iterator_recursive);
	}
	else if ( type == type_mount_db )
	{
		result						=	g_fat->mount_db(logical_path.c_str(), 
														fat_file_path.c_str(), 
														db_file_path.c_str(), 
														need_replication);
	}
	else if ( type == type_unmount_disk )
	{
		g_fat->unmount_disk				(logical_path.c_str(), disk_dir_path.c_str());
	}
	else if ( type == type_unmount_db )
	{
		g_fat->unmount_db				(logical_path.c_str(), fat_file_path.c_str());
	}
	else if ( type == type_erase_file )
	{
		if ( !physical_path.length() )
		{
			R_ASSERT					(logical_path.length());
			fs_iterator					iterator;
			bool const found		=	g_fat->try_find_sync_no_pin(& iterator, & m_find_results, logical_path.c_str());
			R_ASSERT_U					(found);
			if ( iterator )
				iterator.get_disk_path	(physical_path);
		}

		LOGI_INFO						("fs", "erase file(%s)", physical_path.c_str());
		if ( physical_path.length() )
			g_fat->mark_disk_node_as_erased	(physical_path.c_str());
		unlink							(physical_path.c_str());

	#ifndef MASTER_GOLD
		file_change_info.type		=	fs::file_change_info::change_type_removed;
	#endif // #ifndef MASTER_GOLD

	}
	else if ( type == type_mount_disk_node )
	{
		g_fat->mount_disk_node_by_physical_path	(physical_path.c_str());
	}
	else if ( type == type_unmount_disk_node )
	{
		g_fat->mark_disk_node_as_erased	(physical_path.c_str());
	}
	else if ( type == type_update_file_size_in_fat )
	{
#ifndef MASTER_GOLD
		g_fat->update_file_size_in_fat	(physical_path.c_str(), file_change_info.do_resource_deassociation);
#endif
	}
	else if ( type == type_update_disk_node_name )
	{
		bool do_resource_deassociation	=	false;
#ifndef MASTER_GOLD
		do_resource_deassociation		=	file_change_info.do_resource_deassociation;
#endif

		g_fat->rename_disk_node			(old_physical_path.c_str(), new_physical_path.c_str(), do_resource_deassociation);
	}
	else if ( type == type_mount_composite )
	{
		// thats ok, just do nothing
	}
	else
		UNREACHABLE_CODE();

#ifndef MASTER_GOLD
	if ( file_change_info.type != fs::file_change_info::change_type_unset )
		fs::notify_subscribers			(file_change_info);
#endif // #ifndef MASTER_GOLD

	return								result;
}

void   fs_task::call_user_callback ()
{
	R_ASSERT							(type != type_undefined);
	if ( type == type_fs_iterator_task )
	{
		R_ASSERT						(fs_iterator_ready);
		fs_iterator_callback			(m_find_results.pin_iterator);
	}
	else
	{
		if ( mount_callback )
			mount_callback				(result);
	}
}

bool   convert_physical_to_logical_path (path_string * out_logical_path, pcstr const physical_path)
{
	return								g_fat.initialized() && 
										g_fat->convert_physical_to_logical_path (out_logical_path, physical_path);
}

bool   convert_logical_to_physical_path	(fs::path_string * out_physical_path, pcstr const logical_path)
{
	return								g_fat->get_disk_path_to_store_file(logical_path, out_physical_path);
}


} // namespace resources
} // namespace xray