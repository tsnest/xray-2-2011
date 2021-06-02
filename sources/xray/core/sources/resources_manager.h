////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_H_INCLUDED
#define RESOURCES_H_INCLUDED

#include <xray/resources_resource.h>
#include <xray/resources_callbacks.h>
#include <xray/resources.h>
#include <xray/intrusive_list.h>
#include <xray/intrusive_double_linked_list.h>
#include <xray/limited_intrusive_list.h>
#include <xray/resources_cook_classes.h>
#include <xray/threading_event.h>
#include <xray/compressor_ppmd.h>
#include <xray/vfs/virtual_file_system.h>
#include <xray/fs/asynchronous_device_interface.h>
#include "resources_helper.h"
#include "resources_impl.h"
#include "resources_thread_local_data.h"
#include "resources_manager_watcher.h"
#include "resources_manager_debug.h"
#include "resources_manager_allocation.h"

namespace xray {
namespace core {
	namespace configs {
		class lua_config_cook;
		class binary_config_cook;
		class binary_config_cook_impl;
	} // namespace configs
} // namespace core

namespace resources {

class		device_manager;
class		fs_task;
class		thread_local_data;

class resources_manager
{
public:
								resources_manager					(fs_new::asynchronous_device_interface &	hdd,																	 
																	 fs_new::asynchronous_device_interface &	dvd,
																	 enable_fs_watcher_bool						enable_fs_watcher);

								~resources_manager					();
								
	void						on_resources_thread_started			();
	void						resources_thread_tick				();
	void						resources_thread_yield				(u32 thread_sleep_period);
	void						on_resources_thread_ending			();

	void						cooker_thread_tick					();
	void						cooker_thread_yield					(u32 thread_sleep_period);
	void						dispatch_callbacks					(bool finaling_thread);

	long						query_resources_impl				(query_resource_params const & params);
	static queries_result *		create_queries_result				(query_resource_params const & params);
	void						query_resources_by_mask				(query_resource_params const & params);

	void						push_new_query						(query_result * query);
	void						start_query_transaction				();
	void						end_query_might_destroy_this_transaction				();

	threading::event &			get_resources_wakeup_event			();

	static bool					need_replication					(pcstr file_path);
	device_manager *			find_capable_device_manager 		(pcstr file_path);

	void						init_new_queries						(query_result * new_query_list);
	void						push_delayed_delete_managed_resource	(managed_resource * res);
	void						push_delayed_delete_unmanaged_resource	(unmanaged_resource* res);
	void						delete_delayed_managed_resources		();
	void						delete_unmanaged_resource								(unmanaged_resource * dying_resource);

	void						deallocate_unmanaged_resource			(unmanaged_resource_buffer * resource_buffer);
	void						delete_delayed_unmanaged_resource_one	(unmanaged_resource * dying_resource);
	void						delete_delayed_unmanaged_resources		();
	void						deallocate_delayed_unmanaged_resources	();
	void						after_resource_deleted				(cook_base * cook, bool was_delay_delete, query_result * const destruction_observer, memory_usage_type const & memory_usage, class_id_enum class_id, pcstr request_name);

	void						init_new_query						(query_result &		result);
	void							continue_init_new_query			(query_result &		query);

	bool						replicate_resource  				(vfs::vfs_iterator		fat_it,
																	 managed_resource_ptr	resource,
																	 query_callback			callback);

	void						on_query_finished					(queries_result *);

	void						add_fs_task							(fs_task *			mount_task);

	bool						resources_thread_can_exit			();
	u32							resources_thread_id					() const { return m_resources_thread_id; }

	thread_local_data *			get_thread_local_data				(threading::thread_id_type	thread_id,
																	 bool			create_if_not_exist);

	void						finalize_thread_usage				(bool 			call_from_main_thread);
	void						wait_and_dispatch_callbacks			(bool 			call_from_main_thread, 
																	 bool 			finalizing_thread);

	void						register_cook						(cook_base *	cook);
	cook_base *					unregister_cook						(class_id_enum	class_id);

	void						wakeup_resources_thread				();
	void						wakeup_cooker_thread				();
	void						wakeup_thread_by_id_if_needed					(threading::thread_id_type const thread_id);

	u32							num_device_managers					() const	{ return m_device_managers.size(); }

	void						start_cooks_registration			();
	void						finish_cooks_registration			();

	u32							cooker_thread_id					() const	{ return m_cooker_thread_id; }

	core::configs::lua_config_cook &	get_lua_config_cook			();
	core::configs::binary_config_cook &	get_binary_config_cook		();

