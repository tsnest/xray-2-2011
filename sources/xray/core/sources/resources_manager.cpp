#include "pch.h"

#include "resources_manager.h"
#include "resources_device_manager.h"
#include "resources_hdd_manager.h"
#include "resources_macros.h"
#include "resources_allocators.h"
#include "resources_thread_local_data.h"
#include <xray/resources_fs_task.h>

#include <xray/resources_resource_link.h>
#include <xray/resources_queries_result.h>

#include "game_resman.h"
#include "resources_fs_task_composite.h"
#include "vfs_sub_fat_cook.h"

namespace xray {
namespace resources {

uninitialized_reference<resources_manager>		g_resources_manager;
uninitialized_reference<ppmd_compressor>		g_resource_compressor;

command_line::key		s_check_all_threads_do_dispatch_callbacks	( "check_all_threads_do_dispatch_callbacks", "", "", "" );

//----------------------------------------------------------
// initialization / deinitialization
//----------------------------------------------------------

uninitialized_reference<hdd_manager>	s_hdd_device;

resources_manager::resources_manager (fs_new::asynchronous_device_interface &	hdd,
									  fs_new::asynchronous_device_interface &	dvd,
									  enable_fs_watcher_bool					enable_fs_watcher)
:	m_hdd			(hdd), 
	m_dvd			(dvd), 
	m_sync_device	(hdd.get_device())	
{

#if XRAY_FS_NEW_WATCHER_ENABLED
	if ( enable_fs_watcher )
		XRAY_CONSTRUCT_REFERENCE			(m_watcher, watcher)(& m_vfs);
#else
	XRAY_UNREFERENCED_PARAMETERS( enable_fs_watcher );
#endif

	XRAY_CONSTRUCT_REFERENCE				(m_compressor, ppmd_compressor) (&memory::g_cook_allocator, 1);
	m_tls_key_thread_local_data			=	threading::tls_create_key();

	m_mount_and_fs_iterator_dispatch_mutex_hold_thread_id	=	0;
	m_do_mount_mounts_path				=	false;
	m_pending_mount_operations_count	=	0;
	m_pending_mount_helper_query_count	=	0;

	m_delay_delete_unmanaged_resources_count	=	0;

	XRAY_CONSTRUCT_REFERENCE				(s_hdd_device, hdd_manager)(512);
	m_device_managers.push_back				(s_hdd_device.c_ptr());

	m_pending_queries_count				=	0;
	m_uncooked_queries_count			=	0;
	m_num_cook_registrators				=	0;
	m_count_of_pending_query_with_fat_it	=	0;
	m_fs_tasks_execute_on_current_tick	=	0;
	m_dispatching_created_resources		=	0;
	m_self_wakeuping					=	false;

	for ( u32 i=0; i<last_resource_class; ++i )
		m_cooks_registry.push_back			(NULL);

	m_resources_thread_id				=	0;

	m_flush_timer.start						();
	if ( threading::g_debug_single_thread )
	{
		m_resources_thread_id			=	threading::current_thread_id();
		m_cooker_thread_id				=	threading::current_thread_id();
	}

	m_vfs.on_mounted					=	boost::bind(& resources_manager::on_mounted, this, _1);
	register_cooks							();
}

void   resources_manager::dump_unmanaged_resource_leaks ()
{
	u32 const leaked_count				=	debug()->unmanaged_resources_count();
	if ( !leaked_count )
		return;

	LOGI_ERROR								("resources", "%d unmanaged resource(s) or user buffers LEAKED!!!", leaked_count);
	
	unmanaged_resource * it_resource	=	debug()->pop_unmanaged_resources();
	while ( it_resource )
	{
		LOGI_ERROR							("resources", "LEAKED: %s", it_resource->log_string().c_str());
		it_resource						=	debug()->next_unmanaged_resource(it_resource);
	}
}

resources_manager::~resources_manager ()
{
	if ( !threading::g_debug_single_thread )
	{
		memory::g_resources_helper_allocator.user_current_thread_id( );
		memory::g_resources_unmanaged_allocator.user_current_thread_id( );
	}

	dump_unmanaged_resource_leaks			();
	memory::g_resources_managed_allocator.dump_resource_leaks	();

	finalize_name_registry					();

	while ( thread_local_data * const tls	=	m_thread_local_data.unlink_leftmost_without_rebalance() )
		tls->delete_this					();

	XRAY_DESTROY_REFERENCE					(s_hdd_device);

	threading::tls_delete_key				(m_tls_key_thread_local_data);

	XRAY_DESTROY_REFERENCE					(m_compressor);

#if XRAY_FS_NEW_WATCHER_ENABLED
	if ( m_watcher.initialized() )
		XRAY_DESTROY_REFERENCE				(m_watcher);
#endif
}

//----------------------------------------------------------
// general functions
//----------------------------------------------------------

void   resources_manager::execute_fs_task (fs_task * task)
{
	bool const is_mount_task		=	task->is_mount_task();
	if ( is_mount_task )
	{
		while ( m_delay_delete_unmanaged_resources_count )
		{
			delete_delayed_unmanaged_resources		();
			deallocate_delayed_unmanaged_resources	();
			
			if ( threading::g_debug_single_thread )
				resources::dispatch_callbacks	();
		}
	}

	task->execute_may_destroy_this			();

	if ( is_mount_task )
		change_count_of_pending_mount_operations	(-1);
}

void   resources_manager::execute_fs_sub_tasks (fs_task * task)
{
	if ( !task )
		return;

	while ( task )
	{
		fs_task * const next_task		=	task->m_next;

		if ( task->type() == fs_task::type_mount_physical )
			ASSERT							(m_pending_mount_operations_count);

		execute_fs_task						(task);
		task							=	next_task;
	}
}

void   resources_manager::execute_fs_tasks (fs_task * task)
{
	if ( !task )
		return;

	while ( task )
	{
		fs_task * const next_task		=	task->m_next;

		if ( task->type() == fs_task::type_mount_composite )
		{
			fs_task_composite * const composite	=	static_cast_checked<fs_task_composite *>(task);
			fs_task * const children_tasks	=	composite->pop_all_and_clear();
			execute_fs_tasks				(children_tasks);
		}

		execute_fs_task						(task);

		task							=	next_task;
	}
}

//----------------------------------------------------------
// main functions
//----------------------------------------------------------

void   resources_manager::resources_thread_tick ()
{
	dispatch_devices						();
	m_vfs.dispatch_callbacks				();

#if XRAY_FS_NEW_WATCHER_ENABLED
	if ( m_watcher.initialized() )
		m_watcher->dispatch_notifications	();
#endif

	tick_game_resources_manager				();

	bool const do_init_new_queries_with_unlocked_fat_it	=	!m_pending_mount_operations_count && 
															!m_pending_mount_helper_query_count;
	
	thread_local_data * const tls		=	get_thread_local_data(threading::current_thread_id(), false);
	bool const has_fs_dispatch_callbacks	=	tls && !tls->ready_fs_tasks.empty();

	bool const has_fs_tasks				=	!m_fs_tasks.empty();
	bool const do_execute_fs_tasks		=	m_vfs.scheduled_to_unmount.empty() &&
											!m_count_of_pending_query_with_fat_it && 
											!m_pending_mount_helper_query_count &&
											!has_fs_dispatch_callbacks;
	
	init_new_autoselect_quality_queries		();

	query_result * queries_with_unlocked_fat_it		=	do_init_new_queries_with_unlocked_fat_it ? 
														m_new_queries_with_unlocked_fat_it.pop_all_and_clear() : NULL;
	query_result * const queries_with_locked_fat_it	=	m_new_queries_with_locked_fat_it.pop_all_and_clear();

	query_result * const new_inorder_queries	=	m_new_inorder_queries.pop_all_and_clear();

	fs_task * const fs_sub_tasks		=	m_fs_sub_tasks.pop_all_and_clear();

	if ( do_execute_fs_tasks || fs_sub_tasks )
		threading::interlocked_exchange		(m_fs_tasks_execute_on_current_tick, 1);

	fs_task * const fs_tasks			=	do_execute_fs_tasks ? m_fs_tasks.pop_all_and_clear() : NULL;
	if ( !fs_tasks && !fs_sub_tasks )
		threading::interlocked_exchange		(m_fs_tasks_execute_on_current_tick, 0);
	
	delete_delayed_managed_resources		();
	delete_delayed_unmanaged_resources		();
	deallocate_delayed_unmanaged_resources	();

	if ( do_execute_fs_tasks )
		execute_fs_tasks 					(fs_tasks);

	execute_fs_sub_tasks					(fs_sub_tasks);

	if ( m_fs_tasks_execute_on_current_tick )
		threading::interlocked_exchange		(m_fs_tasks_execute_on_current_tick, 0);

	if ( do_init_new_queries_with_unlocked_fat_it && !m_new_queries_waiting_for_cook_register.empty() && !cooks_are_registering() )
	{
		query_result * const new_queries_waiting_for_cook_register	=	m_new_queries_waiting_for_cook_register.pop_all_and_clear();
		if ( !queries_with_unlocked_fat_it )
			queries_with_unlocked_fat_it				=	new_queries_waiting_for_cook_register;
		else
			new_queries_list::set_next_of_object	(queries_with_unlocked_fat_it, new_queries_waiting_for_cook_register);
	}

	init_new_queries						(new_inorder_queries);

	init_new_queries						(queries_with_locked_fat_it);

	if ( do_init_new_queries_with_unlocked_fat_it )
		init_new_queries					(queries_with_unlocked_fat_it);

	dispatch_callbacks						(false);

	m_allocate_functionality.tick			(false); // must be before dispatch_allocated_raw_resources to work synchronously
	dispatch_allocated_raw_resources		();

	std::for_each							(m_device_managers.begin(), 
											 m_device_managers.end(), 
											 std::mem_fun(& device_manager::update));
	
	dispatch_created_resources				();
	dispatch_decompressed_resources			();

	save_generated_resources				();

	delete_name_registry_entries			();
	
	if ( m_flush_timer.get_elapsed_msec() >= 1000 )
	{
//		fs::flush_replications				();
		m_flush_timer.start					();
	}

	// if one or both functionalities where turned off - continue in next tick
	if ( !do_init_new_queries_with_unlocked_fat_it || (!do_execute_fs_tasks && has_fs_tasks) )
	{
		if ( !m_self_wakeuping )
		{
			m_self_wakeuping			=	true;
			m_self_wakeup_timer.start		();
		}
	
		wakeup_resources_thread				();
	}
	else
		m_self_wakeuping				=	false;
}

void   resources_manager::finalize_name_registry ()
{
	delete_name_registry_entries			();
	threading::mutex_raii	raii			(m_name_registry_mutex);
	for ( name_registry_type::iterator	it		=	m_name_registry.begin(),
										it_end	=	m_name_registry.end();
										it		!=	it_end;	)
	{
		name_registry_type::iterator	it_next	=	it;
		++it_next;
		name_registry_entry * entry		=	* it;
		RES_FREE							(entry);
		it								=	it_next;
	}
}

bool   resources_manager::resources_thread_can_exit		()
{
	return									thread_can_exit() && m_fs_tasks.empty();
}

void   resources_manager::on_resources_thread_started ()
{
	threading::interlocked_exchange			(m_resources_thread_id, threading::current_thread_id());

	ASSERT									(!threading::g_debug_single_thread);
	memory::g_resources_helper_allocator.user_current_thread_id();
	memory::g_resources_unmanaged_allocator.user_current_thread_id();

	m_flush_timer.start						();

	initialize_game_resources_manager		();

	if ( m_do_mount_mounts_path )
	{
		m_vfs.unmount_thread_id			=	threading::current_thread_id();
		do_mount_mounts_path				();
		m_do_mount_mounts_path			=	false;
	} 
}

void   resources_manager::on_resources_thread_ending ()
{
	m_mounts_ptr						=	NULL;
	m_mounts_converted_ptr				=	NULL;

	finilize_game_resources_manager			();

	while ( !resources_thread_can_exit() )
		resources_thread_tick				();
}

void   resources_manager::resources_thread_yield (u32 thread_sleep_period)
{
	m_resources_wakeup_event.wait			(thread_sleep_period);
}

void   resources_manager::wait_and_dispatch_callbacks (bool const call_from_main_thread, bool finalizing_thread)
{
	if ( !call_from_main_thread && threading::core_count() == 1 ) 
		return;

	while ( !m_wait_and_dispatch_callbacks_mutex.try_lock() )
	{
		if ( threading::g_debug_single_thread )
			resources::tick					();

		dispatch_callbacks					(true);
	}
	
	while ( !thread_can_exit() )
	{
		if ( threading::g_debug_single_thread )
			resources::tick					();
 
		dispatch_callbacks					(finalizing_thread);
	}

	m_wait_and_dispatch_callbacks_mutex.unlock	();
}

void   resources_manager::on_mounted (vfs::base_node<> * const node)
{
	R_ASSERT								(!node->get_mount_root_user_data());

	vfs_sub_fat_resource * const sub_fat	=	XRAY_NEW_IMPL(helper_allocator(), vfs_sub_fat_resource);

	sub_fat->set_deleter_object				(get_vfs_sub_fat_cook(), threading::current_thread_id());

	sub_fat->mount_ptr					=	vfs::mount_of_node(node);

	vfs::vfs_mount * parent_mount		=	sub_fat->mount_ptr->parent;
	if ( parent_mount )
	{
		R_ASSERT							(parent_mount->user_data);
		sub_fat->parent					=	(vfs_sub_fat_resource*)parent_mount->user_data;
	}
	
	sub_fat->set_creation_source			(resource_base::creation_source_created_by_user, 
											 sub_fat->mount_ptr->get_virtual_path(), 
											 memory_usage_type(unmanaged_memory, 
															   sub_fat->mount_ptr->get_mount_size()));


	node->set_mount_root_user_data			(sub_fat);

	if ( g_game_resources_manager.initialized() )
		g_game_resources_manager->capture_resource	(sub_fat);
}

} // namespace resources
} // namespace xray
