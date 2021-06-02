////////////////////////////////////////////////////////////////////////////
//	Created		: 28.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_query_result.h>
#include <xray/fs/path_string_utils.h>
#include "resources_manager.h"
#include "vfs_sub_fat_cook.h"
#include "game_resman.h"

namespace xray {
namespace resources {

bool   query_result::is_translate_query () const
{
	return									!!cook_base::find_translate_query_cook(m_class_id);
}

bool   query_result::is_fs_iterator_query () const
{
	return									(m_class_id == fs_iterator_class) || (m_class_id == fs_iterator_recursive_class);
}

void   query_result::translate_request_path ()
{
	cook_base * const	cook			=	g_resources_manager->find_cook(m_class_id);
	if ( !cook )
		return;

	fs_new::virtual_path_string				new_path;
	cook->translate_request_path			(get_requested_path(), new_path);

	if ( new_path != get_requested_path() )
	{
		m_request_path					=	(pstr)XRAY_ALLOC_IMPL(* m_user_allocator, char, new_path.length()+1);
		strings::copy						(m_request_path, new_path.length()+1, new_path.c_str());
		set_flag							(flag_should_free_request_path);
	}
}

//-----------------------------------------------
// misc code
//-----------------------------------------------

fs_new::native_path_string   query_result::absolute_physical_path	(assert_on_fail_bool const assert_on_fail) 
{
	XRAY_UNREFERENCED_PARAMETER			(assert_on_fail);

	fs_new::native_path_string				physical_path;
	if ( is_save_type() )
	{
		return								get_save_generated_data()->get_physical_path();
	}
	else
	{
		vfs::vfs_iterator const it		=	get_fat_it();
		physical_path					=	it.get_physical_path();
	}

	R_ASSERT								(physical_path.length() || !assert_on_fail);

	if ( is_replication_type() )
		{;}
#pragma message(XRAY_TODO("when implementing replication, make here a case"))

	if ( !physical_path.length() )
		return								physical_path;

	fs_new::native_path_string				absolute_path;
	bool const convertion_result		=	fs_new::convert_to_absolute_path(& absolute_path, 
																			 physical_path, 
																			 assert_on_fail);

	R_ASSERT_U								(convertion_result);
	return									absolute_path;
}

void   on_hot_mounted					(bool * out_result, vfs::mount_result result)
{
	* out_result						=	(result.result == vfs::result_success);
}

vfs_sub_fat_resource *	get_node_sub_fat	(vfs::base_node<> * const node)
{
	if ( !node )
		return								NULL;

	return									(vfs_sub_fat_resource *)node->get_mount_root_user_data();
}

bool   vfs_sub_fat_resource_is_created	(vfs::vfs_iterator const & it)
{
	if ( it.get_node() && !get_node_sub_fat(it.get_node()) )
		return								false;

	return									true;
}

void   query_result::on_request_iterator_ready (vfs::vfs_iterator & it, bool try_sync_way_only)
{
	if ( !has_flag(flag_processed_request_path) )
	{
		R_ASSERT							(!it.get_link_target());

		m_fat_it						=	it;
		m_sub_fat						=	get_node_sub_fat(it.get_node());

		lock_fat_it							();
		set_flag							(flag_processed_request_path);
	}

	if ( !try_sync_way_only )
		g_resources_manager->continue_init_new_query	(* this);
}

bool   query_result::process_request_path	(bool const try_sync_way_only)
{
	if ( has_flag(flag_processed_request_path) )
	{
		on_request_iterator_ready			(m_fat_it, try_sync_way_only);
		return								true;
	}

	unlock_fat_it							();

	pstr cur_path						=	(pstr)get_requested_path();

	if ( * cur_path == physical_path_char )
	{
		memory::base_allocator * const allocator	=	( threading::current_thread_id() == m_user_thread_id ) ?
														 m_user_allocator : & memory::g_mt_allocator;

		fs_new::asynchronous_device_interface * const async_device	=	g_resources_manager->get_hdd();
		fs_new::synchronous_device_interface	device	(async_device, allocator);
		
		vfs::base_node<> * const temp_node	=	vfs::create_temp_physical_node(device, 
													fs_new::native_path_string::convert(cur_path + 1), 
													allocator);

		m_fat_it						=	vfs::vfs_iterator(temp_node, NULL, NULL);

		set_flag							(flag_uses_physical_path);
		on_request_iterator_ready			(m_fat_it, try_sync_way_only);
		return								true;
	}

	vfs::virtual_file_system * const file_system	=	g_resources_manager->get_vfs();
	if ( get_create_resource_result() == cook_base::result_requery )
	{ 
		vfs::query_hot_mount_and_wait		(* file_system, fs_new::virtual_path_string(cur_path), NULL, helper_allocator());
	}

	if ( try_sync_way_only )
	{
		memory::base_allocator * const find_allocator	=	(threading::current_thread_id() == m_user_thread_id) ?
															 m_user_allocator : helper_allocator();
		vfs::vfs_locked_iterator			it;
		vfs::result_enum const sync_result	=	file_system->try_find_sync	(cur_path, & it, vfs::find_file_only, find_allocator);

		if ( sync_result == vfs::result_success )
		{
			if ( threading::current_thread_id() != g_resources_manager->resources_thread_id() &&
				 !vfs_sub_fat_resource_is_created(it) )
			{
				// associated sub fat resource died, we will recreate it from resources manager thread
				return						false;
			}

			on_request_iterator_ready		(it, try_sync_way_only);
			return							true;
		}

		return								false;
	}

	vfs::vfs_locked_iterator				it;
	vfs::find_async_and_wait				(* file_system, cur_path, & it, vfs::find_file_only, unmanaged_allocator(), 
											 boost::bind(& resources_manager::dispatch_devices, g_resources_manager.c_ptr()));

	on_request_iterator_ready				(it, false);
	return									true;
}

void   query_result::lock_fat_it		()
{
	if ( m_fat_it.is_end() )
		return;

	R_ASSERT								(!has_flag(flag_locked_fat_iterator));
	set_flag								(flag_locked_fat_iterator);
	g_resources_manager->change_count_of_pending_query_with_fat_it (+1);
}

void   query_result::unlock_fat_it		()
{
	if ( m_fat_it.is_end() )
	{
		R_ASSERT							(!has_flag(flag_locked_fat_iterator));
		return;
	}

	R_ASSERT								(has_flag(flag_locked_fat_iterator));
	unset_flag								(flag_locked_fat_iterator);
	g_resources_manager->change_count_of_pending_query_with_fat_it (-1);
}

void   query_result::late_set_fat_it	(vfs::vfs_iterator new_it)
{
	if ( m_fat_it == new_it )
		return;

	m_sub_fat							=	get_node_sub_fat(new_it.get_node());

	R_ASSERT								(m_fat_it.is_end());
	R_ASSERT								(!has_flag(flag_locked_fat_iterator));
	m_fat_it							=	new_it;

	set_flag								(flag_locked_fat_iterator);
	g_resources_manager->change_count_of_pending_query_with_fat_it (+1);

	if ( m_raw_managed_resource )
		m_raw_managed_resource->late_set_fat_it	(new_it);

	if ( m_managed_resource )
		m_managed_resource->late_set_fat_it	(new_it);

	if ( m_unmanaged_resource )
		m_unmanaged_resource->late_set_fat_it	(new_it);
}

} // namespace resources
} // namespace xray