	void						mount_mounts_path					(pcstr mount_path);

	void						set_query_finished_callback			(query_finished_callback callback) { m_query_finished_callback  = callback; }
		
	pvoid						allocate_unmanaged_memory			(u32 size, pcstr type_name);
	managed_resource *			allocate_managed_resource			(u32 size, class_id_enum class_id);
	void						free_managed_resource				(managed_resource * );
	u32							pending_queries_count				() const { return m_pending_queries_count; }
	u32							uncooked_queries_count				() const { return m_uncooked_queries_count; }

	vfs::virtual_file_system *	get_vfs								() { return & m_vfs; }
	fs_new::asynchronous_device_interface *	get_hdd					() const { return & m_hdd; }
	fs_new::asynchronous_device_interface *	get_dvd					() const { return & m_dvd; }
	fs_new::synchronous_device_interface &	get_synchronous_device	() { return m_sync_device; }

	void						dispatch_devices					();
	debug_state *				debug								() { return & m_debug; }
	allocate_functionality *	allocate							() { return & m_allocate_functionality; }
	
#if XRAY_FS_NEW_WATCHER_ENABLED
	watcher *					get_watcher							() { return m_watcher.c_ptr(); }
#endif

	bool						mount_operations_are_pending		() const { return m_pending_mount_operations_count || m_pending_mount_helper_query_count; }

private:
	void						register_cooks						();
	void						init_query_with_no_fat_it			(query_result &		query);
	void						add_to_generate_if_no_file_queue	(query_result &		query);
	void						remove_from_generate_if_no_file_queue	(query_result &		query);

	void						push_to_device_manager				(query_result &		query);
	void						dispatch_query_callbacks			(queries_result *	ready_query_list, 
																	 bool const			finalizing_thread);

	void						dispatch_fs_tasks_callbacks			(fs_task *			ready_fs_tasks, 
																	 bool const			finalizing_thread);

	bool						thread_can_exit						();
	threading::atomic32_type	m_fs_tasks_execute_on_current_tick;
	void						execute_fs_task						(fs_task * task);
	void						execute_fs_tasks 					(fs_task * task);
	void						execute_fs_sub_tasks				(fs_task * task);

	//----------------------------------------------------------
	// create resource
	//----------------------------------------------------------
	void						cooker_thread_proc					();
	void						add_resource_to_create				(query_result *	query);
	void						dispatch_created_resources			();

	template <class query_list> // for query_list::get_next_of_object
	void						create_resources					(query_list const &, query_result * it_query, bool finalizing_thread);

	void						on_created_resource					(query_result * query);

	cook_base *					find_cook							(class_id_enum	class_id);

	//----------------------------------------------------------
	// translating query
	//----------------------------------------------------------
	void						push_to_translate_query				(query_result * query);
	template <class queries_list>
	void						translate_queries					(queries_list const &, query_result * to_translate_query);
	void						translate_query						(query_result * query);

	//----------------------------------------------------------
	// decompressing resources
	//----------------------------------------------------------
	void						add_resource_to_decompress			(query_result * query);
	void						decompress_resource					(query_result * query);
	void						decompress_resources				();
	void						dispatch_decompressed_resources		();

	//----------------------------------------------------------
	// counting queries being processed by resource manager per thread
	//----------------------------------------------------------
	threading::atomic32_type	m_pending_queries_count;
	threading::atomic32_type	m_uncooked_queries_count;			//	note: only for editor to show progress
	void						decrement_uncooked_queries_count	() { R_ASSERT(m_uncooked_queries_count != 0); threading::interlocked_decrement(m_uncooked_queries_count); }

	void						on_added_queries					(u32 num_queries);
	void						on_dispatched_queries				(u32 num_queries);

	//----------------------------------------------------------
	// save_generated_resource
	//----------------------------------------------------------
	void						push_generated_resource_to_save		(query_result * in_query);
	void						save_generated_resource				(query_result * in_query);
	void						save_generated_resources			();

	//----------------------------------------------------------
	// delayed registering of cookers
	//----------------------------------------------------------
	threading::atomic32_type	m_num_cook_registrators;
	bool						cooks_are_registering				() const { return m_num_cook_registrators != 0; }

	//----------------------------------------------------------
	// misc
	//----------------------------------------------------------
	void						dump_unmanaged_resource_leaks			();
	
	//----------------------------------------------------------
	// mounts
	//----------------------------------------------------------
	void						do_mount_mounts_path				();
	fs_new::native_path_string										m_mounts_path;
	threading::atomic32_type										m_do_mount_mounts_path;

