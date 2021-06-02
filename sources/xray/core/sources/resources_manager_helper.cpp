////////////////////////////////////////////////////////////////////////////
//	Created		: 14.06.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/native_path_string.h>
#include <xray/fs/device_utils.h>
#include "resources_manager.h"
#include "resources_device_manager.h"
#include "resources_thread_local_data.h"
#include "vfs_sub_fat_cook.h"
#include "configs_binary_config_cook.h"

#ifndef MASTER_GOLD
	#include "configs_ltx_config_cook.h"
	#include "configs_lua_config_cook.h"
#endif  //MASTER_GOLD

namespace xray {
namespace resources {

static vfs_sub_fat_cook					s_sub_fat_cook;

class unknown_data_class_cook : public translate_query_cook
{
public:
	unknown_data_class_cook() 
		: translate_query_cook(unknown_data_class, reuse_false, use_any_thread_id) {}

	virtual	void			translate_query			(resources::query_result_for_cook &	parent)
	{
		parent.finish_query	(result_error);
	}

	virtual void			delete_resource			(resources::resource_base *)
	{
	}
};

void   resources_manager::register_cooks ()
{
	get_binary_config_cook					();

#ifndef MASTER_GOLD
	static core::configs::ltx_config_cook	ltx_cook;
	register_cook							(& ltx_cook);

	get_lua_config_cook						();
#endif //MASTER_GOLD

	register_cook							(& s_sub_fat_cook);

	static unknown_data_class_cook			unknown_data_cook;
	register_cook							(& unknown_data_cook);	
}

vfs_sub_fat_cook *		get_vfs_sub_fat_cook	()
{
	return									& s_sub_fat_cook;
}

#ifndef MASTER_GOLD

xray::core::configs::lua_config_cook& resources_manager::get_lua_config_cook	()
{
	static xray::core::configs::lua_config_cook	cook;
	static bool initialized					= false;
	if ( !initialized )
	{
		initialized							= true;
		register_cook						(&cook);
	}

	return									cook;
}

#endif //MASTER_GOLD



void   resources_manager::do_mount_mounts_path ()
{
	vfs::query_mount_arguments	mounts_args	=	vfs::query_mount_arguments::mount_physical_path
												(unmanaged_allocator(), 
												 "mounts", 
												 m_mounts_path, 
												 "mounts", 
												 & m_hdd, 
												 NULL, 
												 NULL, 
												 vfs::recursive_false,
												 vfs::lock_operation_try_lock);

	vfs::mount_result const result		=	vfs::query_mount_and_wait(m_vfs, mounts_args, 
												boost::bind(& resources_manager::dispatch_callbacks, g_resources_manager.c_ptr(), false));

	vfs_sub_fat_resource_ptr sub_fat_resource	=	get_sub_fat_resource(result.mount);
	R_ASSERT								(sub_fat_resource);

	m_mounts_ptr						=	XRAY_NEW_IMPL(helper_allocator(), fs_task_unmount)
												(sub_fat_resource);

	fs_new::native_path_string				mounts_converted_path;
	mounts_converted_path.assignf_with_conversion	("%s/platforms/%s", m_mounts_path.c_str(), platform::platform_name());

	fs_new::create_folder_r					(get_synchronous_device(), mounts_converted_path, true);

	vfs::query_mount_arguments	mounts_converted_args	=	
											vfs::query_mount_arguments::mount_physical_path
												(unmanaged_allocator(), 
												 "mounts.converted", 
												 mounts_converted_path, 
												 "mounts.converted", 
												 & m_hdd, 
												 NULL, 
												 NULL, 
												 vfs::recursive_false,
												 vfs::lock_operation_try_lock);

	vfs::mount_result const result_converted	=	vfs::query_mount_and_wait(m_vfs, mounts_converted_args, 
														boost::bind(& resources_manager::dispatch_callbacks, g_resources_manager.c_ptr(), false));

	vfs_sub_fat_resource_ptr converted_sub_fat_resource	=	get_sub_fat_resource(result_converted.mount);
	R_ASSERT								(converted_sub_fat_resource);

	m_mounts_converted_ptr				=	XRAY_NEW_IMPL(helper_allocator(), fs_task_unmount)
												(converted_sub_fat_resource);
}

xray::core::configs::binary_config_cook & resources_manager::get_binary_config_cook	()
{
	static bool initialized								= false;
	static xray::core::configs::binary_config_cook		cook;
	static xray::core::configs::binary_config_cook_impl	cook2;
	if ( !initialized ) 
	{
		initialized							= true;
		register_cook						(&cook);
		register_cook						(&cook2);
	}

	return									cook;
}

threading::event &   resources_manager::get_resources_wakeup_event () 
{ 
	return									m_resources_wakeup_event; 
}

bool   resources_manager::need_replication (pcstr const file_path)
{
	XRAY_UNREFERENCED_PARAMETER				(file_path);
	//^ if ( is_hdd path ... )
	return									false;
}

device_manager *   resources_manager::find_capable_device_manager (pcstr file_path)
{
	for ( device_managers::iterator		it	=	m_device_managers.begin();
										it	!=	m_device_managers.end();
										++it )
	{
		device_manager*	manager			=	*it;
		if ( manager->can_handle_query		(file_path) )
			return							manager;
	}

	return									NULL;
}

void   resources_manager::add_fs_task (fs_task * task)
{
	if ( task->is_helper_query_for_mount() || task->is_sub_request() )
	{
		m_fs_sub_tasks.push_back			(task);
	}
	else
		m_fs_tasks.push_back				(task);

	wakeup_resources_thread					();
}

void   resources_manager::finalize_thread_usage (bool const call_from_main_thread)
{
	this->wait_and_dispatch_callbacks			(call_from_main_thread, true);
}

void   resources_manager::wakeup_resources_thread ()
{
	m_resources_wakeup_event.set				(true);
}

void   resources_manager::wakeup_cooker_thread ()
{
	m_cooker_wakeup_event.set					(true);
}

void   resources_manager::wakeup_thread_by_id_if_needed (threading::thread_id_type const thread_id)
{
	R_ASSERT									(thread_id < u32(-10));
	if ( thread_id == (u32)m_resources_thread_id )
		wakeup_resources_thread					();
	else if ( thread_id == (u32)m_cooker_thread_id )
		wakeup_cooker_thread					();
#if	XRAY_FS_NEW_WATCHER_ENABLED

#pragma message(XRAY_TODO("wake up watcher thread here correctly"))
//	else if ( is_watcher_enabled() && is_watcher_initialized() && thread_id == fs::watcher_thread_id() )
//		m_vfs->wakeup_watcher_thread			();

#endif
}

void   resources_manager::push_generated_resource_to_save (query_result * in_query)
{
	m_generated_resources_to_save_list.push_back	(in_query);
	wakeup_resources_thread							();
}

void   resources_manager::save_generated_resource (query_result * query)
{
	pcstr const physical_path			=	query->get_save_generated_data()->get_physical_path();
	device_manager * const manager		=	find_capable_device_manager(physical_path);
	if ( !manager )
	{
		R_ASSERT							("resources", "cannot save generated resource", detail::make_query_logging_string(query).c_str());
		query->finish_query					(query->get_create_resource_result());
		return;
	}

	query_result * const save_query		=	RES_NEW(query_result)(0, NULL, 0, 0);
	save_query->init_save					(query->grab_save_generated_data(), query);
	
	manager->push_query						(save_query);
	wakeup_resources_thread					();
}

void   resources_manager::save_generated_resources ()
{
	for ( query_result *	it_query	=	m_generated_resources_to_save_list.pop_all_and_clear();
							it_query;
							it_query	=	m_generated_resources_to_save_list.get_next_of_object(it_query) )
	{
		save_generated_resource				(it_query);
	}
}

void   resources_manager::change_count_of_pending_query_with_fat_it (long change)
{
	R_ASSERT								(change == +1 || change == -1);
	if ( change == -1 )
		R_ASSERT							(m_count_of_pending_query_with_fat_it > 0);
	threading::interlocked_exchange_add		(m_count_of_pending_query_with_fat_it, change);
}

void   resources_manager::push_name_registry_to_delete (name_registry_entry * entry)
{ 
	threading::mutex_raii		raii		(m_name_registry_mutex);
	m_name_registry.erase					(entry);
	m_name_registry_delete_queue.push_back	(entry);
	wakeup_resources_thread					();
}

void   resources_manager::delete_name_registry_entries ()
{
	name_registry_entry * it_entry		=	m_name_registry_delete_queue.pop_all_and_clear();
	while ( it_entry )
	{
		name_registry_entry * it_next	=	m_name_registry_delete_queue.get_next_of_object(it_entry);
		RES_FREE							(it_entry);
		it_entry						=	it_next;
	}
}

void   resources_manager::on_query_finished_callback (resource_base * resource)
{
	if ( m_query_finished_callback )
		m_query_finished_callback				(resource);
}

thread_local_data*   resources_manager::get_thread_local_data (threading::thread_id_type const thread_id, bool create_if_not_exist)
{
	thread_local_data *	local_data		=	NULL;
	
	if ( threading::current_thread_id() == thread_id )
	{
		local_data						=	(thread_local_data *)threading::tls_get_value(m_tls_key_thread_local_data);
		if ( local_data )
			return							local_data;
	}

	threading::lock_type_enum const lock_type	=	create_if_not_exist ? threading::lock_type_write : threading::lock_type_read;
	m_thread_local_data_lock.lock			(lock_type);

	thread_local_data_tree::iterator it	=	m_thread_local_data.find(thread_id,thread_local_data_compare());

 	if ( it != m_thread_local_data.end() )
 	{
 		local_data						=	& * it;
 	}
 	else
 	{
 		if ( create_if_not_exist )
 		{
 			if ( threading::current_thread_id() == (u32)m_resources_thread_id )
 				local_data				=	RES_NEW(thread_local_data)(thread_id, & memory::g_resources_helper_allocator);
 			else
 				local_data				=	MT_NEW(thread_local_data)(thread_id, & memory::g_mt_allocator);
 
 			m_thread_local_data.insert		(* local_data);
 		}
 	}
 
 	if ( threading::current_thread_id() == thread_id && local_data )
 		threading::tls_set_value			(m_tls_key_thread_local_data, local_data);

	m_thread_local_data_lock.unlock			(lock_type);

	return									local_data;
}

void   resources_manager::on_query_finished (queries_result * query)
{
	thread_local_data *	local_data		=	get_thread_local_data(query->m_thread_id, true);
	local_data->finished_queries.push_back	(query);

	if ( query->m_thread_id == (u32)m_cooker_thread_id )
		wakeup_cooker_thread				();
	if ( query->m_thread_id == (u32)m_resources_thread_id )
		wakeup_resources_thread				();
}

void   resources_manager::change_count_of_pending_helper_query_for_mount (long change)
{
if ( change == +1 )
	{
		threading::interlocked_increment	(m_pending_mount_helper_query_count);
	}
	else if ( change == -1 )
	{
		R_ASSERT							(m_pending_mount_helper_query_count);
		threading::interlocked_decrement	(m_pending_mount_helper_query_count);		
	}
	else
		NOT_IMPLEMENTED						();
}

void   resources_manager::change_count_of_pending_mount_operations (long change)
{
	if ( change == +1 )
	{
		threading::interlocked_increment	(m_pending_mount_operations_count);
	}
	else if ( change == -1 )
	{
		R_ASSERT							(m_pending_mount_operations_count);
		threading::interlocked_decrement	(m_pending_mount_operations_count);		
	}
	else
		NOT_IMPLEMENTED						();
}

struct mount_one_folder_and_wait_helper
{
	mount_one_folder_and_wait_helper () : callback_called(false), result(false) {}

	void	callback (bool success)
	{
		result							=	success;
		callback_called					=	true;
	}
	bool	result;
	bool	callback_called;
};

//----------------------------------------------------------
// replications
//----------------------------------------------------------

bool   resources_manager::replicate_resource (vfs::vfs_iterator		fat_it, 
											  managed_resource_ptr	resource,
											  query_callback		callback)
{
	XRAY_UNREFERENCED_PARAMETERS			(fat_it, resource, callback);
	NOT_IMPLEMENTED							(return false);

#if 0
	XRAY_UNREFERENCED_PARAMETER				(callback);
	path_string	file_path				=	fat_it.get_physical_path();
	path_string	replicate_path;
	
	g_fat->replicate_path					(file_path.c_str(), replicate_path);

	device_manager*	manager				=	find_capable_device_manager(replicate_path.c_str());
	R_ASSERT								(manager);
	if ( !manager )
		return								false;

	// note: data must live till store_callback is called
	query_result* const	 replication_query	=	RES_NEW(query_result)(0, NULL, 0, 0);

	replication_query->init_replication		(fat_it, resource);
	
	manager->push_query						(replication_query);
	wakeup_resources_thread					();
	return									true;
#endif // #if 0
}

} // namespace resources
} // namespace xray