	//----------------------------------------------------------
	// allocation
	//----------------------------------------------------------
	void						on_allocated_raw_resource			(query_result * query);
	void						dispatch_allocated_raw_resources	();
	allocate_functionality											m_allocate_functionality;
	
	debug_state					m_debug;

	//----------------------------------------------------------	
	// name_registry	
	//----------------------------------------------------------	
	typedef hash_multiset<	name_registry_entry, 
							name_registry_entry *, 
							& name_registry_entry::next_in_hashset, 
							xray::detail::fixed_size_policy<1024*32>, 
							detail::name_registry_hash, 
							detail::name_registry_equal	> name_registry_type;

	name_registry_type												m_name_registry;
	threading::mutex												m_name_registry_mutex;

	threading::mutex &			name_registry_mutex ()				{ return m_name_registry_mutex; }
	name_registry_type &		name_registry		()				{ return m_name_registry; }

	typedef intrusive_list<name_registry_entry, name_registry_entry *, & name_registry_entry::next_to_delete> name_registry_delete_queue;
	name_registry_delete_queue										m_name_registry_delete_queue;

	void						push_name_registry_to_delete		(name_registry_entry * entry);
	void						delete_name_registry_entries		();
	void						finalize_name_registry				();

	//----------------------------------------------------------	
	// global callbacks
	//----------------------------------------------------------	
	void						on_query_finished_callback			(resource_base * resource);

	//----------------------------------------------------------	
	// init new queries
	//----------------------------------------------------------	
	void   						init_new_autoselect_quality_queries	();
	void   						init_new_autoselect_quality_query	(query_result * query);

	//----------------------------------------------------------	
	// pending operations counters
	//----------------------------------------------------------	
	void						change_count_of_pending_mount_operations		(long change);
	void						change_count_of_pending_helper_query_for_mount	(long change);	

	threading::atomic32_type	m_pending_mount_operations_count;
	threading::atomic32_type	m_pending_mount_helper_query_count;
	
	threading::atomic32_type	m_count_of_pending_query_with_fat_it;
	void						change_count_of_pending_query_with_fat_it		(long change);


	//----------------------------------------------------------	
	// fs watcher notifications
	//----------------------------------------------------------	

	void						push_to_call_tasks_finished_callback	(query_result * query);
	void						dispatch_tasks_finished_callback		(query_result * query, bool finalizing_thread);

	void						on_mounted								(vfs::base_node<> * const node);

private:
	threading::mutex												m_ready_queries_mutex;
	typedef intrusive_double_linked_list<query_result, 
										 query_result *, 
										 & query_result::m_prev_in_generate_if_no_file_queue, 
										 & query_result::m_next_in_generate_if_no_file_queue,
										 threading::mutex>	generate_if_no_file_queue;
								 
	generate_if_no_file_queue										m_generate_if_no_file_queue;

	typedef	vector<device_manager*>									device_managers;
	device_managers													m_device_managers;

	bool															m_debug_single_thread;
	timing::timer													m_flush_timer;

	// we need this to prevent the following Code Analysis error
	// "function 'xray::intrusive_list<BaseWithMember,PointerType,MemberNext>::intrusive_list(void)' already has a body"
	//typedef intrusive_list<query_result, query_result *, &query_result::m_next_to_init> new_queries_list;
	typedef intrusive_list<query_result, query_result *, &query_result::m_union_nexts> new_queries_list;

	new_queries_list												m_new_queries_with_locked_fat_it;
	new_queries_list												m_new_queries_with_unlocked_fat_it;
	new_queries_list												m_new_queries_waiting_for_cook_register;
	new_queries_list												m_new_inorder_queries;
	new_queries_list												m_new_autoselect_quality_queries;

	intrusive_list<managed_resource, managed_resource *, &managed_resource::m_next_delay_delete>	m_delayed_delete_managed_resources;
	intrusive_list<unmanaged_resource, unmanaged_resource *, &unmanaged_resource::m_next_delay_delete>	
																	m_delayed_delete_unmanaged_resources;
	threading::atomic32_type										m_delay_delete_unmanaged_resources_count;

	// we need this to prevent the following Code Analysis error
	// "function 'xray::intrusive_list<BaseWithMember,PointerType,MemberNext>::intrusive_list(void)' already has a body"
	// intrusive_list<query_result, query_result *, &query_result::m_next_to_allocate_raw_resource>	m_queries_with_allocated_raw_resources;
	intrusive_list<query_result, query_result *, &query_result::m_union_nexts>	m_queries_with_allocated_raw_resources;
	
	threading::reader_writer_lock									m_thread_local_data_lock;
	thread_local_data_tree											m_thread_local_data;
	u32																m_tls_key_thread_local_data;

	threading::atomic32_type										m_resources_thread_id;
	threading::event												m_resources_wakeup_event;

	threading::atomic32_type										m_cooker_thread_id;
	threading::event												m_cooker_wakeup_event;

#if XRAY_FS_NEW_WATCHER_ENABLED
	uninitialized_reference<watcher>								m_watcher;
#endif

	typedef	fixed_vector<cook_base*, last_resource_class>			cooks_registry_type;
	cooks_registry_type												m_cooks_registry;

	// we need this to prevent the following Code Analysis error
	// "function 'xray::intrusive_list<BaseWithMember,PointerType,MemberNext>::intrusive_list(void)' already has a body"
	//typedef intrusive_list<query_result, query_result *, &query_result::m_next_to_create_resource>	create_resource_list;
	typedef intrusive_list<query_result, query_result *, &query_result::m_union_nexts>	create_resource_list;
	create_resource_list											m_resources_to_create;

	threading::atomic32_type										m_dispatching_created_resources;
	create_resource_list											m_created_resources;

	// we need this to prevent the following Code Analysis error
	// "function 'xray::intrusive_list<BaseWithMember,PointerType,MemberNext>::intrusive_list(void)' already has a body"
	//typedef intrusive_list<query_result, query_result *, &query_result::m_next_generated_to_save>		generated_resources_to_save_list;
	typedef intrusive_list<query_result, query_result *, &query_result::m_union_nexts>		generated_resources_to_save_list;
	generated_resources_to_save_list								m_generated_resources_to_save_list;

	//typedef intrusive_list<query_result, query_result *, &query_result::m_next_decompress>	decompress_list;
	typedef intrusive_list<query_result, query_result *, &query_result::m_union_nexts>	decompress_list;
	decompress_list													m_resources_to_decompress;
	decompress_list													m_decompressed_resources;

	threading::mutex												m_mount_and_fs_iterator_dispatch_mutex;
	threading::thread_id_type										m_mount_and_fs_iterator_dispatch_mutex_hold_thread_id;
	threading::thread_id_type										mount_and_fs_iterator_dispatch_mutex_hold_thread_id () const { return m_mount_and_fs_iterator_dispatch_mutex_hold_thread_id; }

	threading::mutex												m_transaction_mutex;

	intrusive_list<fs_task, fs_task *, & fs_task::m_next>			m_fs_tasks;
	intrusive_list<fs_task, fs_task *, & fs_task::m_next>			m_fs_sub_tasks;

	uninitialized_reference<ppmd_compressor>						m_compressor;

	threading::mutex												m_wait_and_dispatch_callbacks_mutex;

	query_finished_callback											m_query_finished_callback;
	query_finished_callback											get_query_finished_callback() const { return m_query_finished_callback; }

	static u32 const					max_resources_thread_sleep_period	=	300;

	timing::timer						m_self_wakeup_timer;
	bool								m_self_wakeuping;

	fs_new::synchronous_device_interface	m_sync_device;
	fs_new::asynchronous_device_interface &	m_hdd;
	fs_new::asynchronous_device_interface &	m_dvd;

	vfs::virtual_file_system			m_vfs;
	resources::mount_ptr				m_mounts_ptr;
	resources::mount_ptr				m_mounts_converted_ptr;

	//----------------------------------------------------
	// friends
	//----------------------------------------------------

	friend class						query_result;
	friend class						query_result_for_cook;
	friend class						queries_result;
	friend class						unmanaged_resource;
	friend class						managed_resource;
	friend class						managed_allocator;
	friend class						mount_by_config_helper;
	friend class						fs_task;
	friend class						cook_base;
	friend class						base_of_intrusive_base;
	friend class						fs_task_mount;
	friend class						fs_task_unmount;
	friend class						fs_task_erase;
	friend class						fs_task_composite;
	friend class						intrusive_fs_task_unmount_base;
	friend class						allocate_functionality;

	template <platform_pointer_enum pointer_for_fat_size>
	friend class						xray::fs::fat_node;

}; // class resources_manager

extern uninitialized_reference<resources_manager>		g_resources_manager;

} // namespace resources
} // namespace xray

#endif// RESOURCES_H_INCLUDED